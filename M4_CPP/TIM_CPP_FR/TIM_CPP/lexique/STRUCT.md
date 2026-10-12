# `struct` — Une `class` qui se penche par la fenêtre

> **TL;DR.** En C++, `struct` et `class` créent le même genre de type. Les **seules** différences sont : les membres sont `public` par défaut dans une `struct`, `private` dans une `class` ; l'héritage est `public` par défaut pour une `struct`, `private` pour une `class`. Choisissez `struct` pour de la simple donnée brute (plain-old-data) ; choisissez `class` lorsque vous voulez de l'encapsulation.

Voir aussi : [`CLASS.md`](CLASS.md) · [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) · [`INHERITANCE.md`](../notions/oop/INHERITANCE.md)

---

## 1. Le résumé en deux lignes

```cpp
class A {
    int x;            // private (par défaut pour class)
};

struct B {
    int x;            // public (par défaut pour struct)
};
```

C'est la seule différence au niveau du langage. Les deux peuvent avoir des constructeurs, destructeurs, fonctions virtuelles, fonctions membres, spécificateurs d'accès, de l'héritage — absolument tout.

```cpp
struct Counter {
    int  count;
    Counter() : count(0) {}
    void inc() { ++count; }
    virtual ~Counter() {}     // oui, les destructeurs virtuels fonctionnent aussi dans une struct
};
```

---

## 2. La convention (quand utiliser quoi)

Par **convention** en C++ moderne :

| Utiliser `struct` quand… | Utiliser `class` quand… |
|---|---|
| Toutes les données sont publiques | Les données sont privées ; accès via des méthodes |
| Ce sont de pures données (style POD) | Il y a des invariants à maintenir |
| C'est un simple agrégat (`Point{x,y}`) | Il y a du comportement, pas seulement des données |
| C'est un type tag / des type traits | Il y a un copy ctor / OCF |

