# Les bases du C++ — Du C au C++

Écrit pour quelqu'un qui connaît le C et commence les modules CPP de 42.

> **Zoom sur les mots-clés :** [`CLASS`](../../lexique/CLASS.md) · [`STRUCT`](../../lexique/STRUCT.md) · [`PUBLIC/PRIVATE/PROTECTED`](../../lexique/PUBLIC_PRIVATE_PROTECTED.md) · [`CONST`](../../lexique/CONST.md) · [`STATIC`](../../lexique/STATIC.md) · [`THIS`](../../lexique/THIS.md) · [`NAMESPACE`](../../lexique/NAMESPACE.md) · [`NEW`](../../lexique/NEW.md) · [`DELETE`](../../lexique/DELETE.md). Index complet des mots-clés : [`lexique/INDEX.md`](../../lexique/INDEX.md).

---

## 1. Qu'est-ce que le C++ et pourquoi existe-t-il ?

Le C++ a été créé par Bjarne Stroustrup au début des années 1980. Son nom initial était **« C with Classes »** — ce qui reste la description la plus honnête de ce qu'il est.

La motivation principale : le C vous donne un contrôle total sur la mémoire et le matériel, mais il ne vous donne **aucun outil pour organiser de grands programmes**. À mesure que les programmes grandissent, les structs brutes combinées aux function pointers deviennent ingérables. Le C++ ajoute une manière structurée de regrouper **données + comportement**, d'imposer des **invariants**, et de réutiliser du code via **l'héritage et le polymorphisme** — sans sacrifier les performances du C.

Le principe directeur du C++ est :
> **Vous ne payez pas pour ce que vous n'utilisez pas (« You don't pay for what you don't use »).**

Si vous n'utilisez pas de fonctions virtuelles, il n'y a aucun overhead de vtable. Si vous n'utilisez pas d'exceptions, il n'y a aucun coût lié au traitement des exceptions. C'est ce qui distingue le C++ de Java ou Python.

---

## 2. Le changement de modèle mental : du C au C++

En C, vous pensez en termes de :
- Fonctions qui manipulent des données
- Structs qui contiennent des données
- Pointers qui les relient

En C++, vous pensez en termes de :
- **Objets** qui *sont* quelque chose et *peuvent faire* des choses
- **Classes** qui définissent ce que ces objets sont
- **Interfaces** qui définissent ce que différents objets ont en commun

Le changement est subtil mais fondamental. En C, vous écrivez :

```c
// C : fonctions qui agissent sur des données
t_phonebook *pb = phonebook_create();
phonebook_add(pb, contact);
phonebook_destroy(pb);
```

En C++, vous écrivez :

```cpp
// C++ : objets qui possèdent des données et gèrent leur propre durée de vie
PhoneBook pb;       // le constructor s'exécute automatiquement
pb.add(contact);
// le destructor s'exécute automatiquement lorsque pb sort du scope
```

Ce n'est pas simplement du sucre syntaxique. L'objet est **responsable de lui-même**.

---

## 3. Forces du C++

| Force | Ce que cela signifie en pratique |
|---|---|
| **Performance** | Aussi rapide que le C. Pas de runtime, pas de GC, pas de VM. |
| **Zero-cost abstractions** | Les classes, templates, fonctions inline disparaissent à la compilation. |
| **RAII** | Les ressources (mémoire, fichiers, verrous) sont liées à la durée de vie de l'objet — elles sont libérées automatiquement. |
| **Compatibilité avec le C** | Vous pouvez appeler n'importe quelle bibliothèque C directement. |
| **Expressivité** | L'operator overloading, les references, les templates vous permettent d'écrire un code très naturel. |
| **Contrôle** | Vous décidez exactement où réside la mémoire (stack vs heap). |

---

## 4. Faiblesses du C++

