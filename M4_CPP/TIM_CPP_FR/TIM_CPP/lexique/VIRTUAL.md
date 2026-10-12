# `virtual` — Le mot-clé qui fait fonctionner le polymorphisme

> **TL;DR.** `virtual` active le **dynamic dispatch** : la fonction appelée dépend du type d'exécution (runtime) de l'objet, et non du type statique de la variable. Le mécanisme repose sur la **vtable** — un tableau de function pointers par class, chaque objet polymorphique transportant un vptr caché.

Connexe : [`POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md) · [`INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`CASTS.md`](CASTS.md) · [`CPP04.md`](../projets/modules/CPP04.md)

---

## 1. Le problème résolu par `virtual`

```cpp
class Animal {
public:
    void sound() const { std::cout << "generic noise\n"; }
};

class Dog : public Animal {
public:
    void sound() const { std::cout << "woof\n"; }      // masque Animal::sound
};

void make(const Animal& a) { a.sound(); }              // qu'est-ce que cela appelle ?

Dog d;
make(d);            // affiche "generic noise" — INCORRECT
```

Sans `virtual`, l'appel `a.sound()` est dispatché **en fonction du type statique de `a`** — qui est `Animal&`. Le compiler émet un appel direct à `Animal::sound`. Même si l'objet réel est un `Dog`, l'override est ignoré.

Ajoutez un mot-clé :

```cpp
class Animal {
public:
    virtual void sound() const { std::cout << "generic noise\n"; }
};

Dog d;
make(d);            // affiche "woof" — CORRECT
```

Désormais, l'appel est dispatché **au runtime** en fonction du type dynamique de `*a`.

---

## 2. Le mécanisme — vtable + vptr

### 2.1 La vtable par class

Pour chaque class possédant au moins une virtual function, le compiler construit une **vtable** — un tableau de function pointers, un par virtual function :

```
   vtable d'Animal :
   ┌────────────────────────────────────┐
   │  [0] &Animal::sound                │
   │  [1] &Animal::~Animal              │
   └────────────────────────────────────┘

   vtable de Dog (hérite d'Animal) :
   ┌────────────────────────────────────┐
   │  [0] &Dog::sound          ← override
   │  [1] &Dog::~Dog           ← override
   └────────────────────────────────────┘
```

L'ordre des slots est le même dans la classe dérivée et la base — c'est ainsi que le dispatch peut être en O(1).

### 2.2 Le vptr par objet

Chaque objet d'une class polymorphique transporte un pointer caché vers la vtable de sa class. Il s'agit généralement des **8 premiers octets** (sur 64-bit) de l'objet :

```
   Animal a;                  Dog d;
   ┌──────────────────┐       ┌──────────────────────────┐
   │ vptr → Animal vt │       │ vptr → Dog vt            │
   ├──────────────────┤       ├──────────────────────────┤
   │ (Animal members) │       │ (Animal members)         │
   └──────────────────┘       │ (Dog members)            │
                              └──────────────────────────┘
```

```cpp
sizeof(Animal);        // 8 (vptr) + members
sizeof(Dog);           // 8 (vptr) + Animal members + Dog members
```

Une class sans aucun `virtual` n'a **aucun vptr** — `sizeof` ne reflète que les membres de données.

### 2.3 La séquence de dispatch

```cpp
Animal *p = new Dog();
p->sound();
```

Ce que le compiler émet, conceptuellement :

```asm
   mov rax, [p]            ; charger le pointer d'objet
   mov rbx, [rax]          ; charger le vptr (8 premiers octets de l'objet)
   mov rcx, [rbx + 0]      ; charger le slot 0 de la vtable (sound)
   call rcx
```

```
   p              objet        vtable           fonction
   ───            ──────       ──────           ────────
   ptr ──► [vptr | ...] ──► [sound, ~Dog] ──► Dog::sound  ── s'exécute
```

Trois chargements de pointer + un appel indirect. Dans les micro-benchmarks, cela prend quelques cycles de plus qu'un appel direct. Dans du code réel, la différence est généralement négligeable face au reste (cache misses, branch prediction, le travail réel effectué).

---

