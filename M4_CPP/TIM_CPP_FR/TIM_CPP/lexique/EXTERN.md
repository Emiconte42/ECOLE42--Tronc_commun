# `extern` — "Défini ailleurs"

> **TL;DR.** `extern` déclare un nom qui a un linkage externe mais **n'alloue pas de stockage** dans cette unité de traduction. Cela promet que le linker trouvera la définition dans un autre `.cpp` (ou une autre bibliothèque).

Lié : [`STATIC.md`](STATIC.md) · [`LIBRARIES.md`](../notions/tooling/LIBRARIES.md) · [`INLINE.md`](INLINE.md)

---

## 1. Le problème résolu par `extern`

Deux fichiers `.cpp` veulent partager une variable globale.

```cpp
// counter.cpp
int total = 0;
```

```cpp
// main.cpp
total++;            // ERROR — main.cpp ne sait pas que `total` existe
```

Le compiler compile chaque `.cpp` indépendamment. Lors de la compilation de `main.cpp`, il n'a jamais entendu parler de `total`. Il vous faut une déclaration :

```cpp
// main.cpp
extern int total;   // declaration uniquement — aucun stockage alloué ici
total++;            // compile. Le linker résout vers la définition dans counter.cpp.
```

C'est exactement le même modèle que pour les fonctions :

```cpp
extern void log(const char *);   // declaration; définie ailleurs
```

Pour les fonctions, `extern` est le comportement par défaut — une déclaration de fonction sans corps a automatiquement un linkage externe. Pour les variables, vous écrivez `extern` explicitement.

---

## 2. Déclaration vs définition

```
┌─────────────────────────────────────────────────────────────────┐
│  declaration                  │  definition                      │
│  "ce nom existe, voici son    │  "allouer du stockage / générer  │
│   type"                       │   du code pour ce nom"           │
├───────────────────────────────┼──────────────────────────────────┤
│  extern int x;                │  int x;                          │
│  void f();                    │  void f() { … }                  │
│  class C;                     │  class C { … };                  │
└───────────────────────────────┴──────────────────────────────────┘
```

La règle **One Definition Rule (ODR)** stipule que chaque nom requiert **au maximum une** définition à l'échelle du programme. Vous pouvez avoir de nombreuses déclarations.

Pattern pour une variable globale partagée :

```
   header  shared.hpp:    extern int total;     ← declaration
   source  shared.cpp:    int total = 0;        ← LA definition
   source  main.cpp:      #include "shared.hpp"
                          total++;
   source  log.cpp:       #include "shared.hpp"
                          log(total);
```

Trois TU voient la déclaration ; une seule fournit le stockage.

---

## 3. Vue du linker

```
   phase de compilation (chaque .cpp est indépendant) :
   ┌─────────────────────────────────────────────────────────┐
   │ main.cpp     │ le compiler voit :                      │
   │              │   extern int total;                      │
   │              │   …utilise total…                        │
   │              │ ← émet dans main.o :                     │
   │              │     référence UNDEFINED vers "total"     │
   ├──────────────┼──────────────────────────────────────────┤
   │ counter.cpp  │ le compiler voit :                      │
   │              │   int total = 0;                         │
   │              │ ← émet dans counter.o :                  │
   │              │     symbole DEFINED "total" dans .data   │
   └─────────────────────────────────────────────────────────┘

   phase d'édition de liens :
   ┌─────────────────────────────────────────────────────────┐
   │ le linker résout le total UNDEFINED dans main.o         │
   │ vers le total DEFINED dans counter.o.                   │
   │ Les deux fichiers .o pointent désormais vers le même    │
   │ emplacement .data.                                      │
   └─────────────────────────────────────────────────────────┘
```

Inspecter avec `nm` :

```bash
$ nm main.o
                 U total           ← U = undefined; nécessite la résolution du linker
$ nm counter.o
000000000000000c D total           ← D = symbole de données défini

$ nm program
000000000201020 D total           ← binaire lié : un seul symbole
```

---

## 4. Les variantes

### 4.1 Simple `extern int x;` — déclaration

