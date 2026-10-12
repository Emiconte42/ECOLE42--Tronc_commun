# 📖 GLOSSAIRE — Le vocabulaire C++, décodé

> Un dictionnaire guidé de chaque **concept** technique que vous rencontrerez dans les modules CPP de 42 — RAII, lvalue, vtable, deep copy… Chaque entrée comprend : une **définition en une ligne**, un **exemple précis**, une **représentation mentale**, et un **lien** vers le fichier d'étude approfondie.
>
> Vous cherchez un **mot-clé** du langage (`const`, `switch`, `virtual`…) ? Rendez-vous sur [`INDEX.md`](INDEX.md) — une page par mot-clé, les 63 sont couverts.
> Lisez cette page de manière linéaire pour construire votre carte mentale. Cliquez sur n'importe quel lien dès qu'un terme devient flou.

```
                ┌──────────────────────────────────────┐
                │              C++  =                  │
                │  C  +  Classes  +  Templates  +  STL │
                │  +  RAII   +  Exceptions   +  RTTI   │
                └──────────────────────────────────────┘
                          ▲       ▲       ▲
                          │       │       │
                       SAFETY   REUSE  ABSTRACTION
                       (no UB) (generic) (objects)
```

**La bibliothèque en un coup d'œil :**

| Section | Ce qui s'y trouve |
|---|---|
| [`lexique/`](INDEX.md) | Vous êtes ici — l'ensemble des 63 mots-clés + ce glossaire des concepts |
| [`notions/`](../notions/INDEX.md) | Les concepts par thème : fondamentaux, OOP, STL, I/O, outillage |
| [`projets/`](../projets/INDEX.md) | Les guides pas à pas 42 de CPP00 à CPP04 + webserv |

---

## 🗺️ Master map

```
                       ┌───────────────────────┐
                       │   YOUR SOURCE FILE    │
                       └──────────┬────────────┘
                                  │
           ┌──────────────────────┼──────────────────────┐
           │                      │                      │
       PREPROCESSOR           COMPILER                 LINKER
       #include / #define     .cpp → .o               .o → executable
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
              TYPES            OBJECTS           TEMPLATES
              int, bool,       class, struct,    template<T>
              char, ...        vtable, ...       monomorphized
                │                 │                 │
                └─────► RAII ◄────┴────► STL ◄──────┘
                       (lifetime)    (reusable code)
```

---

## 🔤 Saut rapide alphabétique