| Faiblesse | Ce que cela signifie en pratique |
|---|---|
| **Complexité** | Le langage possède une surface gigantesque. Beaucoup de façons de faire la même chose. |
| **Pas de memory safety** | Vous êtes toujours exposé aux dangling pointers, use-after-free, buffer overflows. Contrairement à Rust. |
| **Compilation lente** | Les headers sont inclus et réanalysés partout. Les templates empirent la situation. |
| **Fardeau historique (legacy baggage)** | Le C++ doit rester rétrocompatible avec le C. De vieux anti-patterns compilent toujours. |
| **Undefined behavior** | De nombreuses erreurs (overflow, déréférencement de null, lectures non initialisées) sont des UB — le compiler peut faire n'importe quoi. |
| **Messages d'erreur difficiles** | Les erreurs de template sont notoirement illisibles. |

---

## 5. C++ vs C — ce qui reste identique

Tout ce que vous connaissez du C fonctionne toujours :

- Tous les types de base : `int`, `char`, `float`, `double`, `size_t`, ...
- Tous les opérateurs : `+`, `-`, `*`, `/`, `%`, `&`, `|`, `>>`, `<<`, ...
- Pointers : même syntaxe, mêmes règles, mêmes dangers
- Tableaux : mêmes règles pour la stack/heap
- Contrôle de flux : `if`, `for`, `while`, `switch`, `return`
- Le préprocesseur : `#include`, `#define`, `#ifndef`, `#pragma once`
- `malloc` / `free` existent toujours et fonctionnent toujours
- Tous les headers de la bibliothèque standard C (`<cstdio>`, `<cstring>`, `<cmath>`, ...)
- `main(int argc, char **argv)` est identique

**Règle générale :** si cela compile en C89, cela compile presque certainement en C++.

---

## 6. C++ vs C — ce qui est différent

### 6.1 `new` / `delete` au lieu de `malloc` / `free`

```c
// C
int *arr = malloc(10 * sizeof(int));
free(arr);
```

```cpp
// C++
int *arr = new int[10];
delete[] arr;

// Objet unique
MyClass *obj = new MyClass();
delete obj;  // appelle d'abord le destructor, puis libère la mémoire
```

Différences clés :
- `new` appelle le **constructor**. `malloc` ne le fait pas.
- `delete` appelle le **destructor**. `free` ne le fait pas.
- `new` lance une exception `std::bad_alloc` en cas d'échec (pas besoin de vérifier la présence de NULL, sauf si vous utilisez `new (std::nothrow)`).
- Dans les modules CPP de 42, **utilisez toujours `new`/`delete`**, jamais `malloc`/`free`.

---

### 6.2 References — un alias, pas un pointer

Une reference est un alias pour une variable existante. Elle ne peut pas être NULL et ne peut pas être réassignée (reseated).

```cpp
int x = 42;
int &ref = x;    // ref EST x — même adresse mémoire
ref = 100;       // x vaut maintenant 100

// Pointers vs references
void doubleVal_ptr(int *p) { *p *= 2; }   // Style C, l'appelant passe &x
void doubleVal_ref(int &r) { r  *= 2; }   // Style C++, l'appelant passe x directement

int n = 5;
doubleVal_ptr(&n);   // n = 10
doubleVal_ref(n);    // n = 20  — pas de & à l'appel
```

**Quand utiliser quoi :**
- Utilisez des **references** pour les paramètres de fonction lorsque vous voulez modifier la variable de l'appelant (aucun NULL possible).
- Utilisez des **const references** pour passer de gros objets à moindre coût sans les copier.
- Utilisez des **pointers** quand la valeur peut être NULL, ou quand vous devez réassigner le pointer (changer ce vers quoi il pointe).

```cpp
// Passer un gros objet à faible coût — const reference
void print(const std::string &s) { std::cout << s; }

// Ne compilera PAS — les references doivent être initialisées
int &bad;         // erreur : reference non liée
int *ok = nullptr; // les pointers peuvent être null
```

---

### 6.3 Namespaces

Les namespaces empêchent les collisions de noms entre bibliothèques.