Indique au compiler que le nom existe, a pour type `int`, et possède un linkage externe. Aucun stockage n'est alloué ; le linker doit trouver une définition.

### 4.2 `extern int x = 5;` — définition (rare, généralement une erreur)

Si vous fournissez un initialiseur, `extern` est **silencieusement ignoré**. Cela devient une **définition**. Ne faites pas cela dans les headers — chaque TU qui l'inclut définit désormais `x` et le linker explose.

### 4.3 `extern "C"` — linkage C

Indique au compiler de ne pas **mangler** le symbole de ce nom. Utilisé pour interagir avec les bibliothèques C :

```cpp
extern "C" {
    void c_function(int);
    int  c_global;
}
```

#### Qu'est-ce que le name mangling ?

Le C++ supporte l'overload. Deux fonctions peuvent partager le même nom au niveau du code source tant que leurs signatures diffèrent. Le linker, cependant, voit des *symboles*, pas des signatures. Le compiler applique donc un **mangling** sur le nom source pour en faire un symbole unique qui encode la signature :

```
   void f(int)        →    _Z1fi          (Itanium ABI ; varie selon le compiler)
   void f(double)     →    _Z1fd
   void f(int, int)   →    _Z1fii
```

```
$ nm prog | grep ' f'
0000000000401120 T _Z1fi
0000000000401140 T _Z1fd
```

Le C ne fait rien de tel — `void f(int)` est simplement `f`. Appeler une fonction C depuis le C++ nécessite donc de désactiver le mangling :

```cpp
extern "C" int strcmp(const char *, const char *);   // symbole C : strcmp
```

C'est pourquoi les headers C utilisés en C++ s'encapsulent dans :

```c
#ifdef __cplusplus
extern "C" {
#endif

void mylib_init(void);
int  mylib_sum(int, int);

#ifdef __cplusplus
}
#endif
```

#### `extern "C"` et l'overload

À l'intérieur de `extern "C"`, **vous ne pouvez pas faire d'overload** — il n'y a pas de signature dans le symbole pour lever l'ambiguïté. Le compiler rejettera :

```cpp
extern "C" {
    void f(int);
    void f(double);   // ERROR — symbole dupliqué après désactivation du mangling
}
```

#### `extern "C"` et les function pointers

Le *type* d'un function pointer inclut son linkage de langage :

```cpp
extern "C" int qsort_cmp(const void*, const void*);    // linkage C
int (*p)(const void*, const void*) = qsort_cmp;        // techniquement une incompatibilité de type sur
                                                        // les compilers stricts
```

En pratique, la plupart des compilers acceptent la conversion, mais le standard traite les types de fonction `extern "C"` et les types de fonction `extern "C++"` comme distincts.

---

## 5. `extern` pour les templates (C++11+ uniquement)

```cpp
extern template class std::vector<int>;   // C++11 — ne pas instancier ici
```

Indique au compiler de ne pas instancier le template dans cette TU ; la définition sera trouvée au moment de l'édition de liens. Réduit le temps de compilation et la taille du binaire. **Non disponible en C++98** — à ignorer pour 42.

---

## 6. `extern` et `static` — opposés

```cpp
static int x;        // linkage INTERNE — local à cette TU
extern int y;        // linkage EXTERNE — visible par les autres TUs
int       z;         // aussi un linkage externe au scope de fichier (mais une définition ici)
```

| Modificateur | Linkage | Stockage |
|---|---|---|
| `static` (scope de fichier) | interne | défini ici, non visible à l'extérieur |
| `extern` (sans init) | externe | déclaré ici, défini ailleurs |
| `extern` (avec init) | externe | défini ici (l'initialiseur l'emporte) |
| aucun | externe | défini ici |

---

## 7. Patterns courants

### 7.1 Partager une variable globale entre les TU

```cpp
// app_state.hpp
#ifndef APP_STATE_HPP
# define APP_STATE_HPP

extern bool g_debug;        // declaration

#endif
```

```cpp
// app_state.cpp
#include "app_state.hpp"

bool g_debug = false;       // definition — exactement une
```

