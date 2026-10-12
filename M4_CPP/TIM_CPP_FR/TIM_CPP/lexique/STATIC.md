# `static` — Un mot-clé, trois rôles différents

> **TL;DR.** `static` fait **trois choses sans rapport** selon l'endroit où vous le placez. Linkage interne au niveau de la portée de fichier ; stockage persistant au niveau de la portée de fonction ; membre à l'échelle de la class au niveau de la portée de class. Ils partagent un nom et presque rien d'autre.

Voir aussi : [`CONST.md`](CONST.md) · [`EXTERN.md`](EXTERN.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#11.%20static%20members) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md)

---

## 1. Les trois significations en un coup d'œil

```
┌────────────────────────────────────────────────────────────────────┐
│ où vous le placez         │ ce que cela signifie                   │
├───────────────────────────┼────────────────────────────────────────┤
│ portée de fichier (.cpp)  │ linkage interne — le symbole reste     │
│ static int counter;       │ uniquement dans cette translation unit │
├───────────────────────────┼────────────────────────────────────────┤
│ dans une fonction         │ static storage — variable active toute │
│ void f() { static int n;} │ la vie du programme, init 1 seule fois │
├───────────────────────────┼────────────────────────────────────────┤
│ dans une class            │ membre de class — une copie partagée   │
│ static int count;         │ par toutes les instances ; Class::     │
└────────────────────────────────────────────────────────────────────┘
```

Ce sont trois fonctionnalités de langage complètement différentes qui se trouvent utiliser le même mot-clé. Ne les confondez pas.

---

## 2. `static` au niveau de la portée de fichier — linkage interne

```cpp
// utils.cpp
static int helper(int x) { return x * 2; }   // non visible depuis les autres fichiers .cpp
```

La fonction (ou variable) n'est **pas exportée** vers le linker. Deux fichiers `.cpp` peuvent chacun définir leur propre `static int helper(...)` sans entrer en collision.

### Vue hardware/linker

```
            translation unit       object file        executable
         (un .cpp + headers)       (fichier .o)     (binaire linké)
                  │                  │                   │
non-static fn ────┼─► global symbol ─┼─► visible ───────┼─► un symbole pour
                  │                  │   pour le linker  │   tout le programme
                  │                  │                   │
    static fn ────┼─► local symbol  ─┼─► non exporté     │   aucun risque
                  │                  │                   │   de collision
```

Inspectez-le avec `nm` :
```
$ nm utils.o
0000000000000000 T extern_helper       ← T = symbole text global
0000000000000010 t static_helper       ← t = symbole text local (minuscule)
```

### Pourquoi l'utiliser

- **Évite les collisions de noms** entre translation units.
- **Indique au compiler** que la fonction ne peut pas être appelée depuis l'extérieur de ce fichier → elle peut être inlinée plus agressivement, et le dead-code est éliminé plus facilement.
- **Documente l'intention** — « c'est interne ».

### Alternative moderne — anonymous namespaces

```cpp
namespace {
    int helper(int x) { return x * 2; }   // linkage interne également
    int counter = 0;
}
```

`static` au niveau de la portée de fichier est l'approche de style C ; les anonymous namespaces sont la manière C++. Les deux sont acceptés par la norme 42. Les anonymous namespaces fonctionnent également pour les types (on ne peut pas appliquer `static` à une class).

---

## 3. `static` dans une fonction — variable locale persistante

```cpp
int next_id() {
    static int counter = 0;   // initialisé une seule fois, au premier appel
    return ++counter;
}

next_id();   // 1
next_id();   // 2
next_id();   // 3 — counter a survécu entre les appels
```

La variable est **construite une seule fois** (à la première entrée) et **détruite à la fin du programme**.

### Où réside-t-elle ?

Pas sur la stack. Elle réside dans le **data segment** (initialisée) ou le **BSS** (initialisée à zéro), exactement comme une variable globale — mais son **nom** n'est visible qu'à l'intérieur de la fonction.

```
frames d'appel de fonction             static storage (durée de vie du programme)
┌────────────────┐ stack pointer       ┌───────────────────────────────┐
│  next_id() #3  │ ─── lit/écrit ─────►│    counter = 3                │
└────────────────┘                     └───────────────────────────────┘
       │
dépilé au return
       ▼
┌────────────────┐
│  caller frame  │
└────────────────┘
```

### L'initialisation est « magique »

La première fois que la fonction est appelée, le compiler exécute l'initialiseur. Pour éviter de l'exécuter deux fois (en code multithreadé, à chaque entrée, etc.), le compiler émet un flag caché ainsi que, en C++11+, un guard one-shot thread-safe.

L'initialisation en C++98 n'était **pas thread-safe** par défaut. Si votre variable locale `static` est un objet complexe, deux threads peuvent atteindre le constructeur simultanément. (Pour les modules CPP mono-thread de 42, cela n'a pas d'importance, mais gardez-le à l'esprit.)

```cpp
void log() {
    static std::ofstream f("trace.log");   // ouvert une seule fois, au premier appel
    f << "called\n";
}                                          // f est fermé à la fin du programme
```

### Astuce — le singleton de Meyers

Le singleton le plus propre en C++ :

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger inst;   // construit au premier appel, détruit à la sortie
        return inst;
    }
private:
    Logger() {}
    Logger(const Logger&);                 // désactivé
    Logger& operator=(const Logger&);      // désactivé
};
```

Pas de `new`, pas de `delete`, aucune fuite. La construction est lazy.

---

## 4. `static` dans une class — membre à l'échelle de la class

```cpp
class PhoneBook {
    static int _count;        // déclaration : partagé par toutes les instances
public:
    PhoneBook()  { ++_count; }
    ~PhoneBook() { --_count; }
    static int getCount() { return _count; }
};