```cpp
namespace ft {
    int max(int a, int b) { return a > b ? a : b; }
}

int main() {
    ft::max(3, 5);        // namespace explicite
    using ft::max;        // importe un nom spécifique
    max(3, 5);            // fonctionne maintenant

    using namespace ft;   // importe tout (à éviter dans les headers)
}
```

`std::` est le namespace de la bibliothèque standard. Chaque fonction ou type de la bibliothèque standard s'y trouve.

**Règle 42 :** N'écrivez jamais `using namespace std;` dans un fichier header. C'est acceptable dans les fichiers `.cpp`, mais la norme 42 préfère souvent l'utilisation explicite de `std::`.

---

### 6.4 `std::string` au lieu de `char *`

```c
// C
char *s = strdup("hello");
strcat(s, " world");
free(s);
```

```cpp
// C++
std::string s = "hello";
s += " world";              // gestion automatique de la mémoire
s.length();                 // 11
s.substr(0, 5);             // "hello"
s.find("world");            // 6
s.empty();                  // false
```

`std::string` gère sa propre mémoire. Pas de `malloc`, pas de `free`, pas de buffer overflows.

---

### 6.5 Function overloading

En C, vous ne pouvez pas avoir deux fonctions avec le même nom. En C++, c'est possible, tant qu'elles ont des types de paramètres différents :

```cpp
int    add(int a, int b)       { return a + b; }
double add(double a, double b) { return a + b; }
std::string add(std::string a, std::string b) { return a + b; }

add(1, 2);         // appelle la version int
add(1.0, 2.0);     // appelle la version double
add("hi", "bye");  // appelle la version string
```

---

### 6.6 Default arguments

```cpp
void greet(std::string name, std::string title = "Mr.") {
    std::cout << "Hello, " << title << " " << name << '\n';
}

greet("Smith");           // Hello, Mr. Smith
greet("Smith", "Dr.");    // Hello, Dr. Smith
```

Les default arguments doivent être placés **à la fin** de la liste des paramètres, et doivent être déclarés dans le **header** (pas dans le `.cpp`).

---

## 7. Classes — le cœur du C++

Une class est une struct qui intègre également ses fonctions (appelées **member functions** ou **méthodes**), un contrôle d'accès et des fonctions spéciales pour son cycle de vie.

```cpp
// struct C
typedef struct s_point {
    int x;
    int y;
} t_point;

// class C++
class Point {
private:
    int _x;   // convention 42 : préfixer les membres private par _
    int _y;

public:
    // Constructor : s'exécute lorsque l'objet est créé
    Point(int x, int y) : _x(x), _y(y) {}

    // Member functions
    int getX() const { return _x; }
    int getY() const { return _y; }

    void move(int dx, int dy) { _x += dx; _y += dy; }
};

int main() {
    Point p(3, 4);         // alloué sur la stack, constructor appelé
    p.move(1, 2);
    std::cout << p.getX(); // 4

    Point *q = new Point(0, 0); // alloué sur la heap
    delete q;                   // destructor appelé, mémoire libérée
}
```

### Access control

| Mot-clé     | Accessible depuis            |
|-------------|------------------------------|
| `public`    | N'importe où                 |
| `private`   | Uniquement dans la class elle-même |
| `protected` | Dans la class + ses sous-classes |

**Accès par défaut :** `struct` est `public` par défaut, `class` est `private` par défaut.
---

## 8. La Forme Canonique Orthodoxe (OCF) — obligatoire pour 42

Toute class en CPP à 42 doit implémenter ces quatre fonctions, sauf s'il existe une raison explicite de ne pas le faire :

```
1. Default constructor       MyClass();
2. Copy constructor          MyClass(const MyClass& other);
3. Copy assignment operator  MyClass& operator=(const MyClass& other);
4. Destructor                ~MyClass();
```

Il s'agit de la **Rule of Three** (Rule of Five en C++ moderne, mais 42 utilise le C++98).

