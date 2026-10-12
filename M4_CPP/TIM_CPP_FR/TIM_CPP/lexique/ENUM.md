# `enum` — Un ensemble de constantes entières nommées

> **TL;DR.** `enum` définit un type dont les valeurs sont des constantes entières nommées. Les enums en C++98 sont des entiers sous le capot — ils se convertissent implicitement vers/depuis `int`, la largeur sous-jacente est définie par l'implémentation (implementation-defined), et les noms résident dans le scope englobant (pas à l'intérieur de l'enum). C++11 a ajouté `enum class` pour un typage plus strict.

En lien : [`STATIC.md`](STATIC.md) · [`CONST.md`](CONST.md)

---

## 1. La forme de base

```cpp
enum Color {
    RED,         // 0
    GREEN,       // 1
    BLUE         // 2
};

Color c = GREEN;
int   x = c;     // conversion implicite en int — c vaut maintenant 1
```

Chaque nom reçoit la valeur suivante, en commençant à 0. Vous pouvez surcharger cette valeur :

```cpp
enum HttpStatus {
    OK            = 200,
    NOT_FOUND     = 404,
    SERVER_ERROR  = 500
};

enum Mixed {
    A,           // 0
    B = 5,
    C,           // 6 — continue depuis B
    D = 5,       // légal — les doublons sont autorisés
    E            // 6
};
```

---

## 2. Le piège de la conversion implicite en int (C++98)

```cpp
enum State { IDLE, RUNNING, DONE };

State s = IDLE;
int   n = s;            // OK — conversion implicite State → int
s = 42;                 // ERREUR — int → State nécessite un cast
s = static_cast<State>(42);   // OK, mais State n'a aucun énumérateur avec la valeur 42
                              // → s contient une valeur hors limites, le comportement dépend
```

La conversion est implicite dans un seul sens (enum → int). Revenir en arrière nécessite un cast.

Cette conversion implicite en int est pratique (vous pouvez utiliser des valeurs d'enum là où des ints sont attendus) et dangereuse (mélanger des enums de types différents compile silencieusement) :

```cpp
enum Color { RED, GREEN };
enum Fruit { APPLE, BANANA };

Color c = RED;
Fruit f = APPLE;

if (c == f) { … }        // compile ! les deux sont convertis en int → les deux valent 0 → égaux
```

L'`enum class` de C++11 corrige cela ; en C++98, soyez prudent.

---

## 3. Fuite des énumérateurs dans le scope environnant

```cpp
enum Direction { NORTH, SOUTH, EAST, WEST };

Direction d = NORTH;     // on peut utiliser NORTH directement
```

`NORTH`, `SOUTH`, etc. sont visibles **dans le scope englobant de l'enum** — pas à l'intérieur de `Direction`. On n'écrit pas `Direction::NORTH`. Cela provoque une pollution de noms :

```cpp
enum Color    { RED, GREEN, BLUE };
enum Severity { GREEN, YELLOW, RED };  // ERREUR — RED et GREEN sont déjà déclarés
```

L'`enum class` de C++11 garde les noms scopés (`Color::RED`, `Severity::GREEN`).

En C++98, vous contournez cette fuite en :

- Plaçant l'enum à l'intérieur d'une class ou d'un namespace :

```cpp
class Color {
public:
    enum Value { RED, GREEN, BLUE };
};

Color::Value c = Color::RED;
```

---

## 4. Vue matérielle/stockage

Le compiler choisit un type entier sous-jacent « suffisamment grand pour contenir tous les énumérateurs ». Sur la plupart des compilers, il s'agit de `int` (4 octets), mais il peut être plus petit ou plus grand :

```cpp
enum Tiny  { A };                           // peut faire 1, 2, 4 octets — défini par l'implémentation
enum Big   { LARGE = 0x70000000 };          // nécessite au moins 32 bits
enum Huge  { HUGE  = 0x100000000LL };       // nécessite 64 bits (spécifique à l'implémentation)
```

```
   mémoire:
   ┌────┐                                       
   │  1 │  enum Color { RED }; sizeof = ? (souvent 4)
   └────┘                                       
```

En général, vous ne pouvez pas vous fier à `sizeof(enum) == sizeof(int)`, mais en pratique sur g++, c'est le cas.

En C++11, vous pouvez fixer le type sous-jacent :

```cpp
enum class Severity : char { LOW, MEDIUM, HIGH };   // 1 octet garanti
```

Pas en C++98.

---

## 5. Idiome 42 courant : enum comme constantes entières typesafe

```cpp
class Animal {
public:
    enum Type { CAT, DOG, BIRD, UNKNOWN };

    Animal();
    Animal(Type t);

    Type getType() const;
private:
    Type _type;
};

Animal a(Animal::CAT);
if (a.getType() == Animal::DOG) { … }
```

C'est bien meilleur que :

```cpp
class Animal {
public:
    static const int CAT = 0;
    static const int DOG = 1;
    static const int BIRD = 2;
    static const int UNKNOWN = 3;
};
```

— parce que l'enum est au moins *distinguable par son type* des ints arbitraires (même s'il se convertit implicitement).