## 3. Le constructor initialise le vptr — voilà pourquoi les fonctions virtuelles ne dispatchent pas dans les ctors

Lorsque vous faites `new Dog()` :

```
   1. La mémoire est allouée.
   2. Le constructor d'Animal s'exécute. Le vptr pointe vers la vtable d'Animal.
      ← si vous appelez une virtual function maintenant, elle dispatche vers la version d'Animal
   3. Le constructor de Dog s'exécute. Le vptr est mis à jour vers la vtable de Dog.
      ← à partir de ce moment, les appels virtuels dispatchent vers les overrides de Dog
   4. La construction est terminée.
```

Ainsi, à l'intérieur du constructor d'`Animal`, `this->sound()` appelle `Animal::sound`, et **non** `Dog::sound`. Le Dog n'est pas encore construit — appeler sa méthode accéderait à un état non initialisé de Dog.

Il en va de même dans les destructors, dans l'ordre inverse : lorsque `~Dog` se termine et que `~Animal` commence, le vptr est réinitialisé vers celui d'`Animal` — les appels virtuels vont vers `Animal`.

Règle : **n'appelez jamais de virtuals depuis des constructors ou des destructors.**

---

## 4. Le pure virtual — abstract classes

```cpp
class Shape {
public:
    virtual double area() const = 0;     // pure virtual : aucune implémentation ici
    virtual ~Shape() {}                  // virtual dtor — important !
};
```

Le `= 0` fait de `Shape` une **abstract class**. Vous ne pouvez pas l'instancier directement :

```cpp
Shape s;             // ERROR — impossible d'allouer un objet de type abstrait
Shape *p = ...;      // OK — un pointer vers un type abstrait est valide
Shape& r = ...;      // OK
```

Les sous-classes doivent override la méthode pure virtual pour devenir concrètes :

```cpp
class Circle : public Shape {
    double _r;
public:
    Circle(double r) : _r(r) {}
    double area() const { return 3.14159 * _r * _r; }    // override
};

Circle c(5);          // OK — Circle est concrète
Shape *p = new Circle(5);
p->area();            // dispatché vers Circle::area
delete p;             // appelle Shape::~Shape virtuellement → Circle::~Circle en premier
```

Une méthode pure virtual *peut* avoir une implémentation :

```cpp
class Shape {
public:
    virtual double area() const = 0;
};
double Shape::area() const { return 0; }    // légal — mais reste pure virtual

class FlatShape : public Shape {
public:
    double area() const { return Shape::area(); }    // peut appeler l'implémentation de base
};
```

Rare mais utile pour un comportement par défaut partagé.

---

## 5. Virtual destructor — non négociable pour les bases polymorphiques

```cpp
class Base {
public:
    ~Base() {}        // PAS virtual
};

class Derived : public Base {
    int *_data;
public:
    Derived() : _data(new int[100]) {}
    ~Derived() { delete[] _data; }
};

Base *p = new Derived();
delete p;             // n'appelle que Base::~Base — Derived::~Derived est ignoré
                      // fuite de _data. UB au sens strict.
```

Rendez toujours les destructors virtuals dans toute class destinée à être héritée de manière polymorphique :

```cpp
class Base {
public:
    virtual ~Base() {}
};

delete p;             // dispatche via la vtable : ~Derived s'exécute, puis ~Base. OK.
```

Si une class possède au moins une virtual function, donnez-lui un virtual destructor. Le coût correspond au même vptr que vous avez déjà.

---

## 6. Les mots-clés `override` et `final` (C++11+, pas en C++98 de 42)

C++11 les a ajoutés par sécurité. Indisponibles dans le mode C++98 de 42, mais utiles à connaître pour l'avenir :

```cpp
class Dog : public Animal {
public:
    void sound() const override;          // le compiler vérifie que cela override réellement
    void sound() final;                   // aucun override supplémentaire autorisé dans les sous-classes
};
```

En C++98, vous ne bénéficiez pas de la détection de fautes de frappe. Soyez prudent avec les signatures (une différence de `const` crée un *nouveau* virtual au lieu de faire un override).

---

## 7. La décision de dispatch statique vs dynamique