int PhoneBook::_count = 0;    // DÉFINITION — doit résider dans exactement un seul .cpp
```

Deux parties :
1. **Déclaration** dans la class. Indique au compiler que le membre existe.
2. **Définition** dans un `.cpp`. Réserve l'espace de stockage. **L'oublier est l'erreur de linker n°1** — `undefined reference to PhoneBook::_count`.

### Disposition en mémoire

```
PhoneBook a, b, c;

stack                              static (.data)
┌─────────────┐                    ┌──────────────────┐
│      a      │                    │                  │
│(pas de_count│ ─────┐              │  PhoneBook::    │
│  dedans !)  │      │              │     _count = 3  │
├─────────────┤      ├─────────────►│                  │
│      b      │      │              └──────────────────┘
├─────────────┤      │              ▲
│      c      │ ─────┘              │
└─────────────┘                     les trois instances
                                    partagent cet int unique
```

`sizeof(PhoneBook)` n'inclut **pas** `_count`. Les membres non-static constituent l'empreinte mémoire par instance.

### Fonctions membres static

```cpp
static int getCount();   // pas de pointer 'this'
```

Différences par rapport à une fonction membre non-static :
- **Aucun paramètre `this` implicite.** La fonction ne sait pas quelle instance l'a appelée.
- **Ne peut pas accéder aux membres non-static** (ils nécessiteraient un `this`).
- **Ne peut pas être `const`** — il n'y a aucune instance sur laquelle appliquer const.
- **Ne peut pas être `virtual`** — il n'y a aucune instance sur laquelle faire de dispatch.
- **Appelée via `Class::fn()`** — bien que `obj.fn()` compile également pour des raisons de compatibilité syntaxique.

### Membres intégraux `static const` — le cas particulier

```cpp
class Buffer {
    static const int CAPACITY = 1024;   // déclaration ET initialiseur in-class
public:
    char data[CAPACITY];                // peut être utilisé comme une expression constante
};
```

Pour les types **intégraux ou énumérations** déclarés `static const`, C++98 vous permet de placer l'initialiseur directement dans le corps de la class. Vous n'avez généralement pas besoin d'une définition `.cpp` séparée, sauf si vous prenez son adresse (`&CAPACITY`).

Pour les types non intégraux (par exemple `static const std::string`), vous avez toujours besoin d'une définition dans un `.cpp`.

---

## 5. Ce que `static` ne signifie PAS

Les personnes venant du C/Java commettent souvent ces erreurs :

- ❌ `static` ne signifie pas « constant ». Utilisez [`const`](CONST.md) pour cela.
- ❌ `static` ne signifie pas « private ». Utilisez les spécificateurs d'accès ([PUBLIC/PRIVATE/PROTECTED](PUBLIC_PRIVATE_PROTECTED.md)).
- ❌ Un membre `static` n'est pas « global ». Sa portée est restreinte à la class.
- ❌ Une variable locale `static` n'est pas « exempte d'overhead ». L'init au premier appel effectue une vérification de guard cachée (légère mais réelle).
- ❌ `static` dans un header au niveau de la portée de fichier donne **à chaque .cpp qui l'inclut sa propre copie** — ce qui est généralement incorrect. Utilisez `inline` (C++17) ou `extern` + une définition unique.

---

## 6. Résumé des durées de stockage

```
┌──────────────────┬────────────────────┬──────────────────────────┐
│ durée            │ quand existe-t-il  │ exemples                 │
├──────────────────┼────────────────────┼──────────────────────────┤
│ static           │ tout le programme  │ globales, locales static,│
│                  │                    │ membres de class static  │
├──────────────────┼────────────────────┼──────────────────────────┤
│ automatic        │ portée du bloc     │ locales ordinaires       │
├──────────────────┼────────────────────┼──────────────────────────┤
│ dynamic          │ new → delete       │ objets alloués sur heap  │
├──────────────────┼────────────────────┼──────────────────────────┤
│ thread (C++11+)  │ durée du thread    │ thread_local x;          │
└──────────────────┴────────────────────┴──────────────────────────┘
```
Le mot-clé `static` est impliqué dans la **première ligne**.

---

## 7. The static initialization order fiasco

Lorsque vous avez deux objets `static` à portée de fichier (ou des objets globaux non-`static`) dans des translation units différentes qui dépendent l'un de l'autre :

```cpp
// a.cpp
extern Foo foo;
Bar bar = makeBar(foo);   // dépend de la construction de foo