### 7.2 Appeler une bibliothèque C depuis le C++

```cpp
// my_glue.cpp
extern "C" {
    #include <unistd.h>
    #include <fcntl.h>
}

int x = open("file", O_RDONLY);
```

La plupart des headers standard C modernes s'encapsulent déjà dans `extern "C"`, vous n'avez donc pas besoin de faire cela pour `<cstdio>`, `<cstring>`, etc.

### 7.3 Éviter `extern` pour les constantes — préférer `static const` dans un header

```cpp
// à éviter :
// header.hpp:  extern const int MAX;
// source.cpp:  const int MAX = 1024;

// à préférer :
// header.hpp:
class Config {
public:
    static const int MAX = 1024;   // peut être utilisé comme expression constante
};
```

---

## 8. Trucs & astuces

### 8.1 La valeur d'une variable `extern` est inconnue au compile time

```cpp
extern int N;
int arr[N];     // ERROR — N n'est pas une expression constante
```

Si vous voulez une constante pour la taille d'un tableau, utilisez `enum { N = 10 };` ou `static const int N = 10;` (dans une class).

### 8.2 `extern` n'échappe pas au namespace

```cpp
namespace ft {
    extern int counter;   // ft::counter
}

int counter;              // counter global — symbole différent
```

### 8.3 La redéclaration est autorisée

```cpp
extern int x;
extern int x;             // OK — déclarations multiples autorisées
extern int x;
int x = 0;                // et une seule définition
```

### 8.4 Si vous oubliez de définir un `extern`, le linker vous le signale

```
undefined reference to `total'
```

Différent d'une erreur de compilation — les déclarations `extern` compilent toujours.

### 8.5 N'utilisez pas `extern` pour "exposer" un état privé

Si vous voulez une variable globale, demandez-vous s'il ne devrait pas plutôt s'agir d'un singleton, d'un membre de class, ou simplement d'un paramètre. Les variables globales `extern` sont un risque pour la maintenance ; privilégiez l'encapsulation.

---

## 9. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `undefined reference to 'foo'` (linker) | Déclaration `extern` sans définition correspondante | Ajouter `int foo;` dans un `.cpp`, ou lier la bibliothèque qui le définit |
| `multiple definition of 'foo'` (linker) | Deux `.cpp` font tous deux `int foo = 0;` | En transformer un en `extern int foo;`, conserver une seule définition |
| `'extern' applied to function definition` | `extern void f() { ... }` | Retirer `extern` sur les définitions (c'est le comportement par défaut pour les fonctions) |
| `cannot convert C function to C++` | Incompatibilité de linkage de langage sur un function pointer | Faire concorder les deux côtés (`extern "C"` ou aucun) |
---

## 10. Résumé visuel

```
            ┌──────────────────────────────────────┐
            │           extern keyword             │
            └────────────────┬─────────────────────┘
                             │
       ┌─────────────────────┼─────────────────────┐
       ▼                     ▼                     ▼
  extern T x;          extern "C" {…}         extern template …
  ───────────          ──────────────         ─────────────────
  declaration only,    suppresses C++ name    suppress instantiation
  definition lives     mangling: lets you     in this TU (C++11+,
  in another TU.       call/expose C ABIs.    not in 42 C++98).
                       no overloading
                       allowed inside.
```

---

## 11. Pratique

1. Pourquoi `extern int x = 5;` définit-il plutôt qu'il ne déclare ? *(L'initialiseur en fait une définition ; `extern` est ignoré silencieusement.)*
2. Pourquoi une fonction C appelée depuis du C++ doit-elle être encapsulée dans `extern "C"` ? *(Les symboles C ne sont pas manglés ; sans `extern "C"`, le compiler C++ manglerait l'appel, et le linker ne trouverait pas le symbole C.)*
3. Quelle est la différence entre `static` et `extern` au niveau du file scope ? *(Linkage interne vs externe. Static = privé à la TU ; extern = visible par le linker.)*
4. Peut-on marquer une fonction `extern "C"` ET l'overload ? *(Non — le symbole C non manglé ne peut pas lever l'ambiguïté des overloads.)*