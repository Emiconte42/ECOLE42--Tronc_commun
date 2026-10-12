# `inline` — Un indice, une promesse au linker et une astuce pour les fichiers header

> **TL;DR.** À l'origine un *indice* pour intégrer (inliner) un appel de fonction. Dans les compilers modernes, l'indice d'inlining est généralement ignoré — mais **l'effet sur le linker** (permettre à la même fonction d'apparaître dans plusieurs translation units sans erreur de "multiple definition") est bien réel et fondamental.

Voir aussi : [`STATIC.md`](STATIC.md) · [`MAKEFILE_CPP.md`](../notions/tooling/MAKEFILE_CPP.md) · [`LIBRARIES.md`](../notions/tooling/LIBRARIES.md)

---

## 1. Deux objectifs réunis en un seul mot-clé

```cpp
inline int square(int x) { return x * x; }
```

Cette déclaration fait deux choses :

1. **Donne un indice** au compiler : *"merci de considérer l'inlining des appels à cette fonction — substitue le corps au point d'appel au lieu de générer une instruction `call`."*
2. **Indique au linker** : *"cette fonction peut être définie dans plusieurs translation units, mais elles ont toutes le même corps ; choisis-en simplement une."*

L'effet (1) est le plus célèbre. L'effet (2) est ce qui permet concrètement aux headers de rester utilisables.

---

## 2. Qu'est-ce que l'inlining (effet sur le compiler) ?

Sans inlining :

```cpp
int square(int x) { return x * x; }
int main() { return square(7); }
```

```asm
square(int):
    mov eax, edi
    imul eax, edi
    ret
main:
    mov edi, 7
    call square(int)
    ret
```

Avec inlining (le compiler décide d'inliner) :

```asm
main:
    mov eax, 49        ; constant-folded after inlining
    ret
```

L'appel de fonction disparaît complètement. Le corps est *substitué* au point d'appel, puis l'optimizer peut effectuer un repliement de constantes (constant folding) supplémentaire.

```
   sans inlining               avec inlining
   ────────────────            ─────────────
   main → call square          main { x*x }   ← copié sur place
                                                 puis replié en 49
```

### Quand est-ce rentable ?

- Les fonctions minuscules (une ou deux opérations) — l'overhead de l'appel peut être bien supérieur au travail effectué.
- Les fonctions où le corps débloque des optimisations supplémentaires (constant folding, élimination de branches mortes).
- Les boucles critiques (hot loops) où chaque cycle compte.

### Quand est-ce néfaste ?

- Les fonctions volumineuses copiées à de nombreux points d'appel → **code bloat** → pression sur le cache d'instructions → peut devenir *plus lent*.
- Les fonctions récursives qui ne sont pas réduites par l'inlining.

### Ce que font les compilers modernes

Les compilers avec `-O2` (g++, clang) prennent leurs **propres** décisions d'inlining en se basant sur des heuristiques (taille de la fonction, nombre de points d'appel, données de profilage). Votre mot-clé `inline` est généralement une *suggestion qu'ils peuvent ignorer*. Inversement, ils inlineront des fonctions non-`inline` dès que cela s'avère avantageux.

L'optimizer supporte également `__attribute__((always_inline))` (gcc/clang) et `__forceinline` (MSVC) pour des exigences strictes — mais ces directives ne sont pas standard.

---

## 3. L'effet sur le linker — pourquoi les headers ne cassent pas

Voici un problème en C : si vous placez une fonction non triviale dans un header,

```c
// utils.h
int square(int x) { return x * x; }
```

Chaque fichier `.c` qui inclut le header obtient sa propre copie de `square` dans son `.o`. L'édition de liens de deux d'entre eux échoue :

```
multiple definition of `square'
```

En C, la règle est **une seule définition** sur l'ensemble du programme — la **One Definition Rule (ODR)**.

Le C++ assouplit cette règle : une fonction `inline` peut avoir de **multiples définitions identiques** à travers différentes translation units. Le linker en conserve une et élimine les autres. C'est ce qui rend les headers utilisables pour des corps de fonctions non triviaux.

```cpp
// utils.hpp
inline int square(int x) { return x * x; }   // peut être inclus n'importe où
```

```
   utils.hpp inclus dans :
   ┌──────────┐  ┌──────────┐  ┌──────────┐
   │   a.cpp  │  │   b.cpp  │  │   c.cpp  │
   └────┬─────┘  └────┬─────┘  └────┬─────┘
        │             │             │
        ▼             ▼             ▼
       a.o           b.o           c.o
      contient      contient      contient
      square()      square()      square()
        │             │             │
        └─────── linker ────────────┘
                   │
                   ▼
              un seul square()
              dans le binaire
```

Sans `inline`, le linker refuserait l'édition de liens.

### `inline` implicite — fonctions membres définies dans le corps de la class

```cpp
// MyClass.hpp
class MyClass {
public:
    int value() const { return _value; }   // corps dans la class → IMPLICITEMENT inline
private:
    int _value;
};
```

Toute fonction membre dont le corps réside dans la définition de la class est implicitement `inline`. C'est pourquoi les définitions de class dans les headers ne brisent pas le linker, même lorsqu'elles incluent des corps de fonctions.

La même règle s'applique à :
- Les fonctions membres définies à l'intérieur de la class.
- Les function templates (les templates sont implicitement inline au sens de l'ODR).

### `inline` implicite — fonctions `constexpr` (C++11+, hors norme 42 C++98)

En C++11, une fonction `constexpr` est implicitement inline. Non pertinent pour les modules 42, mais bon à savoir.

---

## 4. Le pattern de la norme 42

Déclaration de la class dans le `.hpp`, définitions dans le `.cpp` — c'est ce qu'impose la norme 42 :

```cpp
// MyClass.hpp
class MyClass {
public:
    int value() const;   // déclaration uniquement
};
```

```cpp
// MyClass.cpp
int MyClass::value() const { return _value; }   // définition hors ligne
```

Les définitions hors ligne (out-of-line) ne sont **pas** implicitement inline. Ce n'est pas un problème — elles résident dans exactement un seul `.cpp`, donc l'ODR est respectée.

Si vous souhaitez inliner un petit accesseur :

```cpp
// MyClass.hpp
class MyClass {
public:
    int value() const;
private:
    int _value;
};

inline int MyClass::value() const { return _value; }   // hors de la class mais dans le header
```

Cela fonctionne grâce à l'effet sur le linker : le header peut être inclus par de nombreux `.cpp`, et `inline` rend cette duplication légale.

---

## 5. Vue matérielle — le coût de l'appel que vous cherchez à éviter

Un appel de fonction non inliné sur x86_64 (ABI System V) :

```
appelant :                     appelé :
   push args vers stack/regs      prologue : push rbp; mov rbp,rsp; sub rsp, N
   call    foo                    corps : exécution du code
                                  épilogue : leave; ret
   continue
```

Coûts :
- Empilement des arguments (push) / configuration des registres
- Instruction `call` (empilement de l'adresse de retour, saut)
- Prologue de la fonction (mise en place de la stack frame)
- Épilogue de la fonction (démontage de la stack frame)
- `ret` (dépilement de l'adresse de retour, saut de retour)
- Prédiction de branchement + perturbation du pipeline
- Éventuel vidage (spill) des registres sauvegardés par l'appelant

Pour une fonction aussi petite que `int square(int x) { return x*x; }`, cet overhead peut être **plus lourd que le calcul lui-même**. L'inlining supprime l'intégralité de ce surcoût.

```
                  coût d'un appel ≈ 2–10 cycles + spills de registres
                  coût du corps   ≈ 1 cycle  (imul)

     avant inline :  10+ cycles
     après inline :  1   cycle  (souvent réduit à 0 cycle par constant folding)
```

---

## 6. Quand l'inlining se retourne contre vous — le code bloat

```
   square(7)   square(8)   square(9)   ...   square(99)
   inliné      inliné      inliné            inliné

   si chaque corps inliné fait 30 octets :
   100 × 30 = 3 Ko de code dupliqué
```

Si ces points d'appel sont dispersés à travers plusieurs boucles, les corps dupliqués évinceront **d'autres** instructions du cache L1 d'instructions. Un appel partagé unique `call square` prendrait bien moins de place.

Les compilers le savent et renoncent à l'inlining lorsque les corps sont volumineux ou que les points d'appel sont nombreux. Votre mot-clé `inline` n'a généralement pas le dessus sur leur jugement.

---

## 7. Trucs & astuces

### 7.1 Ne mettez pas `inline` partout

Ce n'est pas un gain gratuit. Le compiler applique souvent l'inlining sans votre intervention, et en abuser indique simplement "je ne sais pas ce que je fais".

### 7.2 `inline` ne signifie PAS "rapide"

Ce mot-clé concerne l'ODR et fournit une indication, il ne garantit pas la performance. Profilez avant d'optimiser.

### 7.3 Le pattern des bibliothèques header-only

Si vous concevez une bibliothèque ne contenant qu'un header (un seul `.hpp`, aucun `.cpp`), chaque fonction libre doit être déclarée `inline` (ou être un template, qui est implicitement inline pour l'ODR) :

```cpp
// my_header.hpp
inline int helper(int x) { return x * 2; }

template <typename T>
T add(T a, T b) { return a + b; }   // templates: ODR-safe dans les headers
```

### 7.4 Static + inline — des outils différents

```cpp
// dans un header
static int helper(int x) { return x * 2; }
```

versus

```cpp
inline int helper(int x) { return x * 2; }
```

- `static` → chaque `.cpp` obtient sa **propre** copie à internal linkage (adresses différentes pour `&helper` dans différentes TUs !).
- `inline` → tous les `.cpp` partagent **un seul et unique** symbole après l'édition de liens (même adresse partout).

Dans un header, `inline` est presque systématiquement ce que vous recherchez.

### 7.5 Fonctions récursives

```cpp
inline int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}
```

Les fonctions récursives peuvent être marquées `inline` (cela reste un indice), mais le compiler ne peut inliner que jusqu'à une profondeur finie. Avec `-O2 -fno-inline-functions` ou simplement selon ses heuristiques, il émettra généralement un véritable appel après un ou deux déroulements.

### 7.6 Inspecter ce que le compiler a produit

```bash
$ g++ -O2 -S -o - file.cpp | less
```

Cherchez `call square` — s'il a disparu, la fonction a été inlinée. S'il est présent, elle ne l'a pas été.

Également très utile : [godbolt.org](https://godbolt.org/) vous permet de visualiser l'assembleur généré pour n'importe quel extrait avec n'importe quel flag de compiler.

### 7.7 `inline` et fonctions virtuelles

Une fonction membre `virtual` peut être marquée `inline`. Le compiler ne peut toutefois inliner les appels virtuels que lorsqu'il **connaît statiquement** le type dynamique — ce qui est rare. Ne comptez pas sur l'inlining lors d'un dispatch virtuel.
---

## 8. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `multiple definition of 'foo'` (linker) | Fonction avec un corps dans un header, non marquée `inline` | Ajouter `inline`, ou déplacer le corps dans un `.cpp` |
| Build lent / binaire volumineux | Utilisation excessive d'`inline` partout | Déplacer les corps dans un `.cpp` pour les fonctions non triviales |
| L'inlining ne se produit pas à `-O0` | Optimizer désactivé | Utiliser `-O2` ; ou `__attribute__((always_inline))` pour forcer l'inlining |

---

## 9. Résumé visuel

```
                ┌────────────────────────────────┐
                │     keyword: inline            │
                └─────────────┬──────────────────┘
                              │
              ┌───────────────┴────────────────┐
              ▼                                ▼
        compiler hint                    linker permission
        ─────────────                    ─────────────────
        "consider                        "this function
         substituting                    may have multiple
         the body at                     identical definitions
         call sites"                     across TUs"
              │                                │
        modern compilers                 lets you put non-trivial
        decide on their own.             function bodies in headers.
        the keyword is mostly            REQUIRED for free functions
        decorative for inlining.         in header-only libraries.
              │                                │
              └────────── one keyword ─────────┘

   implicit inline:
     • member functions defined in the class body
     • function templates
     • C++11+ constexpr functions
```

---

## 10. Pratique

1. Pourquoi le fait de placer une free function non-template avec un corps dans un header échoue-t-il au link ? *(ODR — chaque TU qui l'inclut définit sa propre copie. `inline` permet au linker de dédupliquer.)*
2. Les member functions définies à l'intérieur du corps de la class sont-elles inline ? *(Oui — implicitement.)*
3. Est-ce qu'`inline` rendra mon code plus rapide ? *(Peut-être — mais l'optimizer décide généralement sans vous. Profilez, ne devinez pas.)*
4. Pourquoi les templates sont-ils « ODR-safe » dans les headers ? *(Les templates ont des règles de linkage implicites similaires à `inline` ; les instanciations identiques sont fusionnées.)*