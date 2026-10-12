# Pointers to Member Functions

> **Connexe :** [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) · [`MEMORY.md`](../fundamentals/MEMORY.md) · [`REFERENCE.md`](../fundamentals/REFERENCE.md)

Un **pointer to a member function** (pointeur vers une fonction membre) stocke *quelle méthode d'une class* appeler, plutôt que quelle fonction libre. La syntaxe est hostile, mais l'idée est simple : c'est un function pointer qui retient également la class à laquelle il appartient, afin que l'appelant puisse fournir l'instance `this` au moment de l'appel.

```cpp
void           (*fp)();           // pointer to a FREE function
void (Harl::*  mp)();             // pointer to a MEMBER function of Harl
```

La raison pour laquelle il s'agit de types distincts : appeler une méthode nécessite **deux choses** — la fonction *et* l'objet (`this`). Un function pointer classique ne transporte que la première ; un member function pointer transporte les informations de type nécessaires pour lier le second au moment de l'appel.

---

## Table of Contents

1. [Why two different pointer types?](#1.%20Why%20two%20different%20pointer%20types%3F)
2. [Declaring a member function pointer](#2.%20Declaring%20a%20member%20function%20pointer)
3. [Taking the address](#3.%20Taking%20the%20address)
4. [Calling through the pointer](#4.%20Calling%20through%20the%20pointer)
5. [The classic use case — dispatch tables](#5.%20The%20classic%20use%20case%20%E2%80%94%20dispatch%20tables)
6. [Virtual methods and member pointers](#6.%20Virtual%20methods%20and%20member%20pointers)
7. [`static` member functions are not members](#7.%20static%20member%20functions%20are%20not%20members)
8. [`typedef` to tame the syntax](#8.%20typedef%20to%20tame%20the%20syntax)
9. [Common errors](#9.%20Common%20errors)
10. [In CPP01](#10.%20In%20CPP01)

---

## 1. Why two different pointer types?

Une fonction libre réside à une seule adresse. Un appel de méthode, en revanche, se développe grosso modo en :

```cpp
harl.debug();          // is conceptually: Harl_debug(&harl);
```

Le premier paramètre caché `this` est lié par l'*appelant* (l'opérateur point ou flèche), et n'est pas stocké à l'intérieur du pointer. Un member function pointer doit donc :

- Savoir à **quelle class** il appartient (pour que le compiler sache quel type donner à `this`).
- Éventuellement transporter des **informations de vtable** pour les méthodes virtuelles (afin que le dispatch virtuel fonctionne toujours à travers le pointer).

Cela signifie qu'un member function pointer n'a **pas la même taille** qu'un function pointer standard, et qu'il n'est **pas convertible** vers celui-ci. Ce sont des types distincts.

---

## 2. Declaring a member function pointer

Lisez-le de gauche à droite, les parenthèses regroupant la partie `ClassName::*` :

```cpp
return_type (ClassName::*pointer_name)(arg_types);
```

Exemples :

```cpp
class Harl {
    public:
        void        debug();
        void        info();
        int         level(std::string s) const;
};

void (Harl::*action)();                          // method, no args, returns void
int  (Harl::*query)(std::string) const;          // const method, takes string, returns int
```

Le `const` à la fin fait partie du *type de la fonction* — un pointer déclaré sans `const` ne peut pas pointer vers une méthode qualifiée de `const`, et inversement.

---

## 3. Taking the address

Utilisez le **nom qualifié** avec `&` :

```cpp
void (Harl::*action)() = &Harl::debug;           // correct
```

Vous **ne pouvez pas** écrire `&harl.debug` — cette forme est illégale en C++98 car le résultat ne serait pas un member pointer (ce serait ambigu avec le fait de lier `this` prématurément). Utilisez toujours :

```cpp
&ClassName::methodName
```

---

## 4. Calling through the pointer

Vous avez besoin d'une instance pour fournir `this`. C++ met à votre disposition deux opérateurs spéciaux :

| Opérateur | À utiliser lorsque vous avez... | Exemple |
|---|---|---|
| `.*` | un objet (ou une reference) | `(harl.*action)()` |
| `->*` | un pointer vers un objet | `(p->*action)()` |

```cpp
Harl  harl;
Harl *p = &harl;

(harl.*action)();        // dot-star
(p->*action)();          // arrow-star
```

**Les parenthèses extérieures autour de `(obj.*ptr)` sont obligatoires.** En termes de priorité, l'opérateur d'appel `()` lie plus fortement que `.*`. Sans elles :

```cpp
harl.*action()           // parsed as: harl .* ( action() )   ❌ won't compile
```

Moyen mnémotechnique : l'opérateur `.*` dit *"accède à cet objet et extrais la fonction désignée par le pointer"* ; puis `()` l'appelle.

---

## 5. The classic use case — dispatch tables

Les chaînes de if/else qui font correspondre une valeur à une méthode sont exactement ce que les member function pointers remplacent. Comparez :

**Sans member pointers** — flux de contrôle :

```cpp
void Harl::complain(std::string level) {
    if (level == "DEBUG")        debug();
    else if (level == "INFO")    info();
    else if (level == "WARNING") warning();
    else if (level == "ERROR")   error();
}
```

**Avec member pointers** — données :

```cpp
void Harl::complain(std::string level) {
    std::string  levels[4] = { "DEBUG", "INFO", "WARNING", "ERROR" };
    void (Harl::*fns[4])() = { &Harl::debug, &Harl::info,
                               &Harl::warning, &Harl::error };

    for (int i = 0; i < 4; ++i) {
        if (levels[i] == level) {
            (this->*fns[i])();
            return;
        }
    }
}
```

Pourquoi la seconde approche est préférable :

- **Ajouter un niveau = ajouter deux entrées de tableau**, au lieu de modifier des branches de code.
- La boucle `for` est **générique** — elle n'a pas besoin de savoir ce que font `debug`/`info`/`warning`/`error`.
- Les deux tableaux *décrivent* la correspondance ; le code qui l'*utilise* reste court et uniforme.

C'est le pattern vers lequel le sujet de 42 vous oriente dans **CPP01 ex05/ex06**.

---

## 6. Virtual methods and member pointers

Les member function pointers respectent le dispatch `virtual`. Si vous prenez l'adresse d'une méthode virtuelle, appeler la fonction à travers le pointer invoquera tout de même l'override le plus dérivé :

```cpp
class Animal {
    public:
        virtual void speak() { std::cout << "..." << std::endl; }
};

class Dog : public Animal {
    public:
        virtual void speak() { std::cout << "Woof" << std::endl; }
};

void (Animal::*sp)() = &Animal::speak;

Dog     d;
Animal &a = d;
(a.*sp)();                  // prints "Woof" — virtual dispatch still happens
```

Mécanisme : le member pointer ne stocke pas une adresse brute. Pour une méthode virtuelle, il stocke quelque chose comme *"slot N de la vtable"*, et l'appel se résout via la vtable réelle de `a`.

---

## 7. `static` member functions are not members

Une méthode `static` n'a **pas de `this`**. C'est une fonction libre qui réside dans le namespace de la class. Vous prenez donc son adresse avec un function pointer standard :

```cpp
class Foo {
    public:
        static void hello() { std::cout << "hi" << std::endl; }
};

void (*fp)() = &Foo::hello;        // plain function pointer
fp();                              // call directly — no instance needed
```

Tenter d'écrire `void (Foo::*fp)() = &Foo::hello;` est une erreur : incompatibilité de types.

---

## 8. `typedef` to tame the syntax

La lourdeur de la déclaration s'accumule vite. `typedef` vous permet de nommer le type une seule fois :

```cpp
typedef void (Harl::*HarlAction)();        // HarlAction is now a real type

HarlAction  table[4] = { &Harl::debug, &Harl::info,
                         &Harl::warning, &Harl::error };

HarlAction  pick = table[2];
(this->*pick)();
```

C'est la manière conventionnelle en C++98 (les alias `using` n'existant pas en C++98). Cela rend également les signatures de fonctions lisibles lorsque vous devez les passer en argument.

---

## 9. Common errors

| Erreur | Résultat | Solution |
|---|---|---|
| `&harl.debug` | erreur de compilation | `&Harl::debug` |
| `harl.*ptr()` | erreur de syntaxe (parse error) | `(harl.*ptr)()` |
| Mélanger des types qualifiés `const` | "cannot convert" | faire correspondre le qualificatif à la fois dans la déclaration et la cible |
| Assigner un member pointer à un simple `void(*)()` | incompatibilité de types | utiliser un type `(Class::*)` ou le définir via un `typedef` |
| Appeler sans instance | erreur de compilation | vous avez **toujours** besoin d'un objet — un appel membre n'existe pas sans `this` |

---

## 10. In CPP01

Vous aurez recours à ces concepts dans :

- **ex05 — Harl 2.0** : une dispatch table associant `"DEBUG" | "INFO" | "WARNING" | "ERROR"` à quatre méthodes privées.
- **ex06 — Harl filter** : la même dispatch table, complétée par un comportement de fall-through implémenté avec un `switch` sur l'index trouvé.

L'évaluateur vous demandera parfois de *justifier* l'utilisation de member pointers à la place de `if/else`. La réponse honnête :

> *Les member pointers transforment le flux de contrôle en données. Le dispatch se résume à une seule boucle au lieu de quatre branches, et ajouter un niveau consiste simplement à ajouter deux entrées de tableau — aucune modification de la logique de correspondance n'est requise.*

C'est la leçon que le sujet cherche à enseigner. La syntaxe un peu lourde n'est que le prix d'entrée.

---

## See also

- [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) — dispatch virtuel, vtables (pourquoi les member pointers peuvent encoder des slots de vtable).
- [`MEMORY.md`](../fundamentals/MEMORY.md) — comment `this` est transmis sous le capot.
- [`modules/CPP01.md`](../../projets/modules/CPP01.md) — guide pas à pas des exercices Harl.