// b.cpp
Foo foo;
```

L'ordre entre les translation units est **non spécifié** (*unspecified*). `bar` peut être initialisé avant que `foo` ne soit construit → undefined behavior.

**Correctif — l'idiome construct-on-first-use :**

```cpp
// a.cpp
Bar& bar() {
    static Bar instance = makeBar(foo());   // construit au premier appel
    return instance;
}

// b.cpp
Foo& foo() {
    static Foo instance;
    return instance;
}
```

Cela fonctionne car **l'ordre d'initialisation des variables static-locales est bien défini** — le premier appel l'emporte.

---

## 8. Trucs & astuces

### 8.1 Default-initialize à coût nul

Les variables `static` (et globales) sont **initialisées à zéro** (*zero-initialized*) avant que tout code utilisateur ne s'exécute :

```cpp
static int n;             // n vaut 0 — garanti
static int *p;            // p vaut NULL — garanti
static std::string s;     // s vaut "" — default-constructed
```

En revanche, `int n;` à l'intérieur d'une fonction contient des données résiduelles non initialisées (*garbage*).

### 8.2 `static` pour des constantes privées au module

```cpp
// parser.cpp
static const int MAX_TOKENS = 4096;     // visible uniquement ici
```

Pas de pollution de header ; pas d'export de symbole.

### 8.3 `static_cast` n'a aucun rapport

`static_cast<T>(x)` réutilise le mot-clé pour une fonctionnalité différente — le cast à la compilation. Voir [`CASTS.md`](CASTS.md). Ne les confondez pas.

### 8.4 Débogage par compteur

Un test de cycle de vie classique pour la forme canonique orthodoxe (OCF) :

```cpp
class Tracer {
    static int _alive;
public:
    Tracer()  { ++_alive; }
    Tracer(const Tracer&)  { ++_alive; }
    ~Tracer() { --_alive; }
    static int alive() { return _alive; }
};
int Tracer::_alive = 0;
```

Si `Tracer::alive() != 0` à la sortie du programme, vous avez une fuite.

### 8.5 Ne retournez pas une reference vers une variable static locale qui dépend de paramètres

```cpp
const std::string& bad(int n) {
    static std::string s;
    s = std::to_string(n);   // écrasé à chaque appel !
    return s;                // l'appelant détient une référence qui mute à son insu
}
```

### 8.6 `static` dans les headers — le piège

```cpp
// utils.hpp
static int counter = 0;     // chaque .cpp qui inclut ceci obtient SA PROPRE copie de counter
```

Ce n'est presque certainement pas ce que vous voulez. Utilisez `extern int counter;` dans le header et `int counter = 0;` dans exactement un seul `.cpp`. Ou encapsulez-le dans une class avec un membre `static` (dans ce cas, la règle d'une seule définition s'applique toujours).

---

## 9. Erreurs courantes

| Erreur | Cause | Correctif |
|---|---|---|
| `undefined reference to ClassName::member` (linker) | Oubli de la définition dans le `.cpp` d'un membre static de class | Ajouter `int ClassName::member = 0;` dans un `.cpp` |
| `cannot declare member function 'X::f' to have static linkage` | Utilisation de `static` sur la définition d'une member function hors de la class | Retirer `static` de la définition hors-classe ; le conserver dans la déclaration |
| `'this' is unavailable for static member functions` | Utilisation de `this` dans une static member function | Ne pas le faire — passer l'objet explicitement |
| `multiple definition of 'foo'` (linker) | `int foo;` (ni `static`, ni `extern`) dans un header → dupliqué dans chaque TU | Déplacer vers un seul `.cpp` |

---

## 10. Résumé visuel

```
                  ┌──────────────────────────┐
                  │      keyword: static     │
                  └────────────┬─────────────┘
                               │
        ┌──────────────────────┼──────────────────────┐
        ▼                      ▼                      ▼
  at file scope          inside a function       inside a class
  ───────────            ──────────────────      ──────────────
  internal linkage       static storage          one copy shared
  (not exported)         (lives forever,         by all instances
                          init once)              (Class::member)

  use for:               use for:                 use for:
  - .cpp helpers         - call counters          - shared state
  - file-private state   - lazy construction      - instance counters
  - Meyers singleton                              - class constants
```

---

## 11. Pratique

1. Pourquoi le fait de placer `static int n = 0;` dans un header crée-t-il un `n` par fichier source ? *(Chaque TU voit sa propre définition ; `static` lui donne un internal linkage.)*
2. Pourquoi un membre static nécessite-t-il une définition dans un `.cpp` ? *(Le corps de la class n'est qu'une déclaration ; l'espace de stockage doit être alloué exactement une seule fois.)*
3. Pourquoi une static member function ne peut-elle pas être `virtual` ? *(Le dispatch virtuel a besoin de `this` ; les static member functions n'en possèdent pas.)*
4. Qu'est-ce qui ne va pas avec `static T& bad() { static T t = makeT(); return t; }` si `makeT()` utilise elle-même une autre fonction retournant une variable static locale ? *(Rien ! C'est l'idiome construct-on-first-use — il résout le static-init-order fiasco.)*