---

## 6. Enum comme constante à la compilation

```cpp
enum { BUFFER_SIZE = 1024 };          // enum anonyme — simplement une constante

char buffer[BUFFER_SIZE];              // OK, BUFFER_SIZE est une expression constante
```

C'est une astuce courante en C++98 pour déclarer des constantes entières lorsque `static const int` dans une class s'avère délicat (cela fonctionne, mais avant le C++11, vous rencontrez parfois des problèmes si vous prenez son adresse). L'« enum hack » contourne ce problème.

```cpp
template <int N>
class Buffer {
    enum { CAPACITY = N * 2 };       // constante calculée à la compilation
    int data[CAPACITY];
};
```

Utile dans les templates où vous voulez une constante calculée à partir des paramètres de template.

---

## 7. Itérer sur un enum

Il n'y a pas de moyen intégré. Pattern courant :

```cpp
enum Day { MON, TUE, WED, THU, FRI, SAT, SUN, DAY_COUNT };

for (int d = MON; d < DAY_COUNT; ++d) {
    Day day = static_cast<Day>(d);
    // ...
}
```

Le `DAY_COUNT` terminal est une sentinelle — le décompte des valeurs réelles. Idiome courant pour « rendre cet enum itérable ».

---

## 8. Enum vs `#define` vs `static const`

```cpp
#define MAX 1024                  // remplacement textuel, non typé, pas de scope
const int MAX = 1024;             // typé, scopé, possède une adresse
static const int MAX = 1024;      // constante typée locale au fichier
enum { MAX = 1024 };              // constante à la compilation, pas d'adresse
```

Choisissez :
- `enum` pour un *ensemble* de constantes liées.
- `static const int` pour une constante entière unique dans une class (`class C { static const int N = 10; };`).
- `const int` à la portée d'un namespace pour une constante typée unique (avec le linkage approprié).
- `#define` uniquement pour les include guards et la compilation conditionnelle.

---

## 9. C++11 `enum class` — pour le contexte uniquement (pas pour 42 C++98)

```cpp
enum class Color : int { RED, GREEN, BLUE };

Color c = Color::RED;             // doit être qualifié
int   x = c;                       // ERREUR — pas de conversion implicite
int   y = static_cast<int>(c);     // OK
```

L'`enum class` est **fortement typé** et **scopé** :
- Les noms ne fuient pas — écrivez `Color::RED`, pas `RED`.
- Pas de conversion implicite en int.
- Type sous-jacent optionnel : `: int`, `: char`, etc.

Si vous quittez 42 pour une base de code C++ moderne, préférez `enum class` partout où il est disponible.

---

## 10. Trucs & astuces

### 10.1 Incluez toujours un UNKNOWN / NONE / DEFAULT

```cpp
enum LogLevel { DEBUG, INFO, WARN, ERR, NONE };
```

Une valeur no-op ou sentinelle est utile pour les objets construits par défaut.

### 10.2 Utilisez le pattern de la sentinelle de comptage

