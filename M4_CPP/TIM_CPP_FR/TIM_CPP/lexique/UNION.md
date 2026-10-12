# `union` — Un emplacement mémoire, plusieurs costumes

> **TL;DR.** Une `union` est comme une `struct`, sauf que **tous les members partagent les mêmes octets**. Sa taille est la taille de son plus grand member. Seul le member écrit en dernier contient une valeur valide — lire un autre member relève du type-punning, et en C++ c'est un undefined behavior (contrairement au C, qui le tolère).

Voir aussi : [`STRUCT.md`](STRUCT.md) · [`SIZEOF.md`](SIZEOF.md) · [`CASTS.md`](CASTS.md) (reinterpret_cast — l'autre outil de punning)

---

## 1. La forme

```cpp
union Value {
    int    i;
    float  f;
    char   c[4];
};              // sizeof(Value) == 4 — ils se chevauchent
```

```
   struct { int i; float f; }      union { int i; float f; }
   ┌────────┬────────┐             ┌────────┐
   │   i    │   f    │  8 octets   │  i / f │  4 octets
   └────────┴────────┘             └────────┘
      côte à côte                    mêmes octets, un à la fois
```

```cpp
Value v;
v.i = 42;          // v contient un int
v.f = 3.14f;       // maintenant elle contient un float — l'int n'existe plus
```

## 2. Pourquoi elle existe

- **Interopérabilité C / syscalls** — par ex. `epoll_data_t` dans l'API du kernel est une union.
- **Variants économes en mémoire** — un champ qui est *soit* un int *soit* un pointer, étiqueté manuellement :

```cpp
struct Token {           // le pattern "tagged union"
    enum Kind { NUMBER, WORD } kind;
    union {
        int   num;       // valide quand kind == NUMBER
        char *word;      // valide quand kind == WORD
    } as;
};
```

L'étiquette `enum` impose la discipline : vérifiez-la avant de lire. Le langage ne vérifie **pas** pour vous.

## 3. Restrictions en C++98

- Les members ne peuvent pas avoir de constructors, de destructors ou d'assignment operators — pas de `std::string` dans une union. (Types POD uniquement.)
- Ne peut pas être une base class, ne peut pas avoir de virtual functions.
- Lire un member autre que le dernier écrit : **UB** en C++. L'infâme astuce des bits de float —

```cpp
union { float f; int i; } u;
u.f = 1.0f;
std::cout << u.i;    // ⚠️ fonctionne partout en pratique, UB sur le papier
```

— est la façon dont les développeurs inspectent le pattern de bits d'un float, mais l'outil C++ homologué est `memcpy`... qui est de toute façon interdit à 42. Sachez que cela existe, ne l'utilisez pas en production.

## 4. En pratique à 42

Vous lirez des unions (structs du kernel, vieux code C) bien plus souvent que vous n'en écrirez. Si vous êtes tenté d'en écrire une pour économiser de la mémoire dans les modules CPP — ne le faites pas ; la clarté l'emporte à cette échelle. Elle existe dans votre boîte à outils pour l'**interopérabilité C** et pour comprendre ce que `std::variant` (C++17) a civilisé par la suite.