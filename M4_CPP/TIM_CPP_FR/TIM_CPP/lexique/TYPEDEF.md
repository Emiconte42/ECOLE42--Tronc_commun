# `typedef` — Donner un autre nom à un type existant

> **TL;DR.** `typedef` introduit un alias pour un type. L'alias et l'original sont **le même type** — le compiler les traite de manière identique, aucune surcharge (overload) ne les distingue, aucune conversion n'est nécessaire. C'est un outil de documentation et d'abréviation.

Voir aussi : [`USING.md`](USING.md) · [`STRUCT.md`](STRUCT.md) · [`TEMPLATE.md`](TEMPLATE.md)

---

## 1. La forme de base

```cpp
typedef <existing-type>  <new-name>;
```

Exemples :

```cpp
typedef unsigned char        byte;
typedef long long            i64;
typedef std::vector<int>     IntVec;
typedef std::map<std::string, int> StringIntMap;
typedef int (*FnPtr)(int);                    // alias de pointer de fonction
typedef void (Container::*MemberFn)();        // alias de pointer-to-member
```

Après cela :

```cpp
byte       b = 200;             // identique à unsigned char b = 200;
IntVec     v;                   // identique à std::vector<int> v;
FnPtr      f = &square;         // identique à int (*f)(int) = &square;
```

---

## 2. Où se place le nouveau nom — lisez-le comme une déclaration

L'astuce pour lire un `typedef` est de **le lire comme si vous déclariez une variable, puis de remplacer le nom de la variable par l'alias de type** :

```cpp
int (*f)(int);            // 'f' est un pointer vers une fonction prenant un int et retournant un int.
typedef int (*FnPtr)(int);// FnPtr est le TYPE : pointer vers ce genre de fonction.

FnPtr g = &square;        // g est un FnPtr (un function pointer)
```

```cpp
char buf[10];                  // 'buf' est un tableau de 10 chars
typedef char Buffer[10];       // 'Buffer' est le TYPE : tableau de 10 chars

Buffer x;                      // déclare 'x' comme un char[10]
```

```cpp
int (Container::*p)();              // p est un pointer-to-member-function
typedef int (Container::*MemFn)();  // MemFn est ce type
```

Cela fonctionne pour *n'importe quelle* déclaration. `typedef` transforme littéralement le nom de la variable en un alias de type.

---

## 3. Le point de vue du compiler — un alias pur

Un `typedef` n'est **pas un nouveau type**. C'est un synonyme. Le compiler traite l'alias et l'original de façon interchangeable :

```cpp
typedef int Distance;
typedef int Mass;

void f(Distance d);
void f(Mass m);             // ERREUR — même signature : void f(int)
```

Les deux noms signifient `int`. Vous ne pouvez pas faire d'overload dessus. Ils sont identiques du point de vue du système de types.

C'est parfois gênant (on aimerait pouvoir distinguer `Distance` et `Mass`). Solutions : une struct d'encapsulation (wrapper), ou — en C++11+ — un `enum class`. En C++98, c'est une vraie limitation.

---

## 4. Pourquoi utiliser `typedef` ?

### 4.1 Raccourcir les types template verbeux

```cpp
typedef std::map<std::string, std::vector<int> > NamedSeries;

NamedSeries data;                                // et non std::map<std::string, std::vector<int> > data;
NamedSeries::iterator it = data.begin();         // également beaucoup plus court
```

### 4.2 Documenter l'intention

```cpp
typedef int FileDescriptor;
typedef int Bytes;
typedef int Seconds;

FileDescriptor fd = open(...);
Bytes          n  = read(fd, buf, BUF_SIZE);
Seconds        t  = time(0);
```

Le compiler les traite toujours tous comme des `int`, mais le lecteur perçoit la signification.

### 4.3 Masquer les détails d'implémentation

```cpp
// MyContainer.hpp
class MyContainer {
public:
    typedef std::map<std::string, int>::iterator iterator;
    typedef std::size_t                           size_type;

    iterator begin();
    size_type size() const;
};
```

Si vous modifiez le container sous-jacent ultérieurement, seul le typedef est mis à jour ; le code client ne casse pas.

### 4.4 Types de function pointer

```cpp
typedef int (*ComparatorFn)(const void*, const void*);

void mySort(void *base, std::size_t n, std::size_t sz, ComparatorFn cmp);
```

La syntaxe des function pointers est notoirement lourde ; un alias rend les signatures lisibles.

### 4.5 Types d'itérateurs dans les templates

```cpp
template <typename T>
class Stack {
public:
    typedef T*       iterator;
    typedef const T* const_iterator;
    typedef std::size_t size_type;
    typedef T        value_type;
};
```

C'est le pattern standard de la STL. Le code peut ensuite écrire `Stack<int>::value_type` et rester générique.

---

## 5. `typedef` avec des structs/classes — astuce issue du C

En C, `struct s_node` requiert le mot-clé `struct`. La convention était donc de combiner avec `typedef` :

```c
typedef struct s_node {
    int data;
    struct s_node *next;
} t_node;

t_node n;             // plus court que 'struct s_node n;'
```

En C++, le nom de la class/struct *est* le type — aucun préfixe `struct` n'est requis :

```cpp
struct Node {
    int data;
    Node *next;
};

Node n;               // fonctionne directement
```

Ainsi, **vous n'avez pas besoin de `typedef struct` en C++**. Utilisez simplement `struct Name { … };`.

---

## 6. L'alternative C++11 : `using` (absente du C++98 de 42)

Le C++11 a ajouté une syntaxe d'alias plus claire :

