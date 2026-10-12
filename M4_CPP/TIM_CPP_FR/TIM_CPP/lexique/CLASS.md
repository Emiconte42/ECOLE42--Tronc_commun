# `class` — Une `struct` qui masque son contenu par défaut

> **TL;DR.** `class` et `struct` sont *presque* identiques en C++. Les seules différences : l'accès par défaut (`private` vs `public`) et l'héritage par défaut (`private` vs `public`). Tout le reste — membres, méthodes, virtuels, layout, sizeof — est identique.

En lien : [`STRUCT.md`](STRUCT.md) · [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) · [`THIS.md`](THIS.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#7.%20Classes%20%E2%80%94%20the%20core%20of%20C%2B%2B) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. Le saut du C au C++

En C, une `struct` rassemble des données ; les fonctions sont séparées.

```c
typedef struct s_point { int x, y; } t_point;
void   point_translate(t_point *p, int dx, int dy);
```

En C++, une `class` rassemble les données **et** les opérations qui agissent dessus au sein d'un type unique :

```cpp
class Point {
    int _x, _y;
public:
    Point(int x, int y) : _x(x), _y(y) {}
    void translate(int dx, int dy) { _x += dx; _y += dy; }
    int  x() const { return _x; }
    int  y() const { return _y; }
};
```

Le point est désormais **responsable de lui-même**. Il transporte ses opérations avec lui ; le contrôle d'accès empêche les intervenants extérieurs de modifier `_x` directement.

---

## 2. L'anatomie d'une classe

```cpp
class MyClass {
private:
    int _data;                                          // variable membre

public:
    MyClass();                                          // ctor par défaut
    MyClass(int d);                                     // ctor de conversion
    MyClass(const MyClass& other);                      // copy ctor
    MyClass& operator=(const MyClass& other);           // assignation par copie
    ~MyClass();                                         // destructeur

    int  getData() const;                               // fonction membre const (getter)
    void setData(int d);                                // fonction membre non-const (setter)

    static int instances();                             // fonction membre static

private:
    static int _instanceCount;                          // variable membre static
};
```

Le compiler C++ va **synthétiser** automatiquement chacun des quatre membres de la Forme Canonique Orthodoxe (OCF) que vous ne déclarez pas (ctor par défaut, copy ctor, assignation par copie, dtor) — mais uniquement s'il en est capable. Voir [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

---

## 3. Memory layout — ce que produit réellement la compilation d'une `class`

Dans le cas le plus simple, une classe a le **même layout qu'une struct C**. Le compiler dispose les membres **dans l'ordre de leur déclaration**, en insérant potentiellement du padding pour l'alignement.

```cpp
class A {
    char  _c;       // 1 octet
    int   _i;       // 4 octets, doit être aligné sur 4 → 3 octets de padding avant
    char  _d;       // 1 octet
};                   // total : 12 octets (1 + 3 pad + 4 + 1 + 3 tail pad)
```

```
   address →    0    1    2    3    4    5    6    7    8    9   10   11
              ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
              │_c  │ pd │ pd │ pd │_i.0│_i.1│_i.2│_i.3│_d  │ pd │ pd │ pd │
              └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
              ↑                 ↑                       ↑
              │                 │                       └ tail padding pour que la
              │                 └ aligné sur 4            taille soit un multiple de
              └ début de l'objet                          l'alignement le plus strict (4)
```

Réorganisation pour plus de compacité :

```cpp
class B {
    int  _i;       // 4
    char _c;       // 1
    char _d;       // 1
};                  // total : 8 (4 + 1 + 1 + 2 tail pad)
```

Le `sizeof(class)` correspond à la somme finale du layout — prévisible et vérifiable :

```cpp
std::cout << sizeof(A) << " " << sizeof(B);   // 12 8
```

### Qu'en est-il des fonctions membres ?

Les fonctions membres ne sont **pas** stockées dans l'objet. Ce sont de simples fonctions dans la section `.text` qui reçoivent un premier paramètre caché `this` :

```cpp
class Point {
    int _x, _y;
public:
    void translate(int dx, int dy);
};

// le compiler émet concrètement :
// void Point__translate(Point *this, int dx, int dy);
```

Par conséquent :

```
   sizeof(Point) == sizeof(int) * 2 == 8

   memory:                            code (.text):
   ┌─────────────────┐                Point::translate(this, dx, dy)
   │ _x   │   _y     │                Point::x(this)
   └─────────────────┘                Point::y(this)
   un seul objet Point,               une seule copie dans le binaire
   8 octets au total                  partagée par toutes les instances de Point
```

C'est en partie pourquoi le C++ peut revendiquer des « abstractions à coût nul » (*zero-cost abstractions*) — les méthodes n'ajoutent aucun octet par instance (sauf si elles sont [`virtual`](VIRTUAL.md) — voir ce fichier).

---

## 4. Le pointeur caché `this`

À l'intérieur de toute fonction membre non-static, `this` est un pointer vers l'objet sur lequel la fonction a été appelée :

```cpp
void Point::translate(int dx, int dy) {
    this->_x += dx;          // explicite
    _y += dy;                // implicite — équivalent à this->_y
}
```

Voir [`THIS.md`](THIS.md) pour une explication complète.

---

## 5. Contrôle d'accès : `private`, `protected`, `public`

```cpp
class Point {
private:    int _x, _y;        // accessible uniquement à l'intérieur des fonctions membres de Point
protected:                     // accessible dans Point + classes dérivées
public:                        // accessible partout
    Point(int x, int y);
};
```

`class` utilise `private` par défaut. `struct` utilise `public` par défaut. C'est la **seule** réelle différence entre elles. Voir [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md).

Le contrôle d'accès est appliqué **à la compilation** — il n'y a aucune vérification à l'exécution, et la mémoire de `_x` est tout aussi modifiable que celle de `x`. Un appelant déterminé muni d'un cast de pointer peut y accéder ; le contrôle d'accès sert à garantir la conception de manière bienveillante, pas à assurer la sécurité.

---

## 6. Constructeur / destructeur — les hooks du cycle de vie

Chaque classe possède implicitement un cycle de vie :

```
   Point p(3, 4);   ─────►  le constructeur s'exécute
                                _x ← 3
                                _y ← 4
   p.translate(1, 1);
   p.translate(2, 2);
                            ... utilisation ...
   }                ─────►  le destructeur s'exécute quand p sort de la portée
```

```cpp
Point::Point(int x, int y) : _x(x), _y(y) {       // liste d'initialisation
    // corps — s'exécute après l'initialisation des membres
}

Point::~Point() {
    // nettoyage — fermer des fichiers, libérer de la mémoire, etc.
    // pour des membres triviaux comme int, il n'y a rien à faire.
}
```

Pour le RAII, voir [`MEMORY.md`](../notions/fundamentals/MEMORY.md). Pour l'OCF, voir [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

---

## 7. Déclaration anticipée (forward declaration)

Vous pouvez indiquer au compiler « cette classe existe, les détails viendront plus tard » sans inclure sa définition complète :

```cpp
class Point;             // déclaration anticipée (forward declaration)

void f(Point *p);        // OK — les pointers/refs ont seulement besoin d'une forward decl
void g(Point  p);        // ERREUR — le type complet est requis pour les paramètres par valeur
```

C'est une excellente pratique pour briser les inclusions circulaires et réduire le temps de compilation. Utilisez-la dès qu'un header a uniquement besoin de `T*` ou `T&`.

---

## 8. Types imbriqués

Une classe peut contenir une autre classe, un enum, un typedef :

```cpp
class List {
public:
    class Iterator {              // classe imbriquée
        // ...
    };

    typedef int size_type;        // typedef imbriqué
    enum { MAX = 1024 };          // enum imbriqué
};

List::Iterator it;                // à l'extérieur, qualifier avec List::
```

Les types imbriqués partagent le contrôle d'accès de l'endroit où ils sont déclarés (public ici).

---

## 9. Déclarer de manière anticipée avec la mauvaise clé (`class` vs `struct`)

L'une ou l'autre de ces syntaxes fonctionne :

```cpp
struct Foo;
class  Foo;                     // identique au niveau du système de types
```

Cependant, choisissez-en une et tenez-vous-y par souci de clarté (les linkers s'en moquent ; les lecteurs, non).

---

## 10. La taille d'une classe vide

```cpp
class Empty {};
std::cout << sizeof(Empty);     // 1 — PAS zéro
```

Pourquoi ? Un objet doit posséder une adresse unique. Si `sizeof(Empty)` valait zéro, deux objets `Empty` pourraient partager la même adresse — ce qui perturberait des mécanismes comme l'indexation de tableaux. Le standard impose *au moins* 1 octet.

```
   Empty array[3];
   address: 0x100   0x101   0x102
            ┌─────┬─────┬─────┐
            │  ?  │  ?  │  ?  │   ← 1 octet chacun, aucun chevauchement
            └─────┴─────┴─────┘
```

Lorsqu'une classe vide sert de **classe de base** à une classe dérivée non vide, le compiler peut appliquer l'**empty base optimization** — aucun octet supplémentaire n'est alors ajouté pour la base.

---

## 11. Trucs & astuces

### 11.1 La convention de l'underscore à 42

La norme de 42 préfixe les membres privés par un `_` :

```cpp
class C {
private:
    int _value;             // pas 'value', pas 'm_value', pas 'value_'
};
```

L'intérêt : en lisant du code tel que `_value = x;`, vous savez immédiatement que vous manipulez un membre privé.

### 11.2 Organisation d'un header

Organisation standard selon 42 :

```cpp
#ifndef MYCLASS_HPP
# define MYCLASS_HPP

# include <string>           // inclure ce que vous utilisez

class MyClass {
private:
    std::string _name;
    int         _value;

public:
    MyClass();
    MyClass(const std::string& name, int value);
    MyClass(const MyClass& other);
    MyClass& operator=(const MyClass& other);
    ~MyClass();

    const std::string& getName() const;
    int                getValue() const;
    void               setName(const std::string& name);
    void               setValue(int value);
};

#endif
```
### 11.3 Ne mélangez pas déclaration et définition pour les fonctions non triviales

```cpp
// bien
class C {
public:
    void heavy_function() const;     // déclaration uniquement
};
// ... dans C.cpp: void C::heavy_function() const { ... }

// à éviter
class C {
public:
    void heavy_function() const {    // corps dans le header — implicitement inline
        // 50 lignes de logique
    }
};
```

Un inline implicite signifie que chaque TU incluant le header recompile le corps.

### 11.4 Ordonnez les members par alignement pour optimiser la taille

Pour les classes de données pures, trier les members du plus grand alignement au plus petit minimise le padding :

```cpp
struct Bad  { char a; double d; char b; };       // 24 octets
struct Good { double d; char a; char b; };       // 16 octets
```

Pour du code de niveau 42, cela importe rarement. Pour des boucles serrées sur des millions d'objets, cela compte énormément.

### 11.5 Idiome Pimpl — masquer l'implémentation

Si vous souhaitez que le header expose uniquement l'interface publique et masque entièrement les data members :

```cpp
// MyClass.hpp
class MyClass {
public:
    MyClass();
    ~MyClass();
    void doStuff();
private:
    class Impl;                   // forward decl
    Impl *_impl;                  // pointer vers le type masqué
};
```

```cpp
// MyClass.cpp
class MyClass::Impl {
    int _bigInternalState;
    // ...
};

MyClass::MyClass() : _impl(new Impl()) {}
MyClass::~MyClass() { delete _impl; }
```

Avantages : le header n'inclut pas de headers d'implémentation lourds ; ABI stable. Inconvénients : allocation sur la heap supplémentaire par objet. Non imposé par la norme 42, mais un excellent pattern à connaître.

---

## 12. `class` vs `typedef struct` (C → C++)

```c
// C
typedef struct s_node {
    int data;
    struct s_node *next;
} t_node;
```

```cpp
// C++
class Node {
public:
    int   data;
    Node *next;
};
```

En C++, vous n'avez pas besoin de `typedef` ; le nom de la class est automatiquement un type. Vous n'avez pas non plus besoin d'écrire `struct Node` — simplement `Node`.

---

## 13. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `private member 'X::_y' is inaccessible` | Accès à un member private depuis l'extérieur | Ajouter un getter/setter, ou déclarer l'accesseur en tant que `friend` |
| `class X has no member named 'foo'` | Faute de frappe ou member déclaré dans la mauvaise section | Vérifier l'orthographe et le niveau d'accès |
| `field has incomplete type` | Utilisation par valeur d'un type déclaré avec une forward declaration | Inclure la définition complète, ou stocker un pointer/reference |
| `unresolved external symbol Class::method` | Déclaré mais jamais défini | Fournir le corps dans le `.cpp` |

---

## 14. Résumé visuel

```
                ┌────────────────────────────────────┐
                │   class C { … };                   │
                └─────────────────┬──────────────────┘
                                  │
              ┌───────────────────┼───────────────────┐
              ▼                   ▼                   ▼
         data members         member functions    access control
         ─────────────        ────────────────    ──────────────
         résident dans la     résident dans .text appliqués au compile
         mémoire de chaque    partagés par toutes time. private par
         objet (avec padding  les instances;      défaut. private/
         pour l'alignement)   reçoivent 'this'    protected/public
                              comme premier
                              paramètre masqué.
              │
              ▼
         sizeof(C) =
         somme des members
         + padding
         + (8 octets pour ptr
            vtable si virtual)
```

---

## 15. Pratique

1. Quel est le `sizeof(C)` pour `class C { char x; };` ? *(1 octet. Aucun padding nécessaire.)*
2. Qu'est-ce qui change si vous ajoutez `int y;` après `x` ? *(8 octets — 1 pour x, 3 de padding, 4 pour y.)*
3. Que se passe-t-il si vous faites une forward declaration d'une class et que vous la passez par valeur ? *(Erreur de compilation — le compiler a besoin de connaître la taille.)*
4. Pourquoi `sizeof(empty class) >= 1` ? *(Pour que des objets distincts aient des adresses distinctes.)*