```cpp
Animal a;
Dog d;
Animal *p = &d;
Animal &r = d;
Animal v = d;            // SLICED — ne copie que la partie Animal

a.sound();          // statique : Animal::sound (a est Animal)
d.sound();          // statique : Dog::sound (résolu à la compilation vers la dérivée)
p->sound();         // dynamique : Dog::sound (lookup dans la vtable)
r.sound();          // dynamique : Dog::sound (lookup dans la vtable)
v.sound();          // statique : Animal::sound (v est un Animal tronqué)
```

**Le polymorphisme ne fonctionne qu'à travers des pointers et des references.** Le passage par valeur d'un type de base tronque la partie dérivée — le vptr est écrasé avec le vptr de la base lors de la copie, et le dispatch se résout vers la base.

```
   Dog d;                 Animal v = d;
   ┌──────────────┐       ┌────────────────┐
   │ vptr → Dog   │       │ vptr → Animal  │  ← écrasé lors de la copie
   │ animalState  │       │ animalState    │
   │ dogState     │       └────────────────┘
   └──────────────┘       (dogState supprimé)
```

C'est le **slicing**. Presque toujours un bug. Passez les objets polymorphiques par `*` ou `&`.

---

## 8. Fonctions virtuelles multiples — agencement de la vtable

```cpp
class Shape {
public:
    virtual void draw() const;
    virtual double area() const;
    virtual void scale(double f);
    virtual ~Shape();
};

class Circle : public Shape { /* override les trois */ };
```

```
   vtable de Shape :                    vtable de Circle :
   ┌─────────────────────┐              ┌─────────────────────┐
   │ [0] Shape::draw     │              │ [0] Circle::draw    │
   │ [1] Shape::area     │              │ [1] Circle::area    │
   │ [2] Shape::scale    │              │ [2] Circle::scale   │
   │ [3] Shape::~Shape   │              │ [3] Circle::~Circle │
   └─────────────────────┘              └─────────────────────┘
```

L'ordre des slots est préservé à travers la hiérarchie — c'est ainsi qu'un `Shape*` peut effectuer un lookup en O(1) quelle que soit la class dérivée réelle.
---

## 9. Multiple inheritance et le diamant — les vtables se complexifient

Lorsqu'une class hérite de deux bases ayant chacune leur propre vtable, elle possède **plusieurs vptrs** — un par sous-objet de base. Le dispatch devient plus complexe (thunks, « this-adjustments »). Le problème du diamant (une class héritant indirectement deux fois de la même base) introduit l'héritage `virtual` — une utilisation différente du mot-clé. Voir [`INHERITANCE.md`](../notions/oop/INHERITANCE.md).

Pour les hiérarchies à héritage simple de 42, vous n'avez pas à vous en soucier. Mais sachez que « virtual » a une *seconde* signification lorsqu'il est appliqué à une class de base :

```cpp
class A {};
class B : virtual public A {};       // 'virtual' inheritance: A is shared in diamonds
class C : virtual public A {};
class D : public B, public C {};     // only ONE A in D, not two
```

Il s'agit d'une fonctionnalité différente des fonctions virtuelles, malgré le mot-clé partagé.

---

## 10. Astuces & conseils

### 10.1 Le mot-clé `virtual` n'est nécessaire qu'une seule fois — à la base

```cpp
class A {
public: virtual void f();
};
class B : public A {
public: void f();              // automatically virtual — override
};
class C : public B {
public: void f();              // automatically virtual — override
};
```

Vous pouvez répéter `virtual` sur les overrides par souci de clarté, et c'est sans danger. En C++11+, `override` rend l'intention évidente.

### 10.2 Ne rendez pas chaque fonction virtuelle

`virtual` a des coûts :
- La vtable + le vptr.
- Appel indirect (ne peut pas être inlined sauf si le compiler prouve le type dynamique).
- Cache miss lors du premier chargement de la vtable.

Rendez une fonction virtuelle lorsque les sous-classes sont *censées* l'overrider. Sinon, laissez-la concrète.

### 10.3 Appeler la version de base

```cpp
class Dog : public Animal {
public:
    void sound() const {
        Animal::sound();             // explicit base call
        std::cout << " — and woof!\n";
    }
};
```