```cpp
// Contact.hpp
class Contact {
private:
    std::string _firstName;
    std::string _lastName;
    int         _index;

public:
    Contact();                              // 1. Default constructor
    Contact(const Contact& other);          // 2. Copy constructor
    Contact& operator=(const Contact& other); // 3. Copy assignment
    ~Contact();                             // 4. Destructor

    // Getters et setters
    std::string getFirstName() const;
    void        setFirstName(const std::string& name);
};
```

```cpp
// Contact.cpp
Contact::Contact() : _firstName(""), _lastName(""), _index(0) {}

Contact::Contact(const Contact& other)
    : _firstName(other._firstName),
      _lastName(other._lastName),
      _index(other._index) {}

Contact& Contact::operator=(const Contact& other) {
    if (this != &other) {           // protection contre l'auto-assignation
        _firstName = other._firstName;
        _lastName  = other._lastName;
        _index     = other._index;
    }
    return *this;                   // retourne toujours *this
}

Contact::~Contact() {}             // les membres string se nettoient d'eux-mêmes
```

**Pourquoi est-ce important ?**
Si vous allouez de la mémoire avec `new` à l'intérieur d'une class et que vous n'écrivez pas de copy constructor, C++ effectuera une **shallow copy** — en copiant le pointer, et non les données. Deux objets pointent alors vers la même mémoire. Lorsque l'un est détruit, il la libère. L'autre se retrouve avec un dangling pointer. C'est un bug C++ classique.

---

## 9. L'initialization list — utilisez-la toujours

Préférez l'initialization list à l'assignation dans le corps du constructor :

```cpp
// Mauvais (construit par défaut puis assigne)
Contact::Contact(std::string name) {
    _name = name;   // _name a déjà été construit par défaut, maintenant réassigné
}

// Bon (initialise directement)
Contact::Contact(std::string name) : _name(name) {}
```

L'initialization list est **obligatoire** pour :
- Les variables membres `const`
- Les variables membres de type reference
- Les objets membres sans default constructor

---

## 10. `const` — bien plus puissant qu'en C

En C++, `const` peut être appliqué aux member functions pour signifier « cette fonction ne modifie pas l'objet » :

```cpp
class Circle {
private:
    double _radius;

public:
    Circle(double r) : _radius(r) {}

    // member function const : peut être appelée sur des objets const
    double area() const { return 3.14159 * _radius * _radius; }

    // non-const : modifie l'objet
    void setRadius(double r) { _radius = r; }
};

const Circle c(5.0);
c.area();       // OK — fonction const
c.setRadius(3); // ERREUR — fonction non-const sur un objet const
```

**Règle :** Tout getter ou fonction qui ne modifie pas l'objet doit être `const`.

---

## 11. Membres `static`

Un membre `static` appartient à la **class**, et non à une instance particulière.

```cpp
class PhoneBook {
private:
    static int _count; // partagé par tous les objets PhoneBook

public:
    PhoneBook()  { _count++; }
    ~PhoneBook() { _count--; }

    static int getCount() { return _count; }
};

int PhoneBook::_count = 0; // doit être défini dans le .cpp

PhoneBook a, b, c;
std::cout << PhoneBook::getCount(); // 3
```

---

## 12. Header guards — identique au C mais plus strict

```cpp
// Contact.hpp
#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>  // incluez ce que vous utilisez

class Contact {
    // ...
};

#endif
```

**Important :** Incluez `<string>` dans le header si `std::string` y est utilisé. Ne comptez pas sur le fait qu'il soit inclus de manière transitive.

---

## 13. Compilation et Makefile 42

Les fichiers C++ utilisent l'extension `.cpp`. Le compiler est `c++` ou `g++` :

```makefile
NAME    = phonebook
CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS    = phonebook.cpp Contact.cpp PhoneBook.cpp
OBJS    = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
```

Le flag `-std=c++98` est exigé par 42. Il vous restreint aux fonctionnalités de C++98 (pas de `auto`, pas de range-based for, pas de lambdas, pas de smart pointers).