Cette convention est imposée par les lecteurs du code, non par le compiler. Le compiler les traite de manière identique (au modulo de la règle d'accès par défaut).

```cpp
struct Point { int x, y; };                          // données simples

class  PhoneBook {                                   // a des invariants
    Contact _contacts[8];
    int     _count;
public:
    void add(const Contact&);
    Contact get(int idx) const;
};
```

---

## 3. L'héritage par défaut

L'autre endroit où le mot-clé a une importance :

```cpp
class  Derived1 : Base { … };       // héritage private (par défaut pour class)
struct Derived2 : Base { … };       // héritage public (par défaut pour struct)

class  Derived3 : public Base { … };       // explicite — préférable pour la clarté
struct Derived4 : public Base { … };       // explicite — explicite même si c'est redondant
```

**Écrivez toujours le spécificateur d'accès explicitement** lorsque vous héritez. Ne vous fiez pas à la valeur par défaut du mot-clé — cela vous rattrapera dès l'instant où vous changerez `class` en `struct` ou inversement.

Voir [`INHERITANCE.md`](../notions/oop/INHERITANCE.md).

---

## 4. Compatibilité C — le superpouvoir de `struct`

Une `struct` ne contenant que des membres **triviaux** (pas de constructeurs, pas de méthodes virtuelles, pas de spécificateurs d'accès aux membres, pas d'héritage) est un **standard-layout type** — elle a le même agencement mémoire qu'une struct C équivalente. Vous pouvez :

- La passer à une bibliothèque C par pointer.
- La copier librement avec `memcpy`/`memmove`.
- La lire/l'écrire depuis un fichier binaire.

```cpp
// En C
struct sockaddr_in { ... };

// En C++
extern "C" {
    #include <netinet/in.h>
}

sockaddr_in addr;        // agencement identique
bind(fd, (sockaddr*)&addr, sizeof(addr));
```

Dès que vous ajoutez un constructeur, une fonction virtuelle ou un membre non par défaut, vous risquez de briser la compatibilité C — le compiler est libre de réordonner les éléments ou d'insérer un pointer de vtable.

---

## 5. POD (Plain Old Data) — la frontière de la compatibilité C

En C++98, un type **POD** est un type qui :

- N'a aucun constructeur, destructeur, ou opérateur d'affectation par copie déclaré par l'utilisateur.
- N'a aucune fonction virtuelle.
- N'a aucune classe de base virtuelle.
- Ne possède que des membres non statiques qui sont des POD.
- N'a aucun membre non statique private/protected.
- N'a aucune reference comme membre.

```cpp
struct PodPoint {
    int x;
    int y;
};                        // POD — sûr pour memcpy, fread/fwrite, etc.

struct NonPodPoint {
    int x, y;
    NonPodPoint() : x(0), y(0) {}    // constructeur utilisateur → plus un POD
};
```

Pour les POD, tout ceci est garanti :
- `memcpy` entre deux d'entre eux fonctionne.
- Les valeurs construites par défaut ne sont *pas initialisées* (pas de zero-init implicite).
- L'agencement en mémoire correspond à celui d'une struct C avec les mêmes membres.

Les non-POD peuvent toujours être copiés en toute sécurité — mais uniquement via le copy constructor, pas avec `memcpy`.

En C++11+, cette notion a été affinée en « trivially copyable » et « standard-layout » — mais pour le C++98/42, c'est le terme POD que vous rencontrerez.

---

## 6. Agencement mémoire — identique à `class`

Une `struct` dispose ses membres dans l'ordre de leur déclaration avec un padding d'alignement, exactement comme une [`class`](CLASS.md) :

```cpp
struct Pixel {
    unsigned char r;     // 1
    unsigned char g;     // 1
    unsigned char b;     // 1
    unsigned char a;     // 1
};                        // sizeof = 4 — parfaitement compacté
```

```
   adresse →   0    1    2    3
              ┌────┬────┬────┬────┐
              │ r  │ g  │ b  │ a  │
              └────┴────┴────┴────┘
   aucun padding car tous les membres sont alignés sur 1 octet
```

Comparez avec une struct aux alignements mixtes :

```cpp
struct Mixed {
    char  flag;          // 1
    int   value;         // 4 (doit s'aligner sur 4)
};                        // sizeof = 8 (1 + 3 pad + 4)
```

```
   address →   0    1    2    3    4    5    6    7
              ┌────┬────┬────┬────┬────┬────┬────┬────┐
              │flag│ pd │ pd │ pd │  value (32-bit)   │
              └────┴────┴────┴────┴───────────────────┘
```

Réorganisez pour la taille :

```cpp
struct Mixed2 {
    int   value;         // 4
    char  flag;          // 1
};                        // sizeof = 8 (4 + 1 + 3 de padding final — même total ici)
```

Le padding final arrondit à l'alignement du membre le plus strict, de sorte que `sizeof` soit un multiple de cet alignement. Cela permet aux tableaux de cette struct de fonctionner sans padding par élément.

### Inspecter le padding

```cpp
#include <cstddef>      // pour offsetof
struct S { char a; int b; };
std::cout << offsetof(S, a);      // 0
std::cout << offsetof(S, b);      // 4
std::cout << sizeof(S);           // 8
```

### `#pragma pack` — briser les règles d'alignement

Pour le parsing de protocoles réseau ou binaires, vous avez parfois besoin d'une struct sans padding :

```cpp
#pragma pack(push, 1)
struct PacketHeader {
    char     version;
    uint32_t length;     // non aligné !
};
#pragma pack(pop)
```

Désormais, la struct fait 5 octets (sans padding). Lire `length` peut être lent sur le matériel qui ne gère pas les accès non alignés (certains cœurs ARM), et peut être un UB sur certaines plateformes. Ne faites pas cela sans une véritable contrainte de protocole.

---

## 7. Agrégats et initialisation par accolades

Une `struct` (ou `class`) sans constructeur déclaré par l'utilisateur, sans membres non statiques private/protected, et sans classes de base est un **agrégat** — et peut être initialisée avec des accolades :

```cpp
struct Point { int x; int y; };
Point p = {3, 4};                  // OK — initialisation d'agrégat
Point q = {3};                     // OK — y initialisé par défaut à 0
```

Dès que vous ajoutez un constructeur défini par l'utilisateur, cela ne fonctionne plus :

```cpp
struct PointC { int x, y; PointC() : x(0), y(0) {} };
PointC p = {3, 4};                 // ERROR en C++98
PointC p(3, 4);                    // nécessite un ctor correspondant
```

L'initialisation d'agrégat est un pattern utile pour les structs de configuration et les types contenant uniquement des données.

---

## 8. Guide de style `struct` vs `class`

Utilisez `struct` pour :
- Les simples conteneurs de données (`Point`, `Color`, `Date`).
- Les couches de compatibilité C.
- Les types tags (`std::random_access_iterator_tag`).
- Les types servant uniquement d'objets-fonctions (`struct Less { bool operator()(int a, int b) const { return a<b; } };`).

Utilisez `class` pour :
- Tout ce qui possède des invariants.
- Tout ce qui a une OCF non triviale.
- La plupart des « vraies » classes des modules 42 (Contact, PhoneBook, Animal, etc.).

Le compiler s'en moque ; les **lecteurs** interprètent ce choix comme un indice.

---

## 9. Le `typedef struct` à la manière du C est inutile

C :

```c
typedef struct s_node {
    int   data;
    struct s_node *next;
} t_node;
```

C++ :

```cpp
struct Node {
    int   data;
    Node *next;       // 'Node' est déjà un type — pas de 'struct Node'
};
```

Vous n'avez pas besoin de `typedef` ni du tag final. Juste `struct Name { … };`.

---

## 10. Astuces et conseils

### 10.1 Déclaration anticipée (forward declare) avec `struct` ou `class` — le linker s'en moque

```cpp
struct Foo;             // forward
class  Foo;             // aussi forward, fait référence au même type
```

Mais choisissez-en un et soyez cohérent dans votre codebase. Certains compilers émettent un avertissement en cas de non-concordance.

### 10.2 Les types triviaux ont un coût nul (zero-cost)

```cpp
struct Vec2 { float x, y; };
Vec2 a, b;
Vec2 c = a;             // simple copie de 8 octets — même vitesse qu'une copie de struct en C
```

L'overhead C++ dont on entend parfois parler (vtables, copy ctors, etc.) n'intervient que si vous activez ces fonctionnalités.

### 10.3 Renvoyer de petites structs ne pose aucun problème

```cpp
struct Point { int x, y; };
Point makePoint() { Point p = {1, 2}; return p; }
```

Les compilers modernes effectuent l'**optimisation de la valeur de retour (RVO)** — la destination de l'appelant est transmise sous forme de pointer caché, la fonction la remplit, et aucune copie n'a lieu.

### 10.4 Structs anonymes (à proscrire selon le standard C++98)

```cpp
struct {
    int x, y;
} g;             // fonctionne en C ; techniquement non standard en C++98 mais les compilers l'acceptent
```

Vous ne verrez pas cela dans la norme 42. Évitez.

### 10.5 Les champs de bits (bit-fields) fonctionnent dans `struct` et `class`

```cpp
struct Flags {
    unsigned int red    : 1;
    unsigned int green  : 1;
    unsigned int blue   : 1;
    unsigned int alpha  : 5;
};                            // 1 octet au total (8 bits)
```
Le layout des bit-fields est implementation-defined — n'envoyez jamais une struct avec bit-field sur le réseau sans tester.

---

## 11. Erreurs courantes

| Erreur | Cause | Correction |
|---|---|---|
| `class S has no member named 'x'` après passage de `struct`→`class` | Les members sont devenus private | Ajouter `public:`, ou écrire `class S { public: int x; };` |
| `cannot access private member ...` après passage de la base de `struct` à `class` | L'inheritance par défaut a basculé en private | Écrire `: public Base` explicitement |
| `member 'x' is uninitialized` | Une struct POD avec `S s;` n'est pas initialisée | Brace-init : `S s = {};` |
| `cannot initialize aggregate of type 'S' with brace list` | La class possède un ctor déclaré par l'utilisateur → n'est plus un aggregate | Supprimer le ctor, ou utiliser les arguments du ctor |

---

## 12. Résumé visuel

```
                ┌──────────────────────────────────────┐
                │   keywords: struct / class           │
                │                                      │
                │   identical except for:              │
                │     • default member access          │
                │     • default inheritance access     │
                └──────────────────┬───────────────────┘
                                   │
              ┌────────────────────┼────────────────────┐
              ▼                    ▼                    ▼
        struct conventions     class conventions    practical rule
        ─────────────────      ─────────────────    ──────────────
        plain data             encapsulated         when in doubt,
        public members         private state        write the access
        no invariants          OCF / behavior       specifier
        C-compatible           "real" object        explicitly so
                                                    the keyword choice
                                                    becomes pure style.
```

---

## 13. Pratique

1. Quelle est la seule différence au niveau du langage entre `class` et `struct` ? *(L'access par défaut des members et des bases — `private` pour `class`, `public` pour `struct`.)*
2. Une `struct` peut-elle avoir un constructor et un destructor ? *(Oui — les deux peuvent avoir tout ce qu'une `class` peut avoir.)*
3. Pourquoi `Point p = {3, 4};` fonctionne-t-il pour une struct sans ctor mais échoue lorsque vous en ajoutez un ? *(L'aggregate initialization exige aucun ctor déclaré par l'utilisateur.)*
4. Pourquoi `sizeof(struct{char a;int b;})` donne-t-il 8 et non 5 ? *(Du padding pour l'alignment du `int` plus du tail padding pour que les tableaux fonctionnent.)*