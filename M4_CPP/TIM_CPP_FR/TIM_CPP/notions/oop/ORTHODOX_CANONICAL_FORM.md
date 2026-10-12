# La Forme Canonique Orthodoxe (FCO) — Guide Complet

> **Zoom sur les mots-clés :** [`OPERATOR`](../../lexique/OPERATOR.md) · [`THIS`](../../lexique/THIS.md) · [`EXPLICIT`](../../lexique/EXPLICIT.md) · [`CONST`](../../lexique/CONST.md) · [`NEW`](../../lexique/NEW.md) · [`DELETE`](../../lexique/DELETE.md)

## Table des Matières
1. [Qu'est-ce que la Forme Canonique Orthodoxe ?](#What%20is%20the%20Orthodox%20Canonical%20Form%3F)
2. [Pourquoi existe-t-elle ?](#Why%20does%20it%20exist%3F)
3. [Les Quatre Membres Obligatoires](#The%20Four%20Mandatory%20Members)
   - [1. Constructeur par Défaut](#1.%20Default%20Constructor)
   - [2. Constructeur de Copie](#2.%20Copy%20Constructor)
   - [3. Opérateur d'Assignation par Copie](#3.%20Copy%20Assignment%20Operator)
   - [4. Destructeur](#4.%20Destructor)
4. [La Règle des Trois (et des Cinq, et de Zéro)](#The%20Rule%20of%20Three%20%28and%20Five%2C%20and%20Zero%29)
5. [Copie Profonde vs Copie Superficielle](#Deep%20Copy%20vs%20Shallow%20Copy)
6. [Pièges Courants](#Common%20Pitfalls)
7. [Exemples Complets](#Full%20Examples)

---

## Qu'est-ce que la Forme Canonique Orthodoxe ?

La **Forme Canonique Orthodoxe** (FCO, ou OCF pour *Orthodox Canonical Form*), parfois appelée la **Forme de Coplien** (d'après James Coplien qui l'a décrite dans *Advanced C++ Programming Styles and Idioms*, 1992), est une convention de codage en C++ qui exige que chaque class définisse **explicitement quatre fonctions membres spécifiques** :

1. Un **default constructor**
2. Un **copy constructor**
3. Un **copy assignment operator** (`operator=`)
4. Un **destructor**

Cette convention est **strictement appliquée à l'École 42** (et dans de nombreuses bases de code professionnelles en C++98) pour toutes les classes non triviales. Elle garantit que vous, le programmeur, gardez **toujours le contrôle** sur la façon dont vos objets sont créés, copiés, assignés et détruits.

> **En résumé** : l'OCF est le contrat que vous signez avec le compiler — « Je sais exactement comment mon objet se comporte tout au long de son cycle de vie. »

---

## Pourquoi existe-t-elle ?

Lorsque vous écrivez une class en C++ sans définir ces fonctions, le **compiler les génère implicitement**. Cela semble pratique, mais c'est dangereux :

- Le copy constructor généré par le compiler effectue une **shallow copy** (copie superficielle, membre par membre). Si votre class gère une ressource (mémoire sur la heap, file descriptor, socket, mutex...), cela mène à des **double frees**, des **dangling pointers**, et des **undefined behaviors**.
- Les fonctions implicites sont silencieuses — vous ne pouvez pas voir d'un coup d'œil ce que fait votre class lorsqu'elle est copiée ou détruite.
- Sans destructor explicite, les ressources que vous avez allouées (par exemple, avec `new`) vont **fuiter**.

Définir explicitement ces quatre fonctions rend votre class :
- **Prévisible** : le comportement est documenté dans le code.
- **Sûre** : aucune shallow copy silencieuse de ressources possédées.
- **Maintenable** : les futurs lecteurs voient immédiatement comment le cycle de vie est géré.

---

## Les Quatre Membres Obligatoires

Prenons une class `Foo` comme fil conducteur.

### 1. Constructeur par Défaut

**Signature :**
```cpp
Foo();
```

**Ce qu'il fait :**
Le default constructor est appelé lorsque vous créez un objet **sans arguments**. Il initialise les membres de l'objet dans un état valide et défini.

**Quand est-il appelé ?**
- `Foo a;` — déclaration explicite sans arguments.
- `Foo arr[10];` — lorsqu'un tableau d'objets est créé.
- Lorsque la class est utilisée comme membre d'une autre class qui utilise elle-même son default constructor.
- Lorsque vous écrivez `Foo()` pour créer un temporaire.

**Pourquoi le définir explicitement ?**
- Si vous définissez **n'importe quel autre constructeur** (par exemple, `Foo(int x)`), le compiler **cesse de générer** celui par défaut. Ainsi, `Foo a;` échouerait à la compilation à moins que vous ne définissiez `Foo()` vous-même.
- Il garantit que vos membres démarrent dans un **état connu** (pas de valeurs résiduelles pour les primitives comme `int`, qui ne sont pas initialisées à zéro par défaut en C++98).

---

### 2. Constructeur de Copie

**Signature :**
```cpp
Foo(const Foo& other);
```

**Ce qu'il fait :**
Construit un **nouvel objet** en tant que copie d'un objet existant. Il est appelé lorsqu'un objet est en cours de **création à partir d'un autre objet du même type**.

**Quand est-il appelé ?**
- `Foo a; Foo b(a);` — construction par copie explicite.
- `Foo b = a;` — initialisation par copie (attention : ce n'est **pas** une assignation, c'est une construction !).
- Lorsqu'un objet est **passé par valeur** à une fonction : `void func(Foo f);` appelé avec `func(a);`.
- Lorsqu'un objet est **retourné par valeur** depuis une fonction (bien que le compiler puisse éliminer cela grâce au RVO/NRVO).
- Lors de l'insertion d'un objet dans un conteneur STL (en C++98).

**Pourquoi le définir explicitement ?**
- La version générée par le compiler copie chaque membre **au niveau bit à bit**. Si un membre est un pointer, l'original et la copie pointeront tous deux vers la **même mémoire** — lorsque l'un d'eux est détruit et libère cette mémoire, l'autre devient un **dangling pointer**.
- Vous pourriez vouloir **journaliser** les copies, ou les **interdire** (en déclarant le copy constructor `private` sans le définir).

**Important :**
- Le paramètre doit être une **`const` reference** (`const Foo&`). S'il était passé par valeur, l'appel du copy constructor nécessiterait lui-même... un copy constructor → récursion infinie.

---

### 3. Opérateur d'Assignation par Copie

**Signature :**
```cpp
Foo& operator=(const Foo& other);
```

**Ce qu'il fait :**
Assigne le contenu d'un objet **déjà construit** à un autre objet **déjà construit**. Il ne crée **pas** de nouvel objet — il modifie un objet existant.

**Quand est-il appelé ?**
- `Foo a; Foo b; b = a;` — assignation explicite entre objets existants.
- N'est **pas** appelé par `Foo b = a;` (c'est le copy constructor !).

**Pourquoi le définir explicitement ?**
- Même raison que pour le copy constructor : le compiler génère une shallow copy bit à bit.
- Si votre objet possède des ressources, vous devez :
  1. **Libérer** toute ressource actuellement détenue par `*this`.
  2. **Allouer de nouvelles ressources** et copier les données depuis `other`.
  3. **Retourner `*this`** par reference pour permettre le chaînage : `a = b = c;`.

**Checklist des bonnes pratiques :**
- ✅ Vérifier l'**auto-assignation** : `if (this != &other)` — assigner un objet à lui-même peut conduire à libérer la mémoire puis à tenter de copier depuis celle-ci.
- ✅ Retourner `*this` par reference (`Foo&`).
- ✅ Prendre le paramètre en tant que `const Foo&`.

---

### 4. Destructeur

**Signature :**
```cpp
~Foo();
```

**Ce qu'il fait :**
Nettoie un objet avant que sa mémoire ne soit récupérée. C'est la **dernière fonction** appelée dans le cycle de vie d'un objet.

**Quand est-il appelé ?**
- Lorsqu'un objet alloué sur la stack sort de la **portée (scope)** : `{ Foo a; } // ~Foo() appelé ici`.
- Lorsque `delete` est appelé sur un objet alloué sur la heap : `Foo* a = new Foo(); delete a;`.
- Lorsqu'un objet temporaire est détruit à la fin de l'expression complète.
- Lorsqu'un objet conteneur est détruit (les destructeurs des membres sont appelés automatiquement, dans l'ordre inverse de leur déclaration).

**Pourquoi le définir explicitement ?**
- Si votre class alloue des ressources (`new`, `open`, `malloc`, etc.), vous **devez** les libérer dans le destructor sous peine de fuite.
- Un destructor est le **seul endroit** où vous pouvez garantir que le nettoyage a lieu — même si des exceptions sont levées.
- Pour les classes de base polymorphiques, le destructor doit être **`virtual`** pour s'assurer que les destructeurs dérivés s'exécutent lors de la suppression via un base pointer.

---

## La Règle des Trois (et des Cinq, et de Zéro)

La **Règle des Trois** (C++98) stipule :

> Si une class a besoin d'un **destructor**, d'un **copy constructor** ou d'un **copy assignment operator** explicite, elle a presque certainement besoin des trois.

Pourquoi ? Parce que tous les trois traitent de la **gestion des ressources**. Si votre destructor libère de la mémoire, votre copy constructor doit dupliquer cette mémoire (sinon les deux copies libèrent le même bloc), et votre assignment operator doit faire de même (et aussi nettoyer d'abord la ressource existante).

La **Forme Canonique Orthodoxe** est une version plus **stricte** : elle ajoute le **default constructor** à cette liste, formant ainsi la **Règle des Quatre**.

> *(Le C++ moderne ajoute la Règle des Cinq — incluant le move constructor et le move assignment — et la Règle de Zéro — concevoir des classes qui n'ont besoin d'aucun d'entre eux en s'appuyant sur des types RAII comme `std::vector`, `std::unique_ptr`. Mais ceux-ci sont postérieurs au C++98 et hors de portée du cursus 42 pour CPP00–CPP04.)*

---

## Copie Profonde vs Copie Superficielle

C'est la **raison fondamentale** de l'existence de l'OCF.

### Shallow copy (par défaut, dangereux en cas de possession de ressources)
```cpp
class Bad {
    int* data;
public:
    Bad() { data = new int(42); }
    ~Bad() { delete data; }
    // Pas de constructeur de copie → le compilateur en génère un qui copie le POINTEUR, pas la donnée
};

Bad a;
Bad b(a);   // b.data == a.data (même mémoire !)
// Lorsque a et b sont détruits → delete appelé deux fois sur le même pointeur → comportement indéfini 💥
```

### Deep copy (correct)
```cpp
class Good {
    int* data;
public:
    Good() : data(new int(42)) {}
    Good(const Good& other) : data(new int(*other.data)) {} // alloue de la nouvelle mémoire, copie la valeur
    Good& operator=(const Good& other) {
        if (this != &other) {
            delete data;
            data = new int(*other.data);
        }
        return *this;
    }
    ~Good() { delete data; }
};
```

---

## Pièges Courants

| Piège | Conséquence |
|---|---|
| Oublier `const` sur le paramètre du copy constructor | Impossible de copier des objets `const`, impossible de copier des temporaires |
| Oublier de retourner `*this` depuis `operator=` | Impossible de chaîner les assignations (`a = b = c;`) |
| Oublier la vérification d'auto-assignation | Ressource libérée avant d'être copiée — UB |
| Oublier `virtual` sur le destructor d'une classe de base | Le destructeur dérivé ne s'exécutera pas lors d'un delete via un base pointer — fuites |
| Définir un constructeur non par défaut sans redéfinir celui par défaut | `Foo a;` ne compile plus |
| Implémenter la copie comme une shallow copy quand la class possède de la mémoire | Double-free, dangling pointers |
| Retourner `operator=` par valeur au lieu de le retourner par reference | Copie inutile, casse la sémantique de chaînage |
---

## Exemples complets

### Exemple 1 : Une class triviale (aucune ressource possédée)

```cpp
// Animal.hpp
#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <string>

class Animal {
private:
    std::string _name;
    int         _age;

public:
    Animal();                                    // default constructor
    Animal(const std::string& name, int age);    // parameterized constructor
    Animal(const Animal& other);                 // copy constructor
    Animal& operator=(const Animal& other);      // copy assignment operator
    ~Animal();                                   // destructor

    const std::string& getName() const;
    int                getAge() const;
};

#endif
```

```cpp
// Animal.cpp
#include "Animal.hpp"
#include <iostream>

Animal::Animal() : _name("Unnamed"), _age(0) {
    std::cout << "Default constructor called" << std::endl;
}

Animal::Animal(const std::string& name, int age) : _name(name), _age(age) {
    std::cout << "Parameterized constructor called for " << _name << std::endl;
}

Animal::Animal(const Animal& other) : _name(other._name), _age(other._age) {
    std::cout << "Copy constructor called for " << _name << std::endl;
}

Animal& Animal::operator=(const Animal& other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other) {
        _name = other._name;
        _age  = other._age;
    }
    return *this;
}

Animal::~Animal() {
    std::cout << "Destructor called for " << _name << std::endl;
}

const std::string& Animal::getName() const { return _name; }
int                Animal::getAge()  const { return _age; }
```

```cpp
// main.cpp
#include "Animal.hpp"

int main() {
    Animal a;                       // default constructor
    Animal b("Rex", 5);             // parameterized constructor
    Animal c(b);                    // copy constructor
    Animal d = b;                   // AUSSI copy constructor (initialisation)
    a = b;                          // copy assignment operator

    return 0;
    // destructors appeles pour d, c, b, a (ordre inverse de construction)
}
```

**Sortie attendue :**
```
Default constructor called
Parameterized constructor called for Rex
Copy constructor called for Rex
Copy constructor called for Rex
Copy assignment operator called
Destructor called for Rex
Destructor called for Rex
Destructor called for Rex
Destructor called for Rex
```

---

### Exemple 2 : Une class gérant une ressource allouée sur la heap (deep copy requise)

```cpp
// Buffer.hpp
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstddef>

class Buffer {
private:
    char*  _data;
    size_t _size;

public:
    Buffer();                                    // default constructor
    Buffer(size_t size);                         // parameterized constructor
    Buffer(const Buffer& other);                 // copy constructor (deep copy)
    Buffer& operator=(const Buffer& other);      // copy assignment operator (deep copy)
    ~Buffer();                                   // destructor (frees memory)

    size_t getSize() const;
    char*  getData() const;
};

#endif
```

```cpp
// Buffer.cpp
#include "Buffer.hpp"
#include <cstring>
#include <iostream>

Buffer::Buffer() : _data(NULL), _size(0) {
    std::cout << "Default Buffer constructed (empty)" << std::endl;
}

Buffer::Buffer(size_t size) : _data(new char[size]), _size(size) {
    std::memset(_data, 0, size);
    std::cout << "Buffer of size " << size << " constructed" << std::endl;
}

Buffer::Buffer(const Buffer& other) : _data(NULL), _size(other._size) {
    if (_size > 0) {
        _data = new char[_size];
        std::memcpy(_data, other._data, _size);
    }
    std::cout << "Buffer copy-constructed (deep copy of " << _size << " bytes)" << std::endl;
}

Buffer& Buffer::operator=(const Buffer& other) {
    std::cout << "Buffer assignment operator called" << std::endl;
    if (this != &other) {            // protection contre l'auto-assignation
        delete[] _data;              // libere l'ancienne ressource
        _size = other._size;
        if (_size > 0) {
            _data = new char[_size]; // alloue la nouvelle
            std::memcpy(_data, other._data, _size);
        } else {
            _data = NULL;
        }
    }
    return *this;
}

Buffer::~Buffer() {
    std::cout << "Buffer destructed (freeing " << _size << " bytes)" << std::endl;
    delete[] _data;
}

size_t Buffer::getSize() const { return _size; }
char*  Buffer::getData() const { return _data; }
```

```cpp
// main.cpp
#include "Buffer.hpp"

int main() {
    Buffer a(128);     // alloue 128 octets
    Buffer b(a);       // deep copy -> b a ses PROPRES 128 octets
    Buffer c;          // vide
    c = a;             // deep copy via assignation

    return 0;
    // chaque destructor nettoie sa PROPRE memoire — pas de double-free
}
```

---

### Exemple 3 : Interdire la copie (style singleton / non-copyable)

Parfois, vous **ne voulez pas** qu'une class soit copiable (par exemple, une class gérant un handle unique). L'idiome C++98 consiste à déclarer le copy constructor et le copy assignment operator en **`private`** et à ne **pas les implémenter** :

```cpp
// Logger.hpp
class Logger {
public:
    Logger();
    ~Logger();
    void log(const std::string& msg);

private:
    Logger(const Logger& other);              // declared but not defined
    Logger& operator=(const Logger& other);   // declared but not defined
};
```

Désormais, toute tentative de copie d'un `Logger` échouera au **compile time** (ou au link time s'il est appelé depuis un friend/membre). Ceci est toujours considéré comme respectant l'OCF — vous avez **explicitement géré** les quatre cas.

---

### Exemple 4 : Une class avec un destructor `virtual` (polymorphisme)

```cpp
class Base {
public:
    Base();
    Base(const Base& other);
    Base& operator=(const Base& other);
    virtual ~Base();              // VIRTUAL — crucial for safe polymorphic deletion
    virtual void speak() const;
};

class Derived : public Base {
private:
    int* _resource;
public:
    Derived();
    Derived(const Derived& other);
    Derived& operator=(const Derived& other);
    ~Derived();                   // automatically virtual because parent's is
    virtual void speak() const;
};
```

Sans `virtual ~Base()`, ce code :
```cpp
Base* b = new Derived();
delete b;   // calls only ~Base() — ~Derived() never runs → leaks _resource!
```
...fuirait silencieusement. Avec `virtual`, les deux destructors s'exécutent dans le bon ordre (dérivé d'abord, puis base).

---

## Fiche de référence rapide

```cpp
class ClassName {
public:
    ClassName();                                       // 1. Default constructor
    ClassName(const ClassName& other);                 // 2. Copy constructor
    ClassName& operator=(const ClassName& other);      // 3. Copy assignment operator
    ~ClassName();                                      // 4. Destructor
};
```

| Fonction | Quand elle s'exécute | Retourne | Paramètre |
|---|---|---|---|
| Default constructor | `ClassName a;` | rien | aucun |
| Copy constructor | `ClassName b(a);` ou `ClassName b = a;` | rien | `const ClassName&` |
| Copy assignment | `b = a;` (les deux existent déjà) | `ClassName&` | `const ClassName&` |
| Destructor | sortie de scope / `delete` | rien | aucun |

---

**TL;DR** : La Forme Canonique Orthodoxe est la discipline du C++98 consistant à toujours définir explicitement les quatre fonctions de cycle de vie afin que la création, la copie, l'assignation et la destruction des objets ne soient jamais laissées aux comportements par défaut silencieux du compiler. Elle empêche les fuites de ressources, les double frees et les dangling pointers dans toute class possédant des ressources — et rend l'intention parfaitement claire dans les classes qui n'en possèdent pas.

---

## Vue matérielle — ce en quoi les quatre membres se compilent

### Default ctor (aucune trace, membres primitifs)

```cpp
class Fixed { int raw; };
Fixed::Fixed() : raw(0) {}
```

Se compile en (x86-64) :

```asm
Fixed::Fixed():
    mov  dword [rdi], 0       ; this->raw = 0
    ret
```

Trois instructions. Un store et un return. Avec `-O2` plus
l'inlining au point d'appel, le ctor peut disparaître entièrement si
l'emplacement sur la stack de l'appelant est déjà initialisé à zéro.

### Copy ctor sur une class ne contenant que des types primitifs

```cpp
Fixed::Fixed(Fixed const& src) : raw(src.raw) {}
```

```asm
Fixed::Fixed(Fixed const&):
    mov  eax, [rsi]           ; load other.raw
    mov  [rdi], eax           ; this->raw = other.raw
    ret
```

Un load de 4 octets, un store de 4 octets. Pour une class avec un
membre `std::string`, ajoutez une allocation pour le buffer sur la heap — des dizaines de nanosecondes
contre ~1 cycle pour le cas avec uniquement des primitives.

### Copy assignment avec protection contre l'auto-assignation

```cpp
Fixed& operator=(Fixed const& o) {
    if (this != &o) raw = o.raw;
    return *this;
}
```

```asm
Fixed::operator=(Fixed const&):
    cmp  rdi, rsi             ; this vs &other
    je   .skip
    mov  eax, [rsi]
    mov  [rdi], eax
.skip:
    mov  rax, rdi             ; return *this
    ret
```

La protection prend deux instructions (`cmp` + `je`). Pour des primitives, elle
coûte une branche. Pour les classes possédant des ressources, la protection empêche
`delete old; this = old;` — ce qui serait catastrophique.

### Destructor pour une class possédant des ressources
```cpp
class Dog { Brain* brain; };
Dog::~Dog() { delete brain; }
```

`delete p` inclut une vérification de null implicite — `delete NULL;` est un no-op défini. C'est pourquoi vous pouvez `delete` un membre sans vérifier au préalable s'il est null.

### Copy elision — quand le copy ctor est silencieusement ignoré

```cpp
Fixed make() { return Fixed(42); }
Fixed f = make();
```

Avec n'importe quel compiler moderne, **un seul** `Fixed` est construit. Le compiler applique la **return value optimisation (RVO)** : il construit `Fixed(42)` directement dans l'espace de stockage de `f`, en ignorant le copy ctor. L'affichage de trace de votre copy ctor **ne se déclenchera pas**, même si le langage a « conceptuellement » effectué une copie.

Règle empirique : ne comptez jamais sur un nombre précis d'affichages de copy ctor dans du code qui passe ou retourne par valeur. La Moulinette teste généralement des **copies explicites** (`Foo b(a);`, et non `Foo b = make();`) pour éviter l'ambiguïté liée à la RVO — mais si votre sortie contient un « copy ctor called » de moins que prévu, la RVO en est probablement la cause.

### Vtable pointer et OCF

Toute class possédant des méthodes `virtual` a (généralement) un vptr comme premier membre. Le copy ctor / copy-assign généré par le compiler **ne copie pas** le vptr depuis la source — chaque ctor initialise le vptr vers la propre vtable de la class. C'est pourquoi l'object slicing fonctionne : copier un `Dog` dans un emplacement d'`Animal` laisse le vptr de destination pointer vers la vtable d'`Animal`, et non celle de `Dog`, de sorte que les appels virtuels ultérieurs sont dispatchés en tant qu'`Animal`.