---

## 14. Sur quoi se concentre chaque module CPP

| Module | Sujets |
|--------|--------|
| **CPP00** | Namespaces, classes, member functions, `iostream`, `std::string`, `const`, `static` |
| **CPP01** | `new`/`delete`, pointers sur membres, references, `switch` |
| **CPP02** | Operator overload, Forme Canonique Orthodoxe, nombres à virgule fixe |
| **CPP03** | Héritage, classes de base / dérivées, chaînage de constructors |
| **CPP04** | Fonctions virtuelles, pure virtual (classes abstraites), interfaces |
| **CPP05** | Exceptions : `try`/`catch`/`throw` |
| **CPP06** | Casts : `static_cast`, `dynamic_cast`, `reinterpret_cast`, `const_cast` |
| **CPP07** | Templates (templates de fonction et de class) |
| **CPP08** | Containers et iterators de la STL |
| **CPP09** | Algorithmes de la STL, containers supplémentaires |

---

## 15. Conseils pratiques pour les modules CPP de 42

### Séparez toujours en `.hpp` et `.cpp`

N'écrivez jamais le corps des fonctions dans le header (sauf pour de courts getters ou des templates).

```
PhoneBook.hpp  — class declaration
PhoneBook.cpp  — method definitions
Contact.hpp    — class declaration
Contact.cpp    — method definitions
main.cpp       — main()
```

### Écrivez l'OCF en premier, puis complétez la logique

Chaque fois que vous créez une nouvelle class :
1. Écrivez les quatre fonctions de l'OCF sous forme de coquilles vides
2. Faites compiler le code
3. Ajoutez ensuite les fonctionnalités réelles

Cela évite le bug du « j'ai oublié le copy constructor » à la fin.

### Lisez l'erreur depuis le début

```
error: no matching function for call to 'PhoneBook::PhoneBook()'
```
Cela signifie que vous avez défini un constructor non par défaut et que C++ recherche `PhoneBook()` sans argument. Ajoutez-en un ou corrigez le code qui instancie un PhoneBook sans argument.

### `this` — pointer vers l'objet courant

À l'intérieur de toute member function non-static, `this` est un pointer vers l'objet sur lequel elle est appelée :

```cpp
Contact& Contact::operator=(const Contact& other) {
    if (this != &other)   // garde contre l'auto-assignation : est-ce que j'essaie de m'assigner à moi-même ?
        _name = other._name;
    return *this;         // retourne une reference sur moi-même
}
```

### Séparez les responsabilités : getters/setters

Données privées, accesseurs publics :

```cpp
// Header
std::string getFirstName() const;       // getter
void        setFirstName(const std::string& name); // setter

// Implémentation
std::string Contact::getFirstName() const { return _firstName; }
void Contact::setFirstName(const std::string& name) { _firstName = name; }
```

### Quand quelque chose ne compile pas

1. Lisez uniquement la **première** erreur — les erreurs suivantes sont souvent des conséquences en cascade de la première.
2. Vérifiez : `#include` manquant, `;` manquant après l'accolade `}` d'une class, incompatibilité de type.
3. La `const` correctness est la source d'erreurs la plus fréquente : si vous appelez une méthode non-const sur un objet `const`, ou si vous oubliez un `const` sur un getter.

### Mémoire : un `new` = un `delete`

Suivez chaque `new` manuellement dans votre destructor. Si une class alloue avec `new`, elle doit libérer avec `delete` dans `~MyClass()`. Si elle alloue un tableau avec `new[]`, elle doit utiliser `delete[]`.

```cpp
// MAUVAIS
MyClass::~MyClass() { delete _arr; }    // _arr était new int[10] — UB

// BON
MyClass::~MyClass() { delete[] _arr; }
```

---

## 16. L'instruction `switch`

`switch` compare un entier (ou un `char`) à une liste de valeurs constantes. Tout comme en C — rien ne change en C++.