| | |
|:---:|---|
| **A** | [Abstract class](#Abstract%20class) · [Abstraction](#Abstraction) · [Aggregate](#Aggregate) · [Allocator](#Allocator) · [API](#API) · [Argument](#Argument%20vs%20Parameter) · [Assignment op](#Assignment%20operator) · [Attribute](#Attribute) |
| **B** | [Base class](#Base%20class) · [Block](#Block) · [Bool](#Bool) · [Build](#Build) |
| **C** | [Cast](#Cast) · [Class](#Class) · [Composition](#Composition) · [Const](#Const) · [Const-correctness](#Const-correctness) · [Constructor](#Constructor) · [**Contract**](#Contract) · [Copy ctor](#Copy%20constructor) |
| **D** | [Dangling pointer](#Dangling%20pointer) · [Data member](#Attribute) · [Declaration](#Declaration%20vs%20Definition) · [Deep copy](#Deep%20vs%20Shallow%20copy) · [Definition](#Declaration%20vs%20Definition) · [Delete](#Delete) · [Destructor](#Destructor) · [Diamond problem](#Diamond%20problem) |
| **E** | [Encapsulation](#Encapsulation) · [Enum](#Enum) · [Exception](#Exception) · [Explicit](#Explicit) · [Expression](#Statement%20vs%20Expression) · [Extern](#Extern) |
| **F** | [Field](#Attribute) · [Forward declaration](#Forward%20declaration) · [Friend](#Friend) · [Functor](#Functor) |
| **G** | [Getters/Setters](#Getters%20and%20setters) |
| **H** | [Header](#Header%20file) · [Header guard](#Header%20guard) · [Heap](#Heap) |
| **I** | [Identifier](#Identifier) · [Implementation](#Interface) · [Inheritance](#Inheritance) · [Initialization](#Initialization%20vs%20Assignment) · [Initializer list](#Initializer%20list) · [Inline](#Inline) · [Instance](#Instance) · [Interface](#Interface) · [Iterator](#Iterator) |
| **L** | [Lifetime](#Lifetime) · [Linkage](#Linkage) · [Literal](#Literal) · [Lvalue](#Lvalue) |
| **M** | [Macro](#Preprocessor) · [Member function](#Method) · [Memory leak](#Memory%20leak) · [**Method**](#Method) · [Mutable](#Mutable) |
| **N** | [Name mangling](#Name%20mangling) · [Namespace](#Namespace) · [New](#New) |
| **O** | [Object](#Object) · [OCF](#Orthodox%20Canonical%20Form%20%28OCF%29) · [ODR](#One%20Definition%20Rule%20%28ODR%29) · [Operator overload](#Operator%20overloading) · [Overload](#Overload) · [Override](#Override) |
| **P** | [Parent class](#Subclass) · [POD](#Aggregate) · [Pointer](#Pointer) · [Polymorphism](#Polymorphism) · [Preprocessor](#Preprocessor) · [Private](#Access%20specifiers) · [Pure virtual](#Pure%20virtual) |
| **R** | [RAII](#RAII) · [Reference](#Reference) · [RTTI](#RTTI) · [Rvalue](#Lvalue) |
| **S** | [Scope](#Scope) · [Shallow copy](#Deep%20vs%20Shallow%20copy) · [Signature](#Signature) · [Sizeof](#Sizeof) · [Slicing](#Object%20slicing) · [Stack](#Stack) · [Statement](#Statement%20vs%20Expression) · [Static](#Static) · [STL](#STL) · [Stream](#Stream) · [String](#String) · [Struct](#Struct) · [Subclass](#Subclass) · [Superclass](#Subclass) |
| **T** | [Template](#Template) · [This](#This) · [Throw](#Throw) · [Translation unit](#Translation%20Unit) · [Try/catch](#Try%20and%20catch) · [Type](#Type) · [Typedef](#Typedef) · [Typeid](#Typeid) · [Typename](#Typename) |
| **U** | [Undefined behavior](#Undefined%20behavior%20%28UB%29) · [Using](#Using) |
| **V** | [Virtual](#Virtual) · [Void](#Void) · [Volatile](#Volatile) · [Vptr](#Vtable) · [Vtable](#Vtable) |

---

## 🌉 Synonymes — passerelles entre vocabulaire courant et C++

Vous entendrez ces mots courants de la part des correcteurs, dans des articles de blog ou dans d'autres langages. Voici comment chacun est *réellement nommé* en C++.

| Vous pouvez dire… | Le C++ l'appelle… | En une ligne |
|---|---|---|
| **Method** | Member function | Une fonction qui appartient à une class. Même chose. |
| **Field** / **Attribute** / **Property** | Data member / Member variable | Une variable qui vit à l'intérieur d'un object. |
| **Instance** | Object | Un exemple concret et vivant d'une class. |
| **Subclass** / **Child class** | Derived class | Celle qui hérite. |
| **Superclass** / **Parent class** | Base class | Celle dont on hérite. |
| **Interface** | Pure virtual / Abstract class | Un type qui indique **quoi** faire mais pas **comment**. |
| **Implementation** | Definition (d'une class/fonction) | La partie **comment**. |
| **API** | Public interface | L'ensemble des méthodes/types publics que vous exposez. |
| **Header** | Fichier de déclaration (`.hpp`) | Ce que les autres fichiers incluent via `#include`. |
| **Cpp file** | Translation unit (après preprocessing) | Un `.cpp` + tout ce qu'il inclut. |
| **Build** | Preprocess + compile + link | Le pipeline de `.cpp` → exécutable. |
| **Library function** | Fonction d'une bibliothèque / STL | Ex. `std::sort` provient *de la STL*. |
| **Variable** | Object (dans le jargon de la norme) | Oui — même `int x;` est techniquement un "object". |
| **Type-cast** / **Conversion** | Cast | Les quatre variantes se trouvent dans [`CASTS.md`](CASTS.md). |

> **Règle mentale :** lorsqu'un évaluateur de 42 (ou un manuel) dit *method*, il fait référence à ce que le C++ appelle une **member function**. Les termes sont interchangeables.

---

## 🧱 1. Fondations & syntaxe

> Les briques fondamentales. Si elles sont bancales, rien d'autre ne tient.
> 📚 Étude approfondie : [`fundamentals/BASICS.md`](../notions/fundamentals/BASICS.md) · [`fundamentals/CONTRACTS.md`](../notions/fundamentals/CONTRACTS.md)

### Contract
Une **promesse entre deux parties de code** — caller↔callee, class↔utilisateur, vous↔compiler. La majeure partie de la syntaxe C++ (`const`, `private`, `virtual`, `explicit`, `noexcept`…) est le langage permettant d'écrire des contrats appliqués par le compiler. 📚 [`fundamentals/CONTRACTS.md`](../notions/fundamentals/CONTRACTS.md)

```cpp
int sum(const std::vector<int>& v);
//      ─────────────────────── 3 promises in one signature:
//                              don't copy · don't modify · return an int
```

```
   CALLER  ─── preconditions ─►  CALLEE
   CALLER  ◄── postconditions ── CALLEE
            break it ⇒ UB / compile error
```

### Type
Une étiquette qui indique au compiler **(a) combien d'octets** réserver et **(b) quelles opérations** sont permises. Les types sont vérifiés à la compilation — c'est pourquoi le C++ intercepte des erreurs que le C ne voit jamais.

```cpp
int    i = 42;       // 4 bytes,  arithmetic
double d = 3.14;     // 8 bytes,  floating ops
bool   b = true;     // 1 byte,   logical ops
```

### Object
Toute région de mémoire dotée d'un type et d'une lifetime. Pas uniquement une "instance de class" — `int x;` est également un object.

```
┌──────────┐       memory:  [.... 42 ....]   ← 4 bytes
│  int x   │       lifetime: from `int x;` to `}`
└──────────┘
```

### Scope
La région textuelle où un nom est visible. Trois variantes : **block** (`{}`), **class**, **namespace**.

```cpp
int g = 1;           // namespace scope (global)
void f() {
    int x = 2;       // block scope — dies at `}`
    {
        int x = 3;   // shadows outer x
    }
}
```

### Argument vs Parameter
**Parameter** = le paramètre formel dans la signature de la fonction. **Argument** = la valeur réelle fournie sur le lieu de l'appel.
```cpp
void greet(std::string name);   // `name` est le paramètre
greet("Alice");                  // "Alice" est l'argument
```

### Header file
Un fichier `.hpp` (ou `.h`) contenant des **declarations** que d'autres fichiers `#include`. Son rôle est d'indiquer au compiler ce qui existe, et non la façon dont c'est implémenté.

```
       header (.hpp)              source (.cpp)
       ┌──────────────┐          ┌──────────────┐
       │ class Foo {  │          │ #include     │
       │   void bar();│  ←──┐    │ "Foo.hpp"    │
       │ };           │     └────┤              │
       └──────────────┘          │ void Foo::   │
                                 │ bar() {...}  │
                                 └──────────────┘
```
Les header guards (`#ifndef FOO_HPP`) empêchent les explosions dues aux inclusions multiples.

### Namespace
Une boîte logique pour les noms. Résout les collisions du type "deux bibliothèques possèdent toutes les deux un `print`". 📚 [`lexique/NAMESPACE.md`](NAMESPACE.md)

```cpp
namespace ft { int abs(int x) { return x < 0 ? -x : x; } }
ft::abs(-5);            // explicite
using ft::abs;          // en importe un
```

### Using
Trois rôles : importer un nom unique, importer un namespace entier, ou réexposer un membre de base. 📚 [`lexique/USING.md`](USING.md)

### Typedef
Crée un alias pour un type existant. Purement à la compilation (compile-time) — aucun nouveau type n'est créé. 📚 [`lexique/TYPEDEF.md`](TYPEDEF.md)

```cpp
typedef unsigned long size_t;   // size_t EST unsigned long
```

### Bool
Un véritable type booléen. `true == 1`, `false == 0`. Utilisez `std::boolalpha` pour afficher des mots. 📚 [`lexique/BOOL.md`](BOOL.md)

```cpp
std::cout << std::boolalpha << (1 < 2);   // "true"
```

### Void
"Aucune valeur." Trois utilisations : type de retour de fonction, pointer générique (`void*`), liste de paramètres vide. 📚 [`lexique/VOID.md`](VOID.md)

### Enum
Constantes entières nommées. Devient type-safe avec `enum class` (C++11). 📚 [`lexique/ENUM.md`](ENUM.md)

```cpp
enum Level { DEBUG, INFO, WARN, ERROR };  // 0,1,2,3
```

### Identifier
Tout nom que vous donnez à une variable, une fonction, un type, etc. Lettres/chiffres/underscore, ne commence pas par un chiffre, sensible à la casse.

### Block
Une région `{ … }`. Définit un scope. Les variables nées à l'intérieur meurent à `}`.

### Statement vs Expression
**Expression** = produit une valeur (`a + b`, `f(x)`). **Statement** = effectue une action (`int x = 1;`, `if (...) {}`). La plupart des expressions deviennent des statements lorsqu'elles sont suivies d'un `;`.

```cpp
a + b;          // expression statement (valeur ignorée)
int x = a + b;  // declaration statement
```

### Literal
Une valeur écrite directement dans le code source : `42`, `3.14`, `"hello"`, `'a'`, `true`. Les literals codés en dur et éparpillés dans le code sont appelés **magic numbers** — donnez-leur plutôt un nom avec `const`.

```cpp
const int MAX_RETRIES = 3;   // ✅ nommé
for (int i = 0; i < 3; ++i)  // ⚠️ c'est quoi 3 ?
```

### Initialization vs Assignment
**Initialization** = donner une valeur à un objet **à sa naissance**. **Assignment** = modifier la valeur d'un objet **après** sa création. Opérations différentes, opérateurs différents dans votre class (copy constructor vs `operator=`).

```cpp
int x = 5;      // initialization
x = 10;         // assignment

Dog a("Rex");   // initialization (appelle le constructeur)
Dog b = a;      // initialization (copy constructor — surprise !)
b = a;          // assignment (operator=)
```

### Lifetime
La fenêtre durant laquelle un objet existe en mémoire. Déterminée par l'endroit où vous le déclarez :

| Storage | Naissance à | Mort à |
|---|---|---|
| Automatique (stack) | declaration `{` | `}` correspondant |
| Static / global | démarrage du programme | fin du programme |
| Dynamique (heap) | `new` | `delete` |
| Sous-objet membre | naissance de l'objet englobant | mort de l'objet englobant |

> RAII = "lier une ressource à un lifetime afin que le nettoyage soit automatique."

---

## 🔧 2. Modèle de compilation — ce par quoi passe votre code

> Avant que votre programme ne s'exécute, quatre outils l'ont déjà manipulé.
> 📚 Pour aller plus loin : [`tooling/MAKEFILE_CPP.md`](../notions/tooling/MAKEFILE_CPP.md) · [`tooling/LIBRARIES.md`](../notions/tooling/LIBRARIES.md)

```
   .cpp + .hpp ──► [PREPROCESSOR] ──► [COMPILER] ──► .o ──► [LINKER] ──► executable
                       ▲                  ▲                    ▲
                  développe #includes, parse, vérifie les    résout les noms
                  développe macros     types, émet assembleur à travers les fichiers .o
```

### Build
**Build = preprocess → compile → link.** Trois phases que l'on confond souvent :
1. **Preprocess** — substitution de texte (`#include`, `#define`, `#ifdef`).
2. **Compile** — transforme chaque `.cpp` en un fichier objet `.o` (code machine avec des références non résolues).
3. **Link** — assemble tous les fichiers `.o` (et bibliothèques) en un seul exécutable.

### Preprocessor
L'étape de substitution de texte qui s'exécute avant la compilation. Les **macros** (`#define`) sont ses remplacements de texte aveugles — elles ne connaissent ni les types ni les scopes. À utiliser avec modération.

```cpp
#define MAX(a,b) ((a)>(b)?(a):(b))   // fonctionne… la plupart du temps
MAX(i++, j++);                        // 💥 i ou j incrémenté deux fois
const int LIMIT = 100;                // ✅ préférez un const typé
```

### Header guard
`#include "foo.hpp"` colle littéralement le contenu du fichier à cet emplacement. Pour éviter de le coller deux fois (et d'enfreindre la One Definition Rule), chaque header reçoit un guard :

```cpp
#ifndef FOO_HPP
#define FOO_HPP
// declarations…
#endif
```

### Translation Unit
Un fichier `.cpp` **plus** chaque fichier qu'il `#include` de manière transitive, une fois le preprocessor terminé. Le compiler travaille sur une TU à la fois. Le linker les assemble.

```
   foo.cpp  +  iostream  +  Foo.hpp  +  string  →  une grande TU
                          ↓
                     compiler
                          ↓
                       foo.o
```

### Declaration vs Definition
La distinction la plus éclairante du langage.

- **Declaration** = "ce nom existe avec ce type." N'alloue rien.
- **Definition** = "voici la chose." Alloue / fournit le corps.

```cpp
extern int g;            // declaration — promesse
int g = 42;              // definition — stockage réel

void f();                // declaration — la fonction existe
void f() { /* … */ }     // definition — corps de la fonction

class Foo;               // forward declaration
class Foo { int x; };    // definition
```

Vous pouvez déclarer plusieurs fois. Vous devez **définir exactement une fois** (par programme pour les objets/fonctions, par TU pour les types). C'est la…

### One Definition Rule (ODR)
Chaque entité doit avoir exactement **une seule definition** dans l'ensemble du programme. La violer = programme mal formé (erreurs de linker ou, pire, compilé silencieusement de travers). Les headers contiennent des *declarations* ; les fichiers `.cpp` contiennent des *definitions*. `inline` et les templates sont particuliers — ils peuvent résider dans des headers car le compiler traite leurs definitions comme compatibles avec l'ODR.

### Forward declaration
Une *declaration sans le corps*, utilisée pour briser des dépendances circulaires ou pour éviter d'inclure un header lourd.

```cpp
class Window;                  // forward declaration
void render(Window* w);        // pointer/ref suffit — pas besoin du corps
```

### Signature
L'identité d'une fonction aux yeux du compiler : **nom + types des paramètres** (le type de retour ne compte pas, sauf pour les templates). Deux fonctions peuvent coexister si leurs signatures diffèrent — c'est l'overload.

```cpp
int  f(int);          // signature : f(int)
int  f(double);       // signature différente — overload OK
double f(int);        // 💥 même signature, retour conflictuel — erreur
```

### Linkage
"Le linker peut-il voir ce nom depuis une autre translation unit ?"

| Linkage | Ce que cela signifie | Exemples |
|---|---|---|
| **External** | Nom visible à travers les TUs (par défaut pour les globales/fonctions) | `int g;` au scope de fichier |
| **Internal** | Nom visible uniquement dans sa propre TU | `static int g;`, anonymous namespace |
| **No linkage** | Local — ne concerne pas le linker | variables locales au scope de block |

### Name mangling
Le compiler **encode** les signatures de fonctions en symboles uniques pour le linker afin que l'overload et les namespaces fonctionnent. Le C ne fait pas cela — c'est ce que désactive `extern "C"`.

```cpp
int f(int)        // symbole mangled comme  _Z1fi
int f(double)     // symbole mangled comme  _Z1fd
extern "C" int f();   // symbole brut        f
```

```
   C++ source        →  symbole mangled     →  linker
   "ft::Vec::push"   →  "_ZN2ft3Vec4pushEi" →  unique
```

C'est aussi pourquoi mélanger différentes versions de compiler peut causer un cauchemar au niveau du linker — ils peuvent appliquer un name mangling différent.

---

## 🧠 3. Mémoire & lifetime

> Où résident vos octets. Se tromper sur ce schéma → segfaults, fuites, comportement indéfini (undefined behavior).
> 📚 Pour aller plus loin : [`fundamentals/MEMORY.md`](../notions/fundamentals/MEMORY.md)

### Stack
Mémoire automatique, rapide, LIFO. Les variables nées sur la stack meurent à l'accolade fermante `}` — gratuitement.

```
   ┌─────────────────┐  ← adresses hautes
   │   main's frame  │
   ├─────────────────┤
   │   f()'s  frame  │
   ├─────────────────┤
   │   g()'s  frame  │  ← stack pointer
   ├ ─ ─ ─ ─ ─ ─ ─ ─ ┤
   │     (free)      │
   ├─────────────────┤
   │      heap       │
   └─────────────────┘  ← adresses basses
```

### Heap
Mémoire dynamique que vous demandez et devez restituer. Plus lente, mais flexible.

```cpp
int *p = new int(42);   // heap
delete p;               // restituez-la, sinon vous créez une fuite
```

### New
Alloue **et** construit. Renvoie un pointer typé. 📚 [`lexique/NEW.md`](NEW.md)
```cpp
T *p   = new T;          // single object
T *arr = new T[10];      // array
```

### Delete
Détruit **et** désalloue. La forme doit correspondre à `new`. 📚 [`lexique/DELETE.md`](DELETE.md)

```cpp
delete p;        // matches new
delete[] arr;    // matches new[]
```

```
   new T   ↔  delete       ✅
   new T[] ↔  delete[]     ✅
   new T   ↔  delete[]     💥 UB
   new T[] ↔  delete       💥 UB
```

### Pointer
Une variable qui contient une adresse. Peu coûteux, nullable, réassignable. Style C.

```cpp
int  x = 5;
int *p = &x;    // p stores the address of x
*p = 10;        // x is now 10
```

### Reference
Un alias — un autre nom pour un objet existant. Non-null, non réassignable. 📚 [`fundamentals/REFERENCE.md`](../notions/fundamentals/REFERENCE.md)

```cpp
int  x = 5;
int &r = x;     // r IS x
r = 10;         // x is now 10
```

```
        Pointer                 Reference
   ┌─────────────────┐    ┌─────────────────┐
   │  p ──► [ 42 ]   │    │  r ═══ [ 42 ]   │
   │  can be null    │    │  cannot be null │
   │  can re-aim     │    │  bound for life │
   └─────────────────┘    └─────────────────┘
```

### Memory leak
Une allocation sur le heap qui n'est jamais libérée avec `delete`. Les octets sont inaccessibles jusqu'à la fin du processus. RAII (ci-dessous) est le remède.

### Dangling pointer
Un pointer dont la cible a déjà été supprimée/détruite. Le lire = undefined behavior.

```cpp
int *p = new int(7);
delete p;
*p = 42;        // 💥 dangling — p still holds the old address
```

### RAII
**Resource Acquisition Is Initialization.** L'idiome C++ le plus important : lier toute ressource (mémoire, fichier, verrou, socket) à la durée de vie d'un objet. Le destructeur la libère automatiquement.

```cpp
{
    std::ifstream f("data.txt");  // open
    // use f
}                                  // f.close() called by destructor
```

### Lvalue
**lvalue** = a un nom et une adresse (vous pouvez le placer à gauche de `=`). **rvalue** = un temporaire, sans adresse persistante.

```cpp
int x = 42;     // x is an lvalue, 42 is an rvalue
x = 10;         // ✅ assigning to lvalue
42 = x;         // ❌ can't assign to rvalue
```

### Sizeof
Taille en octets au moment de la compilation d'un type ou d'une expression. 📚 [`lexique/SIZEOF.md`](SIZEOF.md)

```cpp
sizeof(int)   // 4 (typical)
sizeof(char)  // 1 (always)
```

### Undefined behavior (UB)
Le compiler est **autorisé à faire n'importe quoi** lorsque votre code enfreint les règles : planter, fonctionner correctement, formater votre disque. L'UB n'est pas une erreur d'exécution — c'est une violation de contrat. Exemples : déréférencer null, lire de la mémoire non initialisée, dépassement de capacité signé (signed overflow).

---

## 🛡️ 4. Type qualifiers & storage

> Décorations sur un type qui changent les règles du jeu.

### Const
"Promesse : je ne modifierai pas ceci à travers ce nom." 📚 [`lexique/CONST.md`](CONST.md)

```cpp
const int x = 42;       // x cannot be reassigned
int const *p;           // pointer to const int  (read right-to-left)
int *const p;           // const pointer to int
const int *const p;     // both
```

### Const-correctness
La discipline consistant à marquer tout ce qui ne subit pas de mutation avec `const`. Cela documente l'intention **et** permet au compiler d'intercepter les bugs.

```cpp
class Vec {
public:
    int  size() const;   // ← const member: doesn't modify *this
    void push(int);      // ← non-const: modifies *this
};
```

### Static
Trois rôles : 📚 [`lexique/STATIC.md`](STATIC.md)
1. Linkage **local au fichier** (remplace le `static` du C)
2. Variable locale **persistante** (conservée entre les appels)
3. Membre **à l'échelle de la class** (une seule copie partagée par toutes les instances)

```cpp
class Counter {
    static int count;     // shared
};
int Counter::count = 0;   // must define out-of-class
```

### Mutable
L'échappatoire légitime ("escape hatch") à `const`. Permet à un membre spécifique d'être modifié depuis une méthode `const` (caches, mutexes). 📚 [`lexique/MUTABLE.md`](MUTABLE.md)

### Volatile
"N'optimise pas les lectures/écritures de ceci." Pour les E/S mappées en mémoire et les gestionnaires de signaux. **Pas** pour la synchronisation de threads. 📚 [`lexique/VOLATILE.md`](VOLATILE.md)

### Explicit
Bloque les conversions implicites sournoises des constructeurs à argument unique. 📚 [`lexique/EXPLICIT.md`](EXPLICIT.md)

```cpp
class S { public: S(int); };
S a = 5;        // ✅ implicit conversion (surprising)

class T { public: explicit T(int); };
T b = 5;        // ❌ refused
T b(5);         // ✅ direct init still fine
```

### Inline
Une indication au compiler d'inliner l'appel **et** un assouplissement de la One-Definition Rule pour les fonctions définies dans les headers. 📚 [`lexique/INLINE.md`](INLINE.md)

### Extern
"Défini ailleurs — merci de linker, n'alloue rien." Plus `extern "C"` pour l'interopérabilité avec le C. 📚 [`lexique/EXTERN.md`](EXTERN.md)

```cpp
extern "C" int c_func(int);  // disable C++ name mangling
```

---

## 🧬 5. Object orientation

> Regrouper **données + comportement** + appliquer des **invariants**.
> 📚 Approfondissement : [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md) · [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

### Class
Un type défini par l'utilisateur regroupant des membres (données) et des fonctions membres (comportement), privés par défaut. 📚 [`lexique/CLASS.md`](CLASS.md)

```cpp
class Dog {
    std::string name_;            // private data
public:
    Dog(std::string n);           // constructor
    void bark() const;            // behavior
};
```

### Struct
La même chose qu'une `class` — sauf que les membres sont **publics par défaut**. Convention : `struct` pour les données pures, `class` pour les "vrais" objets avec des invariants. 📚 [`lexique/STRUCT.md`](STRUCT.md)

### Instance
Un exemple concret et vivant d'une class — aussi appelé un **objet**. `Dog rex;` crée une instance de `Dog`. La class est l'emporte-pièce ; l'instance est le biscuit.

```
        class Dog              instances
        ┌──────────┐          ┌──────┐ ┌──────┐ ┌──────┐
        │ blueprint│  ──────► │ rex  │ │ buddy│ │ luna │
        └──────────┘          └──────┘ └──────┘ └──────┘
```

### Method
Aussi appelée une **fonction membre** — même chose, deux noms. Une fonction qui appartient à une class et reçoit un pointer `this` masqué vers l'objet courant. La plupart des langages disent "méthode" ; le standard C++ dit "fonction membre". Utilisez ce que votre public préfère.

```cpp
class Dog {
public:
    void bark() const;   // a method (a.k.a. member function)
};
rex.bark();              // method call
```

### Attribute
Aussi appelé **champ** (field) ou **data member** — trois noms pour la même chose : une variable qui réside à l'intérieur d'un objet. Le C++ l'appelle un *data member* ; Java/UML disent *field* ; la littérature orientée objet dit *attribute*. Même concept.

```cpp
class Dog {
    std::string name_;   // ← data member / field / attribute
    int         age_;    // ← also a data member
};
```

### Access specifiers
Contrôle de visibilité au moment de la compilation. Zéro coût à l'exécution. 📚 [`lexique/PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md)

| Specifier | Visible depuis… |
|---|---|
| `public` | n'importe où |
| `protected` | la class **et** ses classes dérivées |
| `private` | uniquement la class elle-même |

```
              ┌────────────────┐
              │  public:  ✅   │  ← outside world
              │  protected: ⚠   │  ← children only
              │  private: 🔒   │  ← same class only
              └────────────────┘
```

### Encapsulation
Masquage des détails internes derrière une interface stable. Concrètement : données `private` + méthodes `public`. La class devient responsable du maintien de ses propres invariants.

### This
Le premier paramètre implicite de chaque fonction membre non-static : un pointer vers l'objet courant. 📚 [`lexique/THIS.md`](THIS.md)

```cpp
void Dog::rename(std::string n) {
    this->name_ = n;     // explicit
    name_ = n;           // implicit — same thing
}
```

### Constructor
Un membre spécial qui **initialise** un objet. Pas de type de retour. Même nom que la class. S'exécute une fois à la création de l'objet.

```cpp
Dog::Dog(std::string n) : name_(n) { }
                       // ↑ initializer list (preferred)
```

### Initializer list
La syntaxe `: x_(arg), y_(arg)` qui initialise les membres **avant** que le corps du constructor ne s'exécute. Obligatoire pour les membres `const` et reference.

```cpp
class A {
    const int id_;
    int      &ref_;
public:
    A(int i, int &r) : id_(i), ref_(r) { } // only way
};
```

### Destructor
Un membre spécial portant le nom `~ClassName()` qui s'exécute en fin de vie. Le point d'ancrage de nettoyage pour le RAII.

```cpp
~Dog() { /* free resources, log, etc. */ }
```

### Copy constructor
Construit un nouvel objet **à partir** d'un objet existant. Signature : `T(const T& other)`.

```cpp
Dog rex("Rex");
Dog clone(rex);          // ← copy ctor
Dog clone2 = rex;        // ← also copy ctor (not assignment!)
```

### Assignment operator
Remplace le contenu d'un objet **déjà construit**. Signature : `T& operator=(const T& other)`.

```cpp
Dog a("A"), b("B");
a = b;                   // ← assignment, not copy ctor
```

### Deep vs Shallow copy
**Shallow** = copie les octets (par défaut). **Deep** = clone également les ressources allouées sur le heap.

```
   shallow copy of struct{int*p}:   deep copy:
   a → [ p* ]──┐                    a → [ p* ]──► [42]
   b → [ p* ]──┘                    b → [ p* ]──► [42]
        both alias the same int      separate ints
```
Si vous faites `delete` sur des pointer copiés superficiellement (shallow copy) depuis deux destructors → **double-free**.

### Orthodox Canonical Form (OCF)
La "Règle des 4" canonique de 42 : chaque class gère son propre cycle de vie. 📚 [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

```cpp
class T {
public:
    T();                         // 1. default constructor
    T(const T& other);           // 2. copy constructor
    T& operator=(const T& rhs);  // 3. copy assignment
    ~T();                        // 4. destructor
};
```

### Aggregate
Un type "plain old data" (aussi appelé **POD**) — agencé en mémoire comme une struct C. Trivially copyable, sûr pour memcpy.

### Friend
Un passe-droit d'accès ciblé : cette fonction/class peut voir mes membres privés. À utiliser avec parcimonie. 📚 [`lexique/FRIEND.md`](FRIEND.md)

```cpp
class Box {
    int secret_;
    friend std::ostream& operator<<(std::ostream&, const Box&);
};
```

### Operator overloading
Définir ce que `+`, `<<`, `==`, etc. signifient pour votre class. 📚 [`oop/OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`lexique/OPERATOR.md`](OPERATOR.md)

```cpp
Vec operator+(const Vec& a, const Vec& b);
std::cout << my_obj;     // calls operator<<(cout, my_obj)
```

### Getters and setters
Des méthodes publiques pour un accès contrôlé aux données privées. 📚 [`oop/GETTERS_SETTERS.md`](../notions/oop/GETTERS_SETTERS.md)

```cpp
int  getAge() const   { return age_; }   // getter — const
void setAge(int a)    { age_ = a;    }   // setter
```

---

## 🌳 6. Inheritance & polymorphism

> Réutilisation + dispatch. La relation "est-un" (is-a).
> 📚 Pour aller plus loin : [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md)

### Inheritance
La class B "**est une**" class A — B réutilise l'interface et les données de A, puis en ajoute/affine.

```cpp
class Animal {            class Dog : public Animal {
public:                   public:
    void breathe();           void bark();
};                        };

   ┌──────────┐
   │  Animal  │  ← base class
   └────┬─────┘
        │ public
   ┌────▼─────┐
   │   Dog    │  ← derived class (gets breathe() for free)
   └──────────┘
```

### Base class
L'entité dont on hérite. L'opposé est la **derived class**, qui hérite. Synonymes : parent/child, super/sub.

### Subclass
**Subclass** et **superclass** sont des termes courants du quotidien — équivalents à **derived class** / **base class**. Java et la plupart des manuels de POO utilisent par défaut *subclass / superclass* ; la littérature C++ utilise par défaut *derived / base*. Interchangeables.

```
        Animal      ←── superclass / base / parent
          │
          ▼
        Dog         ←── subclass / derived / child
```

### Overload
Plusieurs fonctions avec le **même nom, des paramètres différents**. Résolu au compile time.

```cpp
void log(int);
void log(const std::string&);
void log(double);
```

### Override
Une derived class réimplémente une fonction **virtual** d'une base class. Résolu au runtime.

```cpp
class Animal { public: virtual void speak() const; };
class Dog : public Animal { public: void speak() const; };
                          // ↑ overrides Animal::speak
```

### Virtual
Active le polymorphism au runtime via la résolution par vtable. 📚 [`lexique/VIRTUAL.md`](VIRTUAL.md)

```cpp
Animal *a = new Dog;
a->speak();   // calls Dog::speak — only because speak is virtual
```

### Vtable
L'implémentation de `virtual`. Chaque class polymorphique reçoit une **vtable** (par class). Chaque instance porte un **vptr** caché (par objet) qui pointe vers elle.

```
   Dog object                vtable for Dog
   ┌──────────┐              ┌──────────────────┐
   │  vptr   ─┼─────────────►│ &Dog::speak      │
   ├──────────┤              │ &Animal::breathe │
   │  data... │              └──────────────────┘
   └──────────┘
```

### Pure virtual
Une fonction virtual sans implémentation : `= 0`. Force les derived classes à l'override.

```cpp
class Shape {
public:
    virtual double area() const = 0;   // pure virtual
};
```

### Abstract class
Une class avec au moins une fonction pure virtual. **Ne peut pas être instanciée** directement — seulement héritée.

```cpp
Shape s;             // ❌ abstract
Shape *p = new Circle(...);   // ✅
```

### Polymorphism
"Même appel, comportement différent selon le type réel." Le combo base-pointer + virtual + override.

```cpp
std::vector<Animal*> zoo;
for (auto *a : zoo) a->speak();   // each one speaks its own way
```

### Object slicing
Copier un objet dérivé **par valeur** dans une base — les parties dérivées sont tronquées.

```cpp
Dog d;
Animal a = d;     // 💥 only the Animal sub-object copied
a.speak();        // calls Animal::speak, NOT Dog::speak
```
Remède : passer/stocker des **references** ou des **pointers** vers la base.

### Diamond problem
Héritage multiple où deux parents partagent un grand-parent → ambiguïté.

```
        Animal
        /    \
     Dog    Cat
        \    /
       DogCat   ← which Animal does it have? virtual base solves it
```

---

## ⚙️ 7. Templates & generics

> Du code qui écrit du code. Programmation paramétrée par les types.
> 📚 Pour aller plus loin : [`advanced/TEMPLATES.md`](../notions/advanced/TEMPLATES.md) · [`lexique/TEMPLATE.md`](TEMPLATE.md)

### Template
Un modèle dont un ou plusieurs types sont des paramètres. Le compiler **instancie** une version concrète pour chaque type utilisé.

```cpp
template <typename T>
T max(T a, T b) { return a > b ? a : b; }

max(3, 7);          // T = int
max(3.0, 7.0);      // T = double — separate compiled function
```

### Typename
Deux rôles : 📚 [`lexique/TYPENAME.md`](TYPENAME.md)
1. Déclarer un paramètre de type de template (interchangeable avec `class`)
2. Désambiguïser un nom dépendant comme étant un type

```cpp
template <typename T>
void f() {
    typename T::value_type x;   // tell compiler this is a TYPE
}
```

### Function template
Un template produisant une fonction — comme `max` ci-dessus.

### Class template
Un template produisant une class — comme `std::vector<T>`.

```cpp
template <typename T>
class Box { T value_; public: T get() const { return value_; } };

Box<int>    bi;
Box<std::string> bs;
```

### Specialization
Une implémentation sur mesure d'un template pour un type spécifique.

```cpp
template <> class Box<bool> { /* bit-packed version */ };
```

### Monomorphization
Le processus généré par le compiler pour produire une fonction/class concrète par instanciation de template. C'est pourquoi les templates résident dans les headers.

---

## 🎭 8. Casts & RTTI

> Conversions de types, explicites et au runtime.
> 📚 Pour aller plus loin : [`CASTS.md`](CASTS.md)

### Cast
Une conversion explicite de type. Le C++ dispose de quatre casts typés (par opposition au brutal `(T)x` du C) :

| Cast | Purpose | Sample |
|---|---|---|
| `static_cast<T>(x)` | Conversions raisonnables vérifiées au compile time | `static_cast<int>(3.14)` |
| `const_cast<T>(x)` | Ajouter ou retirer `const`/`volatile` | `const_cast<char*>(s)` |
| `reinterpret_cast<T>(x)` | Réinterprétation au niveau binaire. Dernier recours. | `reinterpret_cast<int*>(buf)` |
| `dynamic_cast<T>(x)` | Down-cast sûr dans une hiérarchie polymorphique. Retourne null/lance une exception en cas d'échec. | `dynamic_cast<Dog*>(animal_ptr)` |

### RTTI
**Run-Time Type Information.** Permet d'interroger le type réel d'un objet au runtime — implémenté via la vtable. Le coût ne s'applique que si vous l'utilisez.

### Typeid
L'opérateur de RTTI. Renvoie un `std::type_info` décrivant le type réel. 📚 [`lexique/TYPEID.md`](TYPEID.md)

```cpp
Animal *a = new Dog;
std::cout << typeid(*a).name();   // "Dog" (mangled, but resolvable)
```

---

## 📦 9. Standard Template Library (STL)

> Conteneurs + algorithmes génériques préconçus.
> 📚 Pour aller plus loin : [`advanced/STL.md`](../notions/advanced/STL.md)

### STL
Une bibliothèque de **containers**, **iterators** et **algorithms** templatés — la boîte à outils que vous cessez de réinventer.

```
       ┌──────────┐    ┌─────────────┐    ┌──────────────┐
       │ Container│───►│  Iterator   │───►│  Algorithm   │
       │ vector   │    │ begin()/end │    │ find/sort/...│
       │ map list │    │             │    │              │
       └──────────┘    └─────────────┘    └──────────────┘
```

### Container
Un objet qui possède et organise une collection de valeurs. Les plus courants :

| Container | Big-O insert | Big-O find | Use when |
|---|---|---|---|
| `std::vector<T>` | O(1) fin | O(n) | Par défaut. Tableau optimisé pour le cache. |
| `std::list<T>` | O(1) n'importe où | O(n) | Beaucoup d'insertions au milieu de la liste. |
| `std::map<K,V>` | O(log n) | O(log n) | Clé→valeur triée. |
| `std::set<T>` | O(log n) | O(log n) | Clés uniques triées. |
| `std::deque<T>` | O(1) extrémités | O(n) | Insertion aux deux extrémités. |
| `std::stack<T>` | O(1) | — | LIFO. |
| `std::queue<T>` | O(1) | — | FIFO. |

### Iterator
Un objet semblable à un pointer qui parcourt un container. Le lien entre les containers et les algorithms.

```cpp
std::vector<int> v = {1, 2, 3};
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it;
```

### Allocator
La source de mémoire interchangeable pour les containers STL. Vous l'ignorez 99 % du temps ; le `std::allocator` par défaut convient très bien.

### String
`std::string` — un buffer de caractères géré sur la heap avec longueur, copie et concaténation automatiques. La solution pour remplacer les `char*` du C source de fuites. 📚 [`io-errors/STRING_FUNCTIONS.md`](../notions/io-errors/STRING_FUNCTIONS.md)

```cpp
std::string s = "hello";
s += " world";
std::cout << s.size();   // 11
```
---

## 🌊 10. I/O streams

> Entrées et sorties buffurisées et type-safe.
> 📚 Pour aller plus loin : [`io-errors/FSTREAM_GUIDE.md`](../notions/io-errors/FSTREAM_GUIDE.md) · [`io-errors/OPEN.md`](../notions/io-errors/OPEN.md)

### Stream
Une abstraction au-dessus d'une séquence d'octets. La vision de l'OS (fichier/pipe/terminal) enveloppée dans un buffer typé.

```
                ┌───────────────────────────────────┐
                │     std::cout    →   stdout       │
                │     std::cin     ←   stdin        │
                │     std::cerr    →   stderr (raw) │
                │     std::ifstream /  std::ofstream│
                │     std::stringstream             │
                └───────────────────────────────────┘
```

```cpp
int x;
std::cin >> x;                    // formatted input
std::cout << "got " << x << '\n'; // formatted output
```

### Flags d'état du stream
`good`, `eof`, `fail`, `bad`. Traités comme un bool : `if (cin >> x)` est l'idiome pour "la lecture a réussi".

---

## 🚨 11. Erreurs et exceptions

> Quand les choses tournent mal, échouez bruyamment, échouez en toute sécurité.
> 📚 Pour aller plus loin : [`io-errors/ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md) · [`lexique/TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md)

### Exception
Un **saut non local** + un objet de charge utile (payload) — utilisé pour signaler des erreurs que le code local ne peut pas gérer.

### Throw
Déclenche une exception. Le stack unwinding démarre immédiatement.

```cpp
throw std::runtime_error("file missing");
```

### Try et catch
Où les exceptions sont attrapées et gérées. Les blocs catch sont testés dans l'ordre — placez les types dérivés en premier.

```cpp
try {
    risky();
} catch (const std::out_of_range& e) {
    /* specific */
} catch (const std::exception& e) {
    /* generic fallback */
}
```

### Stack unwinding
Lorsqu'une exception est propagée, chaque objet local situé entre le throw et le catch voit son **destructor appelé** dans l'ordre inverse. Le RAII garantit le nettoyage, même en pleine erreur.

```
        throw
          │
   ┌──────▼─────┐
   │  ~Lock()   │   ← destructors fire on the way out
   │  ~File()   │
   │  ~Buffer() │
   └──────┬─────┘
          ▼
        catch
```

### Niveaux de sécurité des exceptions
| Niveau | Garantie |
|---|---|
| **No-throw** | L'opération ne lève jamais d'exception. |
| **Strong** | Soit elle réussit, soit elle effectue un rollback complet (aucun changement observable). |
| **Basic** | Pas de leaks, pas d'UB, mais l'état a pu changer. |
| **None** | À éviter. |

---

## 🧪 12. Idiomes à la sauce 42

> Patterns que vous retrouverez à travers CPP00–CPP04.
> 📚 Pour aller plus loin : [`modules/WALKTHROUGH.md`](../projets/modules/WALKTHROUGH.md)

| Idiome | Où | Ce que cela enseigne |
|---|---|---|
| **Megaphone** | [`modules/CPP00.md`](../projets/modules/CPP00.md) | Premier contact avec `std::string`, `std::cout` |
| **PhoneBook** | [`modules/CPP00.md`](../projets/modules/CPP00.md) | Class avec état private + formatage iostream |
| **OCF / Règle des 4** | [`modules/CPP01.md`](../projets/modules/CPP01.md) | Default ctor, copy ctor, op=, dtor |
| **Virgule fixe** | [`modules/CPP02.md`](../projets/modules/CPP02.md) | Operator overloading, conversions |
| **Chaîne ClapTrap** | [`modules/CPP03.md`](../projets/modules/CPP03.md) | Héritage, puis base virtuelle (Diamant) |
| **Hiérarchie Animal** | [`modules/CPP04.md`](../projets/modules/CPP04.md) | Polymorphisme, classes abstraites, deep copy |

### Les réflexes de la norme 42
- Tabulations uniquement. Les fichiers se terminent par `.hpp` / `.cpp`.
- Une class par paire de fichiers.
- Gardes `#ifndef` sur chaque header.
- Pas de `using namespace std;` dans les headers — qualificateur complet (`std::cout`).
- `-Wall -Wextra -Werror -std=c++98`.

---

## 🛠️ 13. Outillage

| Sujet | Fichier |
|---|---|
| Makefiles pour le C++ | [`tooling/MAKEFILE_CPP.md`](../notions/tooling/MAKEFILE_CPP.md) |
| Libs statiques vs partagées | [`tooling/LIBRARIES.md`](../notions/tooling/LIBRARIES.md) |
| `<cmath>` | [`fundamentals/CMATH.md`](../notions/fundamentals/CMATH.md) |

---

## 🧭 14. Aide-mémoire — quoi utiliser et quand

```
   ┌─────────────────────────────────────────────────────────────┐
   │ Need a container of unknown size?                           │
   │     → std::vector<T>                                        │
   │ Want type-safe input?                                       │
   │     → std::cin >> x;  if (cin.fail()) ...                   │
   │ Want automatic cleanup of a resource?                       │
   │     → wrap it in a class, free in destructor (RAII)         │
   │ Same operation on different types?                          │
   │     → template<typename T>                                  │
   │ Same call, different behavior per subtype?                  │
   │     → virtual + base pointer/reference                      │
   │ "Should this be a class or struct?"                         │
   │     → invariants? class. plain data? struct.                │
   │ "Should I use new?"                                         │
   │     → only if lifetime exceeds the current scope            │
   │ "Should I make this const?"                                 │
   │     → yes, until proven otherwise                           │
   └─────────────────────────────────────────────────────────────┘
```

---

## 🪜 15. Par où continuer

1. Vous débutez ? → [`fundamentals/BASICS.md`](../notions/fundamentals/BASICS.md)
2. Vous voulez le modèle mental sous-jacent ? → [`fundamentals/CONTRACTS.md`](../notions/fundamentals/CONTRACTS.md)
3. Vous luttez avec la mémoire ? → [`fundamentals/MEMORY.md`](../notions/fundamentals/MEMORY.md) + [`lexique/NEW.md`](NEW.md) / [`lexique/DELETE.md`](DELETE.md)
4. Vous écrivez votre première class ? → [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)
5. L'héritage ne clique pas ? → [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) → [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md)
6. Les templates semblent magiques ? → [`advanced/TEMPLATES.md`](../notions/advanced/TEMPLATES.md)
7. La STL est encore inconnue ? → [`advanced/STL.md`](../notions/advanced/STL.md)
8. Les exceptions explosent ? → [`io-errors/ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md)
9. Tous les mots-clés, un clic chacun → [`lexique/INDEX.md`](INDEX.md)

> **La règle empirique :** si un terme dans un sujet 42 vous déroute, cherchez-le sur cette page → cliquez → 5 minutes plus tard vous l'avez compris. S'il n'y est pas, c'est que la page est incomplète — ajoutez-le.

---

## 🧾 16. Notions clés fréquemment citées par les tuteurs

Définitions rapides pour le vocabulaire OOP de haut niveau qui gravite autour des discussions C++.

### Abstraction
Cacher le *comment* derrière un *quoi* limpide. Un `std::string` est abstrait — peu importe la façon dont les caractères sont stockés. Les mots-clés pour **construire** des abstractions : `class`, `private`, pure virtual, `template`.

### Composition
"**Contient un**" au lieu de "**est un**". Une `Car` *a* un `Engine` (composition) ; une `SportsCar` *est* une `Car` (héritage).

```cpp
class Engine { /* … */ };
class Car {
    Engine engine_;        // composition: Car has-an Engine
};
class SportsCar : public Car { … };  // inheritance: SportsCar is-a Car
```

> Règle empirique : **préférez la composition**. N'héritez que lorsque le comportement doit être substituable.

### Interface
Le *quoi* — les méthodes publiques, le contrat. En C++, généralement exprimée sous forme d'une classe abstraite avec des pure virtuals. L'**implémentation** correspondante est le *comment* — la classe concrète avec les corps de méthodes.

```cpp
class IShape {                          // interface
public:
    virtual double area() const = 0;
    virtual ~IShape() {}
};
class Circle : public IShape {          // implementation
    double r_;
public:
    double area() const { return 3.14 * r_ * r_; }
};
```

### API
"**Application Programming Interface**" — l'ensemble des types, fonctions et méthodes publiques que votre code ou bibliothèque expose aux tiers. En C++, "API" ≈ "la partie publique d'un header".

### Functeur
Aussi appelé **function object**. Une class dont l'`operator()` est overloadé — vous pouvez ainsi appeler les instances comme des fonctions. La façon C++98 de transmettre du comportement (les lambdas sont apparues en C++11).

```cpp
struct Greater {
    bool operator()(int a, int b) const { return a > b; }
};
std::sort(v.begin(), v.end(), Greater());   // pass behavior
```

```
   regular function:    int  cmp(int a, int b);     ← state-less
   functor:             Greater g; g(3, 5);         ← can carry state in members
```