Utile pour superposer des comportements (chaînes de constructors, enrichissement d'une implémentation de base).

### 10.4 Destructor virtuel pur — le cas inhabituel

```cpp
class Base {
public:
    virtual ~Base() = 0;          // pure virtual destructor
};
Base::~Base() {}                  // STILL needs a body!
```

Pourquoi un corps ? Parce que le destructor dérivé appellera le destructor de base, qui doit exister. `= 0` marque seulement Base comme abstraite ; le corps est toujours nécessaire.

C'est l'astuce standard pour rendre une class abstraite sans lui donner de fonctions membres abstraites.

### 10.5 Attention aux discordances de signature

```cpp
class Animal {
public:
    virtual void sound() const;
};

class Dog : public Animal {
public:
    void sound();                 // NOT an override — missing const!
};                                 // creates a new virtual function in Dog,
                                   // doesn't override Animal::sound
```

En C++11, utilisez `override` ; en C++98, soyez simplement prudent. Une faute de frappe dans une signature est un bug silencieux.

### 10.6 Inspecter une vtable

Vous ne pouvez pas le faire de manière *standard*, mais des outils peuvent vous aider :

```bash
$ g++ -fdump-class-hierarchy a.cpp
```

Cela génère le layout, y compris le contenu de la vtable. Utilisez-le pour voir ce que fait réellement votre compiler.

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| Slicing — méthodes dérivées non appelées | Passé/copié par valeur | Passer par reference ou pointer |
| Fuite de mémoire — dtor dérivé ignoré | Destructor de base non virtuel | Rendre `~Base()` virtuel |
| `cannot allocate object of abstract class type` | Tentative d'instancier une class avec des fonctions virtuelles pures non implémentées | Overrider chaque fonction virtuelle pure, ou utiliser une sous-classe concrète |
| `pure virtual method called` (runtime) | Appel d'une fonction virtuelle depuis un constructor/destructor | Ne pas appeler de fonctions virtuelles depuis un ctor/dtor |
| Défaut d'override subtil : l'« override » dérivé ne se déclenche pas | Discordance de signature (const, paramètres, type de retour) | Comparer soigneusement les déclarations de base et dérivées |

---

## 12. Résumé visuel

```
                  ┌──────────────────────────────────────┐
                  │   keyword: virtual                    │
                  └─────────────────┬────────────────────┘
                                    │
            ┌───────────────────────┼───────────────────────┐
            ▼                       ▼                       ▼
       virtual function        pure virtual           virtual base
       (dynamic dispatch)      (= 0, abstract)        (diamond resolution
                                                       in multiple inheritance)
            │                       │                       │
            ▼                       ▼                       ▼
       vtable + vptr           class is abstract:      shared base subobject
       per-class table         cannot instantiate;     in multi-inheritance
       per-object pointer      subclasses must         hierarchies. one of A,
                              implement to be          not two.
                              concrete.

    Object memory:
       ┌──────────────────────────────┐
       │  vptr  ──► class vtable      │  ← 8 bytes overhead per polymorphic object
       │  data members (in order)     │
       └──────────────────────────────┘

    Always:
       virtual base destructor when designing for inheritance.
       no virtual calls from ctor/dtor.
       pass polymorphic objects by ref/ptr (no slicing).
```

---

## 13. Pratique

1. Pourquoi `Animal a = dog;` affiche-t-il « generic noise » même lorsque `Dog::sound` est virtuel ? *(Slicing — `a` est un nouvel objet Animal avec le vptr d'Animal.)*
2. Quel est le coût d'un appel de fonction virtuelle par rapport à un appel non virtuel ? *(Un chargement mémoire supplémentaire (recherche dans la vtable) + un appel indirect. Généralement négligeable ; peut empêcher l'inlining.)*
3. Pourquoi une class de base polymorphique doit-elle avoir un destructor virtuel ? *(`delete base_ptr` ignorerait sinon le destructor dérivé → fuite/UB.)*
4. Pourquoi un appel virtuel depuis un constructor ne dispatche-t-il pas vers la class dérivée ? *(Le vptr est configuré sur la table de la class de base pendant la construction de la base ; la partie dérivée n'a pas encore été construite.)*