```cpp
int cmd = 2;

switch (cmd) {
    case 1:
        std::cout << "one\n";
        break;
    case 2:
        std::cout << "two\n";
        break;
    case 3:
        std::cout << "three\n";
        break;
    default:
        std::cout << "unknown\n";
        break;
}
// affiche : two
```

### `break` est obligatoire — sinon il y a fallthrough

Sans `break`, l'exécution continue en cascade (**falls through**) dans le case suivant :

```cpp
switch (cmd) {
    case 1:
        std::cout << "one\n";   // pas de break !
    case 2:
        std::cout << "two\n";   // s'exécute même si cmd == 1
        break;
    case 3:
        std::cout << "three\n";
        break;
}
// si cmd == 1 : affiche "one" puis "two"
// si cmd == 2 : affiche "two"
```

Le fallthrough est parfois intentionnel (pour regrouper des cas), mais écrivez-le toujours explicitement :

```cpp
switch (cmd) {
    case 1:
    case 2:
        std::cout << "one or two\n"; // les deux cas effectuent la même chose
        break;
    case 3:
        std::cout << "three\n";
        break;
}
```

### `switch` fonctionne uniquement avec les entiers et les chars

```cpp
switch (42)    { ... }  // int — ok
switch ('A')   { ... }  // char — ok
switch (3.14)  { ... }  // double — ERREUR
switch ("ADD") { ... }  // std::string — ERREUR
```

Pour `std::string` (comme les commandes de votre annuaire), vous devez utiliser `if`/`else if` :
```cpp
std::string input;
std::cin >> input;

if (input == "ADD")
    addContact();
else if (input == "SEARCH")
    searchContact();
else if (input == "EXIT")
    return 0;
```

### `default` est la solution de repli

S'exécute lorsqu'aucun `case` ne correspond. Ajoutez-le systématiquement — c'est le `else` d'un `switch`.

---

## 17. Common mistakes coming from C

| Habitude en C | Équivalent en C++ |
|---|---|
| `char *str = malloc(...)` | `std::string str` ou `new char[n]` + `delete[]` |
| `printf("%d", n)` | `std::cout << n` |
| `strcmp(a, b) == 0` | `a == b` (comparaison de std::string) |
| `struct` with function pointers | `class` avec fonctions virtuelles |
| `NULL` | `NULL` fonctionne toujours, préférez `0` en C++98 ou `nullptr` en C++11 |
| `typedef struct {...} t_name` | Simplement `class Name` ou `struct Name` |
| `void *` for generics | Templates |
| `#include <string.h>` | `#include <cstring>` (versions C++ des headers C) |

---

## 17. Quick reference card

```cpp
// Squelette de class (à copier-coller pour chaque nouvelle class)
#ifndef MYCLASS_HPP
# define MYCLASS_HPP

class MyClass {
private:
    int _value;

public:
    MyClass();                               // default constructor
    MyClass(const MyClass& other);           // copy constructor
    MyClass& operator=(const MyClass& other); // copy assignment
    ~MyClass();                              // destructor

    int  getValue() const;
    void setValue(int value);
};

#endif
```

```cpp
// Squelette du .cpp
#include "MyClass.hpp"
#include <iostream>

MyClass::MyClass() : _value(0) {
    std::cout << "MyClass default constructor called\n";
}

MyClass::MyClass(const MyClass& other) : _value(other._value) {
    std::cout << "MyClass copy constructor called\n";
}

MyClass& MyClass::operator=(const MyClass& other) {
    std::cout << "MyClass copy assignment operator called\n";
    if (this != &other)
        _value = other._value;
    return *this;
}

MyClass::~MyClass() {
    std::cout << "MyClass destructor called\n";
}

int  MyClass::getValue() const  { return _value; }
void MyClass::setValue(int v)   { _value = v; }
```

Les appels à `std::cout` dans le constructor/destructor sont une pratique standard à 42 pour afficher le cycle de vie de l'objet pendant l'évaluation.