```cpp
using FnPtr = int (*)(int);                       // C++11 — identique à typedef
using IntVec = std::vector<int>;
```

Elle se lit de gauche à droite : « FnPtr est un alias pour `int(*)(int)` ». Elle gère mieux les templates (alias templates, voir ci-dessous).

À 42 en C++98, tenez-vous-en à `typedef`.

---

## 7. Alias templates (C++11+, pas à 42)

```cpp
template <typename T>
using Vec = std::vector<T>;

Vec<int>    v;        // identique à std::vector<int>
```

Un `typedef` ne peut pas être paramétré de cette façon en C++98. Il faudrait un wrapper de type struct :

```cpp
template <typename T>
struct Vec {
    typedef std::vector<T> type;
};

Vec<int>::type v;     // verbeux mais fonctionne en C++98
```

---

## 8. Point de vue matériel et codegen

`typedef` n'a aucun impact à l'exécution. Il n'y a pas de type distinct, pas de conversion, pas de métadonnées. Le compiler résout l'alias avant toute codegen :

```
   typedef int  Distance;
   Distance d = 5;     →   int d = 5;     // même code que si vous aviez écrit int directement
```

Même alignement, même layout, même convention d'appel.

---

## 9. Astuces et bonnes pratiques

### 9.1 Utiliser `typedef` au sein des classes pour des interfaces de style STL

```cpp
template <typename T>
class List {
public:
    typedef T              value_type;
    typedef T&             reference;
    typedef const T&       const_reference;
    typedef T*             pointer;
    typedef const T*       const_pointer;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;
};
```

Cela permet aux algorithmes génériques d'interroger le container (« quel est ton type de valeur ? ») via `Container::value_type`.

### 9.2 Ne pas renommer les types intégrés (built-in) inutilement

```cpp
typedef int   Integer;          // aucune valeur ajoutée
typedef char  Character;        // aucune valeur ajoutée
```

Ne renommez les types built-in que lorsqu'ils apportent une information **sémantique** (`FileDescriptor`, `Bytes`).

### 9.3 Déclarer les function pointers avec des typedefs

```cpp
// pénible :
void register_callback(void (*fn)(int, const char *), void *user);

// lisible :
typedef void (*Callback)(int, const char *);
void register_callback(Callback fn, void *user);
```

### 9.4 Nom de typedef vs nom de class — les deux sont des types

```cpp
struct S { int x; };
typedef S Synonym;

S        a;
Synonym  b;             // type identique — ces variables sont de la même nature
```

Vous pouvez passer `a` à une fonction attendant un `Synonym` et inversement.

### 9.5 L'interaction avec le « specifier de type élaboré » en C++

```cpp
typedef int X;
class X { ... };          // déclare une *class* X, distincte du typedef

X x;                       // ambiguïté ? — cherche d'abord la class
typedef X y;               // recherche du nom via typedef
```

Ne masquez pas (shadow) des typedefs avec des noms de class. C'est valide selon la norme, mais déroutant.

---

## 10. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `redefinition of 'X'` lors de deux typedefs du même nom | Deux typedefs dans le même scope | Supprimez le doublon ou assurez-vous qu'ils se trouvent dans des scopes différents |
| Erreurs de syntaxe sur un typedef de pointer de fonction | Parenthèses mal placées | `typedef Ret (*Name)(Args);` — les parenthèses autour de `*Name` sont obligatoires |
| `'X' has not been declared` | Utilisation d'un typedef avant sa déclaration | Déplacez le `typedef` au-dessus de sa première utilisation |
| Conflit d'overload entre deux typedefs | Alias du même type sous-jacent | `typedef` n'introduit pas de nouveaux types — utilisez une struct wrapper |

---

## 11. Résumé visuel

```
              ┌────────────────────────────────────────────┐
              │   typedef <type> <alias>;                   │
              │                                              │
              │   l'alias et <type> sont le MÊME TYPE.       │
              │   aucun nouveau type. pas d'overload.        │
              │   résolu par le compiler avant la codegen.  │
              └────────────────────┬───────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────┐
              ▼                    ▼                     ▼
         lisibilité            masquage              alias de fonction /
        ─────────────         d'implémentation      pointer
        types template        ────────────────      ───────────────────
        longs                 le client utilise     int (*F)(int);
        intention             le nom typedefé       typedef int (*F)(int);
        documentation         le type interne peut  ──────────────────────
        d'unités              changer sans casser
                              le client.

       se lit comme une déclaration : remplacez le nom de la variable
       par le nom de l'alias, et cela forme le type.
       
       en C++, les noms de struct/class sont déjà des types — nul besoin
       d'écrire 'typedef struct s_X {…} t_X;'.
```
---

## 12. Pratique

1. Pourquoi `typedef int Distance;` et `typedef int Mass;` ne sont-ils pas distinguables pour l'overloading ? *(`typedef` crée un alias d'un type existant ; les deux sont toujours `int`. L'overloading se base sur l'identité du type.)*
2. Comment définiriez-vous un alias pour un type function pointer ? *(`typedef Ret (*Name)(Args);` — notez les parenthèses autour de `*Name`.)*
3. Pourquoi `typedef struct s_node { … } t_node;` est-il inutile en C++ ? *(En C++, `struct Name` déclare déjà `Name` comme un type ; le `typedef` n'apporte rien.)*
4. Pourquoi la syntaxe `using` du C++11 (hors 42) semble-t-elle plus propre ? *(Elle se lit de gauche à droite et supporte les alias templates : `template <typename T> using Vec = std::vector<T>;`.)*