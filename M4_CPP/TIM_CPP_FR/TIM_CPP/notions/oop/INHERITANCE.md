# L'héritage en C++ — Guide complet

> **Zoom sur les mots-clés :** [`PUBLIC/PRIVATE/PROTECTED`](../../lexique/PUBLIC_PRIVATE_PROTECTED.md) · [`VIRTUAL`](../../lexique/VIRTUAL.md) · [`USING`](../../lexique/USING.md)

L'héritage vous permet d'exprimer une relation **« est-un »** (is-a) entre des types. Un `Dog` est un `Animal`. Un `ScavTrap` est un `ClapTrap`. La classe dérivée obtient automatiquement les membres de la base — avec en plus la capacité d'ajouter les siens ou de les override.

```cpp
class Animal {
public:
    void breathe() const { std::cout << "..." << '\n'; }
};

class Dog : public Animal {
public:
    void bark() const { std::cout << "Woof!" << '\n'; }
};

Dog d;
d.breathe();   // hérité d'Animal
d.bark();      // propre à Dog
```

---

## Table des matières

1. [Pourquoi l'héritage existe](#1.%20Why%20inheritance%20exists)
2. [Syntaxe et les trois modes d'accès](#2.%20Syntax%20and%20the%20three%20access%20modes)
3. [`protected` — le juste milieu](#3.%20protected%20%E2%80%94%20the%20middle%20ground)
4. [Ordre de construction et de destruction](#4.%20Construction%20and%20destruction%20order)
5. [Appel du constructeur parent](#5.%20Calling%20the%20parent%27s%20constructor)
6. [Disposition en mémoire](#6.%20Memory%20layout)
7. [Upcasting et downcasting](#7.%20Upcasting%20and%20downcasting)
8. [Masquage de nom (name hiding)](#8.%20Name%20hiding)
9. [Héritage multiple](#9.%20Multiple%20inheritance)
10. [Le problème du diamant et l'héritage virtuel](#10.%20The%20diamond%20problem%20and%20virtual%20inheritance)
11. [Composition vs héritage](#11.%20Composition%20vs%20inheritance)
12. [Dans CPP03 — la famille ClapTrap](#12.%20In%20CPP03%20%E2%80%94%20ClapTrap%20family)
13. [Dans CPP04 — hiérarchies polymorphiques](#13.%20In%20CPP04%20%E2%80%94%20polymorphic%20hierarchies)
14. [Règles empiriques](#14.%20Rules%20of%20thumb)
15. [Pièges courants](#15.%20Gotchas)

---

## 1. Pourquoi l'héritage existe

Deux raisons :

1. **Réutilisation de code.** `Dog` ne réimplémente pas `breathe()` — il en hérite.
2. **Polymorphisme.** Un `Dog*` peut être traité comme un `Animal*` partout où le code attend un `Animal`. C'est ce qui vous permet d'écrire du code manipulant « n'importe quel type d'animal » sans connaître spécifiquement les chiens ou les chats. Voir [POLYMORPHISM.md](POLYMORPHISM.md).

La première raison est utile mais *insuffisante* à elle seule (la composition est souvent plus propre — voir §11). La seconde constitue sa véritable puissance.

---

## 2. Syntaxe et les trois modes d'accès

```cpp
class Base { /* ... */ };

class Derived : public    Base { /* ... */ };   // is-a  — 99 % du temps
class Derived : protected Base { /* ... */ };   // rare
class Derived : private   Base { /* ... */ };   // "implémenté-en-termes-de"
```

Le spécificateur d'accès sur la ligne d'héritage contrôle **ce que le monde extérieur peut voir** à travers la classe dérivée.

| Type d'héritage | `public` dans Base devient… | `protected` dans Base devient… | `private` dans Base devient… |
|---|---|---|---|
| `public`    | `public`    | `protected` | non accessible |
| `protected` | `protected` | `protected` | non accessible |
| `private`   | `private`   | `private`   | non accessible |

**Les membres `private` de la base ne sont jamais accessibles à la classe dérivée**, quel que soit le type d'héritage. Seuls les membres `protected` et `public` sont transmis.

**À 42**, vous utiliserez presque exclusivement l'héritage `public`. Les deux autres existent mais justifient rarement leur utilisation.

---

## 3. `protected` — le juste milieu

- **`private`** — uniquement la class elle-même.
- **`public`** — tout le monde.
- **`protected`** — la class elle-même **et toute classe dérivée**.

```cpp
class Animal {
protected:
    std::string _type;     // les classes dérivées peuvent y toucher
};

class Dog : public Animal {
public:
    Dog() { _type = "Dog"; }   // OK — _type est protected
};

Dog d;
d._type = "Cat";               // ERREUR — en dehors de la classe, _type est masqué
```

`protected` est utile mais permissif : n'importe quelle sous-classe peut y accéder et altérer un état que la classe de base essayait de protéger. Privilégiez `private` + des accesseurs protected à moins que le membre ne soit *conçu* pour être partagé avec les sous-classes.

---

## 4. Ordre de construction et de destruction

**Le parent d'abord à la montée, l'enfant d'abord à la descente.** Toujours.

```
Dog d;  // construction

  1. Animal() s'exécute       — le sous-objet de base est construit
  2. Dog()    s'exécute       — puis le propre code de Dog

~Dog();  // destruction (qu'elle soit explicite ou automatique)

  1. ~Dog()    s'exécute      — le propre code de Dog d'abord
  2. ~Animal() s'exécute      — puis le destructeur de base
```

### Pourquoi cet ordre ?

Parce que le constructeur de `Dog` peut dépendre du fait que la partie `Animal` soit prête. (Vous ne pouvez pas utiliser des champs qui ne sont pas encore initialisés.) Et le destructeur d'`Animal` peut compter sur le fait que l'état spécifique à `Dog` ait déjà été nettoyé.

### Chaîne à plusieurs niveaux

```cpp
class A { public: A() { std::cout << "A\n"; } ~A() { std::cout << "~A\n"; } };
class B : public A { public: B() { std::cout << "B\n"; } ~B() { std::cout << "~B\n"; } };
class C : public B { public: C() { std::cout << "C\n"; } ~C() { std::cout << "~C\n"; } };

C obj;
// Affiche :  A  B  C                 — construction (du haut vers le bas)
// Puis :     ~C  ~B  ~A              — destruction (du bas vers le haut)
```

---

## 5. Appel du constructeur parent

Le constructeur par défaut de la base s'exécute automatiquement. Pour passer des arguments à la base, utilisez la liste d'initialisation :

```cpp
class Animal {
public:
    Animal(const std::string& t) : _type(t) {}
protected:
    std::string _type;
};

class Dog : public Animal {
public:
    Dog() : Animal("Dog") {}           // construction explicite de la base
    Dog(const std::string& name)
        : Animal("Dog"), _name(name) {}
private:
    std::string _name;
};
```

**Si la base n'a pas de constructeur par défaut, la liste d'initialisation de la classe dérivée DOIT la construire explicitement.** L'oublier provoque une erreur de compilation indiquant « no matching function for call to `Animal::Animal()` ».

### Constructeur de recopie et `operator=`

Ceux-ci ne sont **pas** hérités tels quels — le compiler les génère pour la classe dérivée, mais si vous écrivez les vôtres, vous devez explicitement faire suivre l'appel à la base :

```cpp
Dog::Dog(const Dog& other) : Animal(other) { /* copier les champs spécifiques à Dog */ }

Dog& Dog::operator=(const Dog& other) {
    if (this != &other) {
        Animal::operator=(other);        // appel de l'operator= de la base
        _name = other._name;
    }
    return *this;
}
```

Oublier de transmettre l'appel à la base signifie que le sous-objet de base ne sera pas copié — un bug classique.

---

## 6. Disposition en mémoire

```
Dog d;

Mémoire :
┌──────────────────────────────┐  ◄── commence ici
│  Sous-objet Animal :         │
│    _type (std::string)       │
│    [vptr si virtuel]         │
├──────────────────────────────┤
│  Membres propres à Dog :     │
│    _name (std::string)       │
└──────────────────────────────┘
```

Propriété clé : un `Dog*` et un `Animal*` pointent tous les deux vers la **même adresse** (le sous-objet de base se trouve à l'offset 0). C'est pourquoi l'upcasting est gratuit — aucun ajustement de pointer n'est requis.

(L'héritage multiple casse légèrement cette règle ; la seconde base réside à un offset non nul, et le cast doit décaler le pointer.)

---

## 7. Upcasting et downcasting

### Upcasting — implicite et sûr

De la dérivée vers la base. Fonctionne toujours, aucun cast nécessaire :

```cpp
Dog     d;
Animal* a = &d;       // upcast implicite
a->breathe();         // OK
```

C'est la pierre angulaire du polymorphisme : une fonction qui prend un `Animal*` fonctionne pour n'importe quelle sous-classe.

### Downcasting — nécessite un cast, peut échouer

De la base vers la dérivée. Le compiler ne peut pas vérifier le type réel, vous devez donc être explicite :

```cpp
Animal* a = ...;
Dog*    d = static_cast<Dog*>(a);        // vous promettez qu'il s'agit bien d'un Dog — aucune vérification
Dog*    d = dynamic_cast<Dog*>(a);       // vérifié à l'exécution — NULL s'il ne s'agit pas d'un Dog
```

Utilisez `dynamic_cast` dès lors que vous ne pouvez pas prouver à la compilation que `a` est un `Dog`. Voir [CASTS.md](../../lexique/CASTS.md).

### Troncature d'objet (object slicing) — le bug silencieux

Effectuer un upcast par **valeur** (et non par pointer/reference) **tronque** la partie dérivée :

```cpp
Dog    d;
Animal a = d;         // copie UNIQUEMENT le sous-objet Animal ; les champs spécifiques à Dog sont perdus
```

`a` est désormais un simple `Animal` — sans rien de spécifique à un Dog. C'est légal et silencieux. Passez toujours les hiérarchies polymorphiques **par pointer ou reference**, jamais par valeur.

---

## 8. Masquage de nom (name hiding)

Si une classe dérivée déclare un nom qui existe également dans la base, le nom entier de la base est **complètement** masqué — même les overloads qui prennent des arguments différents :

```cpp
class Base {
public:
    void f(int);
    void f(double);
};

class Derived : public Base {
public:
    void f(int);        // masque À LA FOIS Base::f(int) ET Base::f(double)
};

Derived d;
d.f(3.14);              // ERREUR — Base::f(double) est masqué
d.Base::f(3.14);        // OK — explicite
```

Solution : réintroduire le nom à l'aide de `using` :
```cpp
class Derived : public Base {
public:
    using Base::f;      // rend visibles tous les overloads de Base::f
    void f(int);        // ajoutez le vôtre
};
```

---

## 9. Héritage multiple

Une classe peut hériter de plus d'une base :

```cpp
class Flier    { public: virtual void fly() = 0;  };
class Swimmer  { public: virtual void swim() = 0; };

class Duck : public Flier, public Swimmer {
public:
    void fly()  { /* ... */ }
    void swim() { /* ... */ }
};
```
Disposition en mémoire :

```
Duck d;
┌──────────────┐  ◄── &d as Duck* or Flier*
│ Flier part   │
├──────────────┤  ◄── &d as Swimmer*     (non-zero offset)
│ Swimmer part │
├──────────────┤
│ Duck part    │
└──────────────┘
```

L'upcasting vers `Swimmer*` **ajuste le pointer** — il pointe désormais au milieu de l'objet. `static_cast` et `dynamic_cast` gèrent cela pour vous.

---

## 10. Le problème du diamant et l'héritage virtuel

```
     A
    / \
   B   C
    \ /
     D
```

Si `B` et `C` héritent tous deux de `A`, et que `D` hérite des deux, `D` possède **deux copies de `A`**. Accéder à n'importe quel membre de `A` depuis `D` est ambigu.

```cpp
class A { public: int n; };
class B : public A {};
class C : public A {};
class D : public B, public C {};

D d;
d.n = 5;          // ERREUR — ambigu : B::A::n ou C::A::n ?
d.B::n = 5;       // fonctionne, mais laid
```

### Solution : l'héritage virtuel

Marquez l'héritage de `B` et `C` depuis `A` comme `virtual` :

```cpp
class A { public: int n; };
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};

D d;
d.n = 5;          // OK — un seul sous-objet A partagé
```

Désormais, `D` contient **un** `A` partagé. À 42, CPP04 ex03 (« Interesting idea ») est un aperçu contrôlé de l'héritage virtuel. En dehors de cela, vous l'écrirez rarement.

---

## 11. Composition vs héritage

L'héritage n'est pas le mécanisme de réutilisation par défaut — c'est le mécanisme pour le polymorphisme. Pour « la class X utilise la class Y », utilisez généralement la **composition** :

```cpp
// Héritage (is-a) — pour le polymorphisme
class Dog : public Animal { /* ... */ };

// Composition (has-a) — pour la réutilisation
class Car {
    Engine _engine;             // Car A un Engine
    Wheel  _wheels[4];
public:
    void start() { _engine.ignite(); }
};
```

### Lequel choisir et quand

| Question | Héritage | Composition |
|---|---|---|
| X est-il un Y ? | ✅ | ❌ |
| X possède-t-il un Y ? | ❌ | ✅ |
| Les appelants détiendront-ils un `Y*` et voudront-ils un dispatch virtuel ? | ✅ | ❌ |
| Voulez-vous simplement réutiliser les méthodes de Y ? | ❌ | ✅ |
| Y peut-il changer au cours de la vie de l'objet ? | ❌ | ✅ (remplacer le membre) |

La composition est plus souple, plus facile à tester, et ne fait pas fuiter les détails internes de la base dans la class dérivée. N'utilisez l'héritage que lorsque vous avez besoin d'une sémantique « is-a » et de polymorphisme.

---

## 12. Dans CPP03 — la famille ClapTrap

```
ClapTrap
   │
   ├── ScavTrap  (virtual inheritance of ClapTrap in ex03)
   │
   └── FragTrap  (virtual inheritance of ClapTrap in ex03)
         │
       DiamondTrap : ScavTrap, FragTrap   ← the diamond
```

- **ex00** — class simple, sans héritage. Construit `ClapTrap`.
- **ex01** — héritage simple. `ScavTrap : public ClapTrap`. Leçon clé : chaîne de construction et de destruction, appel du constructor de base avec le bon argument.
- **ex02** — `FragTrap : public ClapTrap`. Idem, une autre classe sœur.
- **ex03** — `DiamondTrap : public ScavTrap, public FragTrap`. C'est ici que vous rencontrez le problème du diamant et avez besoin de l'**héritage virtuel** sur les lignes d'héritage de `ScavTrap` et `FragTrap`. La sortie attendue trace précisément l'ordre de construction.

Attention à la règle subtile : dans l'héritage virtuel, la **class la plus dérivée** (`DiamondTrap`) est responsable de l'initialisation de la base virtuelle (`ClapTrap`). Les constructors intermédiaires (`ScavTrap`, `FragTrap`) ne peuvent pas le faire.

---

## 13. Dans CPP04 — hiérarchies polymorphiques

- **ex00** — base `Animal` avec les sous-classes `Dog`, `Cat`. Focus : destructor virtuel, `makeSound` virtuel.
- **ex01** — ajout de `Brain` en tant que membre **composé** (non hérité !). Rigueur de la deep-copy (`Dog` possède un `Brain*` ; le ctor de copie et l'`operator=` doivent cloner).
- **ex02** — rendre `Animal` **abstraite** (`makeSound` pure virtual). Impossible d'instancier `Animal` directement — seulement `Dog` et `Cat`.
- **ex03** — l'exercice d'interfaces `AMateria` / `Character` / `MateriaSource`. Interfaces = classes abstraites contenant uniquement des méthodes pure-virtual.

C'est ici que l'héritage prend tout son sens : vous écrivez du code qui manipule des `Animal*` et les bonnes méthodes s'exécutent automatiquement pour `Dog` ou `Cat`. C'est le polymorphisme. Voir [POLYMORPHISM.md](POLYMORPHISM.md).

---

## 14. Règles empiriques

- **Privilégiez l'héritage `public` par défaut.** L'héritage `private` et `protected` relève de cas particuliers.
- **Toute class de base polymorphique a un destructor `virtual`.** Sans lui, `delete base_ptr` ignore le destructor dérivé → fuites.
- **Passez les objets polymorphiques par pointer ou reference**, jamais par valeur. (Évite le slicing.)
- **Préférez la composition** à moins d'avoir spécifiquement besoin de polymorphisme.
- **Transmettez à la base dans le ctor de copie et l'`operator=`** lorsque vous les écrivez.
- **Héritage multiple uniquement pour les interfaces.** Hériter de plusieurs classes concrètes est un nid à bugs.
- **N'utilisez pas de membres de données `protected`** à moins que la class ne soit spécifiquement conçue pour être étendue. Utilisez des méthodes `protected` à la place.

---

## 15. Pièges courants

- **Pas de destructor virtuel sur une base polymorphique** → `delete ptr` n'appelle que le destructor de base → fuite des ressources dérivées. Le barème 42 vérifie cela.
- **Object slicing** lors du passage par valeur. `void f(Animal a)` + `f(dog)` tronque silencieusement la partie Dog.
- **Oublier de transmettre à la base** dans le ctor de copie / `operator=` laisse le sous-objet de base intact.
- **Masquage de nom (name hiding)** — un seul `f(int)` dans la dérivée masque tous les overloads de `f` dans la base.
- **L'héritage en diamant sans `virtual`** produit deux copies du grand-parent et un accès ambigu aux membres.
- **Appeler une fonction virtuelle depuis un constructor ou un destructor** ne dispatche PAS vers la dérivée — pendant ceux-ci, l'objet n'est pas encore totalement un `Derived` (ou ne l'est plus). Vous obtenez la version de la base.
- **L'héritage privé n'est pas un « is-a »** — c'est un « implémenté en termes de » (implemented-in-terms-of). Le code extérieur ne peut pas upcaster `Derived*` en `Base*`.
- **Les default constructors hérités sont générés implicitement**, mais pas les constructeurs avec arguments. Vous devez écrire chaque overload explicitement dans la class dérivée.

---

## 16. Vision matérielle — ce que fait réellement le CPU

### Héritage simple — sous-objet de base à l'offset 0

```
class Animal { std::string type; };            // 32 bytes
class Dog : public Animal { Brain* brain; };   // 32 + 8 = 40 bytes

Dog layout:
┌──────────────────────────┐ offset 0
│  Animal::type            │   (std::string, 32 bytes)
├──────────────────────────┤ offset 32
│  Dog::brain              │   (Brain*, 8 bytes)
└──────────────────────────┘ total 40 bytes
```

`Animal* a = &dog;` se compile en zéro instruction machine — le sous-objet de base commence à l'offset 0, donc le `Animal*` et le `Dog*` pointent vers la même adresse. L'upcast est gratuit.

### Avec une méthode virtuelle — le vptr prend la tête

```
class Animal { virtual void makeSound(); std::string type; };
class Dog : public Animal { Brain* brain; };

Dog layout:
┌──────────────────────────┐ offset 0
│  vptr → Dog's vtable     │   (8 bytes)
├──────────────────────────┤ offset 8
│  Animal::type            │   (32 bytes)
├──────────────────────────┤ offset 40
│  Dog::brain              │   (8 bytes)
└──────────────────────────┘ total 48 bytes
```

Le vptr est désormais le premier membre. Malgré tout, `Dog*` et `Animal*` pointent vers la même adresse.

### Héritage multiple — seconde base à un offset non nul

```
class A { int a_data; };       // 4 bytes
class B { int b_data; };       // 4 bytes
class C : public A, public B {};

C layout:
┌──────────────────────────┐ offset 0
│  A::a_data               │   (4 bytes)
├──────────────────────────┤ offset 4
│  B::b_data               │   (4 bytes)
└──────────────────────────┘
```

`A* p_a = &c;` → même adresse que `&c`, aucun ajustement.
`B* p_b = &c;` → l'adresse est `&c + 4`, une instruction `lea`.

### Héritage virtuel — le coût

Le diamant `class D : virtual public A` ajoute un **virtual base table pointer** (vbptr) à chaque class qui hérite virtuellement :

```
class A { int a_data; };
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};

D layout (conceptually):
┌──────────────────────────┐
│  B's vbptr               │   (offset to A's subobject)
├──────────────────────────┤
│  B members               │
├──────────────────────────┤
│  C's vbptr               │
├──────────────────────────┤
│  C members               │
├──────────────────────────┤
│  D members               │
├──────────────────────────┤
│  A::a_data (shared)      │   ← one copy
└──────────────────────────┘
```

Accéder à `A::a_data` depuis l'intérieur de `B` ou `C` nécessite de lire d'abord le vbptr : *« où est le sous-objet A partagé ? »*. Une indirection supplémentaire par rapport à l'héritage non virtuel. Le coût est minime (généralement un chargement L1), mais c'est la raison pour laquelle le standard ne fait pas de `virtual` le comportement par défaut.

### Ordre de construction en assembleur

Pour `Dog d("Rex");` :

```asm
; 1. réserver l'espace de stockage (stack)
sub  rsp, 48

; 2. construire le sous-objet Animal
lea  rdi, [rsp]                  ; this = &d (vue Animal)
mov  rsi, "Rex"
call Animal::Animal

; 3. assigner le vptr à la vtable de Dog (généré automatiquement par le compiler)
lea  rax, [rip + vtable_for_Dog + 16]
mov  [rsp], rax

; 4. construire les membres spécifiques à Dog
lea  rdi, [rsp + 40]             ; &d.brain
; ... new Brain(), stocker le pointer ...

; 5. exécuter le corps du ctor de Dog (ex. afficher une trace)
; ...
```
L'étape 3 explique pourquoi les virtual calls pendant `Animal::Animal` sont dispatchés vers `Animal` — le vptr n'a pas encore basculé vers celui de `Dog`. Entre l'étape 2 et l'étape 3, le « type » de l'objet change au runtime.

### Padding et alignment

Le compiler ajoute du padding aux membres pour satisfaire les contraintes d'alignment.
`Dog { char c; Brain* brain; }` fait **16 bytes**, pas 9 : le
`Brain*` requiert un alignment de 8 bytes, donc le compiler insère 7
bytes de padding après `c`.

Règle empirique : **ordonnez les membres du plus grand au plus petit** pour minimiser
le padding. Cela n'a pas d'importance pour la validité du code, mais en a
pour le cache packing dans le hot code.