```cpp
enum Direction { NORTH, SOUTH, EAST, WEST, DIR_COUNT };
const char *names[DIR_COUNT] = { "north", "south", "east", "west" };
```

Ajouter une direction signifie modifier l'enum et ajouter un nom — la taille du tableau s'aligne automatiquement sur `DIR_COUNT`.

### 10.3 Attention aux collisions de noms

```cpp
enum Color { RED, GREEN };

void RED() { }       // ERREUR — RED est déjà déclaré
```

Les noms d'enum classiques vivent dans le scope englobant et peuvent masquer / être masqués.

### 10.4 Ne comparez pas des enums de types différents

```cpp
enum A { X };
enum B { Y };

if (X == Y) { }      // compile — les deux valent 0 — silencieusement vrai
```

Deux enums non liés évalués comme égaux parce que leurs ints sous-jacents correspondent est l'un des bugs silencieux les plus courants en C++98.

### 10.5 Switch sur un enum — activez les warnings

```cpp
enum State { IDLE, RUNNING, DONE };

void f(State s) {
    switch (s) {
        case IDLE:    handleIdle(); break;
        case RUNNING: handleRun();  break;
        // DONE a été oublié !
    }
}
```

Les compilers peuvent émettre un avertissement (`-Wswitch`) lorsqu'un switch sur un enum ne couvre pas chaque énumérateur. Activez-le toujours.

### 10.6 Convertir un enum en string pour le débogage

Il n'y a pas de moyen intégré en C++98. Pattern :

```cpp
const char *toString(LogLevel l) {
    switch (l) {
        case DEBUG: return "DEBUG";
        case INFO:  return "INFO";
        case WARN:  return "WARN";
        case ERR:   return "ERR";
        case NONE:  return "NONE";
    }
    return "??";
}
```

Fastidieux mais explicite. C++ ne possède pas de réflexion.

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `cannot convert int to enum X` | Affectation d'un int brut à un enum | `static_cast<X>(value)` |
| `redeclaration of 'RED'` | Deux enums dans le même scope utilisent le même nom | Encapsuler dans des classes/namespaces, ou renommer |
| Comparaison d'enums silencieusement égale | Types d'enum différents valant tous les deux 0 | Encapsuler dans des classes ; activer `-Wsign-compare`/`-Wenum-compare` |
| `enumerator value too large` | La valeur dépasse le type sous-jacent | Ajouter un cast, ou spécifier un type sous-jacent plus large (C++11) |

---

## 12. Résumé visuel

```
              ┌──────────────────────────────────────────────┐
              │   enum Color { RED, GREEN, BLUE };           │
              │                                              │
              │   constantes entières avec des noms.         │
              │   les noms vivent dans le scope ENGLOBANT.   │
              │   conversion implicite en int.               │
              │   type sous-jacent défini par l'implémentation│
              └────────────────────┬─────────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────────┐
              ▼                    ▼                         ▼
     ensemble de constantes   constante à la compilation  astuce de scoping
     ──────────────────────   ──────────────────────────  ─────────────────
     enum Day { MON, …};      enum { N = 10 };            class C {
     enum Status {…};         int arr[N];                     enum E { A,B };
                                                          };
                                                          C::E e = C::A;

     Alternative C++11 : enum class — fortement typé, scopé,
     pas de conversion implicite. À utiliser dès que vous en avez l'option.
```
---

## 13. Practice

1. Pourquoi `enum Color {RED, GREEN}; enum Severity {GREEN, YELLOW};` ne compile-t-il pas ? *(Les deux définissent `GREEN` dans le scope englobant — collision de noms.)*
2. Pourquoi `enum E { A };` suivi de `int x = A;` est-il légal ? *(Conversion implicite enum-to-int.)*
3. Quel est l'underlying type de `enum E { X };` ? *(Implementation-defined, mais typiquement `int` sur les compilers 32/64-bit.)*
4. Qu'est-ce que l'« enum hack » pour les constantes compile-time ? *(Un `enum { N = …};` anonyme — produit une constant expression utilisable pour les tailles d'array, etc.)*