# Polymorphism & Virtual Functions — Guide complet

> **Zoom sur les mots-clés :** [`VIRTUAL`](../../lexique/VIRTUAL.md) · [`TYPEID`](../../lexique/TYPEID.md)

Le polymorphisme signifie **« une seule interface, plusieurs comportements »**. Vous écrivez du code qui manipule des `Animal*` — et à l'exécution, il appelle `Dog::makeSound()` si l'objet réel est un `Dog`, ou `Cat::makeSound()` s'il s'agit d'un `Cat`. Le même point d'appel produit des résultats différents selon le type réel sous-jacent au pointer de base.

```cpp
Animal* zoo[3];
zoo[0] = new Dog();
zoo[1] = new Cat();
zoo[2] = new Dog();

for (int i = 0; i < 3; ++i)
    zoo[i]->makeSound();    // "Woof!" "Meow!" "Woof!"
```

Cela ne fonctionne que parce que `makeSound` est **virtual**.

---

## Table des matières

1. [Deux types de polymorphisme](#1.%20Two%20kinds%20of%20polymorphism)
2. [Le mot-clé `virtual`](#2.%20The%20virtual%20keyword)
3. [Dispatch statique vs dynamique](#3.%20Static%20vs%20dynamic%20dispatch)
4. [La vtable — fonctionnement en mémoire](#4.%20The%20vtable%20%E2%80%94%20how%20it%20works%20in%20memory)
5. [Fonctions virtuelles pures et classes abstraites](#5.%20Pure%20virtual%20functions%20and%20abstract%20classes)
6. [Les interfaces en C++](#6.%20Interfaces%20in%20C%2B%2B)
7. [La règle du destructeur virtuel](#7.%20The%20virtual%20destructor%20rule)
8. [L'object slicing — le tueur silencieux](#8.%20Object%20slicing%20%E2%80%94%20the%20silent%20killer)
9. [Appeler des méthodes virtuelles depuis les constructeurs/destructeurs](#9.%20Calling%20virtuals%20from%20ctors%2Fdtors)
10. [Le coût du virtuel](#10.%20The%20cost%20of%20virtual)
11. [Dans le CPP04](#11.%20In%20CPP04)
12. [Règles empiriques](#12.%20Rules%20of%20thumb)
13. [Pièges courants](#13.%20Gotchas)

---

## 1. Deux types de polymorphisme

Le C++ en possède deux :

- **Le polymorphisme ad-hoc (à la compilation)** — comportement différent sélectionné au moment de la compilation. Function overloading, operator overloading, templates.
  ```cpp
  void print(int);
  void print(const std::string&);
  print(42);      // appelle l'overload int — le compiler choisit
  ```
- **Le polymorphisme de sous-typage (à l'exécution)** — comportement différent sélectionné à l'**exécution**, selon le type réel de l'objet. Cela nécessite des fonctions `virtual`.
  ```cpp
  Animal* a = new Dog();
  a->makeSound();   // résolu à l'exécution vers Dog::makeSound
  ```

Ce guide traite du second type. Pour les templates (ad-hoc), voir [TEMPLATES.md](../advanced/TEMPLATES.md).

---

## 2. Le mot-clé `virtual`

Une méthode marquée `virtual` **peut être redéfinie (override)** par une sous-classe, et les appels via un pointer/une reference de base seront dirigés vers la version dérivée.

```cpp
class Animal {
public:
    virtual void makeSound() const { std::cout << "..." << '\n'; }
    virtual ~Animal() {}
};

class Dog : public Animal {
public:
    void makeSound() const { std::cout << "Woof!" << '\n'; }   // override
};
```

Règles :
- La méthode de la sous-classe doit avoir **la même signature** (même type de retour, mêmes paramètres, même qualification const). Sinon, vous déclarez une méthode *différente* qui masque celle de base.
- Vous pouvez réécrire `virtual` sur l'override — facultatif en C++98, utile comme rappel.
- Le C++11 ajoute le mot-clé `override` pour que le compiler vérifie les redéfinitions. Vous ne l'avez pas en C++98.

### Sans `virtual`

La méthode de base est appelée, même à travers un pointer dérivé :

```cpp
class Animal { public: void makeSound() { std::cout << "...\n"; } };
class Dog : public Animal { public: void makeSound() { std::cout << "Woof!\n"; } };

Dog d;
Animal* a = &d;
a->makeSound();        // "..." — la version de base, PAS celle de Dog
d.makeSound();         // "Woof!" — appelée directement sur Dog
```

Sans virtual : **dispatch statique** — le type déclaré du pointer décide.
Avec virtual : **dispatch dynamique** — le type réel de l'objet décide.

---

## 3. Dispatch statique vs dynamique

Le **dispatch statique** correspond à ce que l'on a en C : le compiler examine le type écrit sur la page et encode l'adresse de la fonction en dur.

```
Dog d;
d.bark();
  │
  └─► le compiler émet un appel direct à Dog::bark — adresse connue à la compilation
```

Le **dispatch dynamique** ajoute un niveau d'indirection. Le compiler recherche la méthode réelle à l'exécution via une table associée à l'objet.

```
Animal* a = new Dog();
a->makeSound();
  │
  ├─► charge a->vptr
  ├─► charge [vptr + offset_for_makeSound]    ← le slot de la vtable pour cette méthode
  └─► appel indirect à cette adresse          ← Dog::makeSound
```

Trois chargements mémoire + un appel indirect contre un appel codé en dur. C'est le coût ; il est faible mais non nul.

---

## 4. La vtable — fonctionnement en mémoire

Pour chaque class possédant au moins une fonction virtuelle, le compiler génère une **vtable** — un tableau de function pointers, à raison d'une entrée par méthode virtuelle. Chaque objet de cette class reçoit un membre caché **vptr** pointant vers la vtable de sa class.

```
class Animal {
    virtual void makeSound() const;
    virtual ~Animal();
};
class Dog : public Animal { void makeSound() const; };
class Cat : public Animal { void makeSound() const; };

         Dog's vtable                 Cat's vtable
       ┌──────────────────────┐     ┌──────────────────────┐
       │ &Dog::makeSound      │     │ &Cat::makeSound      │
       │ &Dog::~Dog           │     │ &Cat::~Cat           │
       └──────────────────────┘     └──────────────────────┘

Dog d;                                Cat c;
┌─────────────────┐                  ┌─────────────────┐
│ vptr ──► Dog's  │                  │ vptr ──► Cat's  │
│ vtable          │                  │ vtable          │
├─────────────────┤                  ├─────────────────┤
│ _type ...       │                  │ _type ...       │
└─────────────────┘                  └─────────────────┘
```

Lorsque vous écrivez `a->makeSound()` où `a` est un `Animal*` :
1. Le compiler sait que `makeSound` correspond au slot #0 de la vtable de `Animal`.
2. Il charge `a->vptr` — quelle que soit la vtable de l'objet réel.
3. Il charge le slot #0 depuis cette vtable — il obtient `Dog::makeSound` s'il s'agit d'un Dog.
4. Il appelle cette adresse.

Le vptr est initialisé par le **constructeur** de chaque class de la hiérarchie. Durant le constructeur de `Animal`, le vptr pointe vers la vtable de `Animal`. Ensuite, le constructeur de `Dog` s'exécute et met à jour le vptr pour pointer vers la vtable de `Dog`. C'est pourquoi appeler des méthodes virtuelles depuis un constructeur de base invoque la version de base (§9).

### Quelle est la taille de l'objet ?

La taille d'un pointer de plus que sans virtual. Sur une machine 64-bit, cela représente 8 octets de surcharge vptr par objet — partagés entre *toutes* les fonctions virtuelles, peu importe leur nombre.

---

## 5. Fonctions virtuelles pures et classes abstraites

Une fonction **virtuelle pure** n'a pas de corps dans la class de base. Syntaxe : `= 0` à la fin de la déclaration.

```cpp
class Shape {
public:
    virtual ~Shape() {}
    virtual double area() const = 0;        // virtuelle pure — aucune implémentation
};
```

Une class contenant au moins une fonction virtuelle pure est **abstraite** :

- Vous ne pouvez pas l'instancier. `Shape s;` est une erreur de compilation.
- Vous pouvez toujours manipuler des `Shape*` et des `Shape&`.
- Les sous-classes **doivent** redéfinir toutes les virtuelles pures pour devenir concrètes.

```cpp
class Circle : public Shape {
    double _r;
public:
    Circle(double r) : _r(r) {}
    double area() const { return 3.14159 * _r * _r; }   // fournit l'élément manquant
};

Shape* s = new Circle(5);                    // OK
std::cout << s->area();                       // 78.5...
delete s;
```

### Pourquoi les classes abstraites existent

Pour exprimer : « cette catégorie d'entités *doit* posséder cette opération, mais je ne peux pas fournir de comportement par défaut qui ait du sens ». Chaque `Shape` possède une aire (`area`), mais il n'existe pas de valeur par défaut logique — vous forcez donc chaque class dérivée à définir la sienne.

---

## 6. Les interfaces en C++

Le C++ ne dispose pas du mot-clé `interface`. À la place, une **interface** est simplement une class abstraite contenant :

- Uniquement des fonctions virtuelles pures
- Un destructeur virtuel
- Aucun membre de données

```cpp
class IClickable {
public:
    virtual ~IClickable() {}
    virtual void onClick() = 0;
    virtual bool isEnabled() const = 0;
};
```

Toute class qui hérite publiquement (`public`) de `IClickable` et implémente les deux méthodes **est un** `IClickable`. C'est l'idiome C++ équivalent aux interfaces Java/C#.

Les classes peuvent hériter de **plusieurs** interfaces sans rencontrer les problèmes habituels de l'héritage multiple, car les interfaces ne possèdent pas de données pouvant entrer en conflit.

---

## 7. La règle du destructeur virtuel

**Si un objet est susceptible d'être détruit via `delete` à travers un pointer de base, votre destructeur de base DOIT être virtuel.**

```cpp
class Animal {
public:
    /* virtual */ ~Animal() {}
};
class Dog : public Animal {
    int* _brain;
public:
    Dog() : _brain(new int[100]) {}
    ~Dog() { delete[] _brain; }
};

Animal* a = new Dog();
delete a;
```

- **Sans `virtual` sur `~Animal()` :** seul `~Animal()` s'exécute. `_brain` fuite. Il s'agit d'un comportement indéfini (Undefined Behavior) selon le standard.
- **Avec `virtual` sur `~Animal()` :** `~Dog()` s'exécute d'abord (libérant `_brain`), puis `~Animal()` s'exécute.

La règle à 42 : **chaque class de base a `virtual ~ClassName();`**. Toujours. Même si vous pensez que personne ne supprimera via un pointer de base aujourd'hui, quelqu'un le fera demain, et le coût se résume à un vptr que vous possédez déjà pour les autres fonctions virtuelles.

---

## 8. L'object slicing — le tueur silencieux

Assigner une instance dérivée **par valeur** dans une instance de base ne copie que le sous-objet de base. La partie dérivée est silencieusement supprimée.
```cpp
Dog    d;
Animal a = d;       // SLICE : seul le sous-objet Animal est copié
a.makeSound();      // version de base — il n'y a plus de Dog ici
```

Même chose avec les arguments de fonction :

```cpp
void treat(Animal a);         // par valeur — découpe tout ce qui lui est passé
treat(d);                     // la nature de Dog a disparu dès l'entrée dans la fonction

void treat(const Animal& a);  // par reference — sûr, a est réellement un Dog
treat(d);                     // appelle correctement les virtuals de Dog
```

**Règle : passez les objets polymorphiques par pointer ou reference.** Jamais par valeur.

Les conteneurs ont le même problème : `std::vector<Animal>` subit un slicing. Si vous avez besoin d'un conteneur d'animaux mixtes, il doit s'agir d'un `std::vector<Animal*>`.

---

## 9. Appeler des fonctions virtuals depuis les ctors/dtors

**Pendant un constructeur, l'objet n'est pas encore un « Dog » — il est toujours en cours d'assemblage.** La base de `Dog` s'exécute en premier, donc le vptr pointe vers la vtable d'`Animal` pendant que le constructeur d'`Animal` s'exécute :

```cpp
class Animal {
public:
    Animal()          { init(); }      // appelle quel init() ?
    virtual void init() { std::cout << "Animal::init\n"; }
};
class Dog : public Animal {
public:
    void init() { std::cout << "Dog::init\n"; }
};

Dog d;    // affiche "Animal::init" — PAS "Dog::init"
```

La même chose se produit en sens inverse lors de la destruction : quand `~Animal()` s'exécute, la partie `Dog` est déjà détruite, donc les appels de fonctions virtuals se résolvent vers la version d'`Animal`.

**N'appelez pas de fonctions virtuals depuis des constructeurs ou des destructeurs**, à moins d'en comprendre parfaitement les mécanismes. Le pattern le plus sûr consiste à construire complètement l'objet, puis à appeler explicitement la fonction virtual.

---

## 10. Le coût du virtual

- **Mémoire :** un pointer supplémentaire par objet (le vptr), une vtable par class.
- **Temps :** deux lectures mémoire supplémentaires et un appel indirect par invocation virtual. Les CPU modernes prédisent ces appels assez bien, mais un appel indirect empêche l'inlining.
- **Taille du binaire :** légèrement plus grande — toutes les vtables sont émises dans l'exécutable.

Pour du code applicatif classique, ce coût est négligeable. Pour des boucles internes critiques (*tight inner loops*), le dispatch virtuel peut avoir un impact. Le principe de conception du C++ — « you don't pay for what you don't use » (vous ne payez pas pour ce que vous n'utilisez pas) — signifie que les fonctions non-virtual sont aussi peu coûteuses que des appels C ; seules les classes utilisant des virtuals paient le prix de la vtable.

---

## 11. Dans CPP04

C'est dans ce module que le polymorphisme prend enfin tout son sens.

- **ex00 — Base `Animal`, sous-classes `Dog`/`Cat`, et un `WrongAnimal`.**
  Comparez le comportement avec et sans `virtual makeSound()`. La hiérarchie `WrongAnimal` existe pour montrer ce qui arrive quand on oublie `virtual` : la méthode de la classe de base est appelée à travers un `WrongAnimal*`, même s'il s'agit en réalité d'un `WrongCat`.

- **ex01 — `Brain` en tant que membre composé.**
  `Dog` et `Cat` possèdent chacun un `Brain*`. C'est ici que la rigueur de la deep-copy prend toute son importance : le copy constructor doit cloner le `Brain` ; l'`operator=` doit gérer l'auto-assignation (*self-assignment*) et libérer l'ancien brain. Créez un `std::vector<Animal*>` de `Dog`s et `Cat`s mélangés, puis copiez le vector — tout doit survivre.

- **ex02 — Rendre `Animal` abstraite.**
  Transformez `Animal::makeSound` en `= 0`. Désormais, `Animal a;` produit une erreur de compilation, mais `Animal* a = new Dog();` fonctionne toujours — et devient la seule manière d'en manipuler un. Cela applique la règle : « impossible d'avoir un Animal *générique* ; choisissez une espèce précise. »

- **ex03 — L'exercice d'interfaces `IMateria` / `AMateria` / `Character` / `MateriaSource`.**
  Une véritable hiérarchie d'interfaces C++. `IMateria` (ou équivalent) est une class abstraite ne comportant que des pure virtuals ; `AMateria` est une implémentation partielle ; `Ice` et `Cure` sont concrètes. `Character` stocke un tableau `AMateria*[4]`. Vous manipulez un tableau polymorphique sans en connaître le type concret — ce qui est tout l'intérêt de la démarche.

À la fin, vous aurez manipulé : fonctions virtuals, impact des vtables, classes abstraites, interfaces, tableaux polymorphiques et deep-copy au sein d'une hiérarchie polymorphique.

---

## 12. Règles empiriques

- **Destructeur virtual sur chaque classe de base destinée à être détruite de manière polymorphique.** Toujours.
- **Si vous ajoutez une fonction virtual, demandez-vous : ai-je déjà un destructeur virtual ?** Généralement oui dès qu'il y a la moindre méthode virtual, mais vérifiez systématiquement.
- **Les signatures d'override doivent correspondre exactement** (nom, paramètres, const-ness). Dans le cas contraire, vous ajoutez une nouvelle méthode sans surcharger l'existante.
- **Utilisez des classes abstraites pour les contrats à « implémentation obligatoire ».** Ne fournissez pas d'implémentation par défaut boiteuse pour un comportement que chaque sous-classe doit obligatoirement personnaliser.
- **Interfaces = classes abstraites avec uniquement des pure virtuals.** Gardez-les concises.
- **Passez les objets polymorphiques par pointer ou reference.** Jamais par valeur.
- **N'appelez pas de fonctions virtuals depuis les constructeurs ou destructeurs.**

---

## 13. Pièges courants

- **Oublier `virtual` sur le destructeur** → un `delete` via un base pointer engendre des fuites de ressources.
- **Non-concordance de signature d'override** (ex. `const` sur l'override mais pas sur la base, ou vice-versa) → crée silencieusement une nouvelle méthode sans override. La version de base continue d'être appelée.
- **Slicing** lors du passage par valeur ou avec un conteneur stockant par valeur. `std::list<Animal>` découpe (slice) ; `std::list<Animal*>` ne découpe pas.
- **Les virtuals dans les constructeurs/destructeurs ne dispatch pas vers les classes dérivées.**
- **Les pure virtuals nécessitent tout de même une définition si vous voulez les appeler via `Base::foo()` depuis un override.** Rare mais légal.
- **L'héritage multiple de classes concrètes est un champ de mines.** Les interfaces (uniquement pure-virtual) ne posent aucun problème.
- **`dynamic_cast` requiert une base polymorphique** (au moins une fonction virtual). Voir [CASTS.md](../../lexique/CASTS.md).
- **Confusion entre statique et dynamique.** `Animal* a = new Dog; typeid(*a) == typeid(Dog)` est vrai ; `typeid(a) == typeid(Animal*)` est également vrai. `*a` est dynamique ; `a` est statique.

---

## 14. Vue matérielle — ce que fait réellement le CPU

### Un appel virtual, en pseudo-assembleur x86-64

`a->makeSound()` où `a` est un `Animal*` :

```asm
mov  rax, [a]            ; charge le vptr (les 8 premiers octets de l'objet)
mov  rax, [rax + 0]      ; charge le slot 0 de la vtable (makeSound)
mov  rdi, a              ; passe `this` en argument 0
call rax                 ; appel indirect via le pointer chargé
```

Deux lectures mémoire interdépendantes, puis un appel indirect. À comparer avec un appel non-virtual, qui se résume à une unique instruction directe `call <symbol>`.

### Prédiction de branchement

Les CPU modernes intègrent un **prédicteur de branchement indirect** (*indirect branch predictor*) qui met en cache la cible des appels indirects récents. Quand `a` pointe quasi systématiquement vers le même type concret (site d'appel monomorphique), le prédicteur réussit (*hit*) et l'appel ne coûte presque rien — 1 à 2 cycles plus les lectures mémoire. Lorsque le site d'appel voit défiler de nombreux types différents (mégamorphique), le prédicteur échoue (*miss*) et vous écopez d'une purge de pipeline (*pipeline flush*) de 10 à 20 cycles par échec.

C'est pourquoi la dévirtualisation (quand le compiler réussit à prouver le type concret lors de la compilation) représente un enjeu d'optimisation majeur. Dans votre code à 42, partez du principe qu'un appel virtual coûte ~5 cycles et passez à la suite.

### Impact sur le cache

Le vptr correspond généralement aux 8 premiers octets de l'objet. L'accès à n'importe quel membre charge également le vptr en cache (même ligne de cache). Le premier accès à la vtable est donc le plus souvent chaud en L1 (*L1-hot*). La vtable elle-même est partagée entre toutes les instances — une ligne de cache par vtable de class, généralement chaude également.

### L'emplacement de la vtable dans le binaire

Les vtables résident dans la section `.rodata` (*read-only data*). Chaque vtable contient l'adresse des méthodes virtuals de la class, ainsi que les informations RTTI (utilisées par `dynamic_cast` et `typeid`). Le linker déduplique les vtables entre les unités de traduction via la « one definition rule » combinée aux symboles faibles (*weak symbols*).

La commande `objdump -d -s -j .rodata ./program | grep -A 8 "vtable for Dog"` permet de l'observer si vous êtes curieux.

### Ce que fait réellement `dynamic_cast<Dog*>(animal_ptr)`

1. Charge le vptr.
2. Parcourt la structure jusqu'aux informations RTTI rattachées à la vtable.
3. Compare le type RTTI au RTTI de `Dog`.
4. En cas de correspondance (directe ou via la chaîne des classes de base), renvoie le pointer ajusté. Sinon, renvoie `NULL`.

L'étape 3 consiste en une **comparaison de chaînes** (*string compare*) sur les noms de types dans beaucoup d'ABIs (notamment l'Itanium C++ ABI). C'est lent — de l'ordre de la microseconde pour une hiérarchie profonde. N'employez `dynamic_cast` que lorsque vous avez strictement besoin d'une discrimination de type à l'exécution.

### Le stub pure-virtual

Dans l'ABI Itanium, une fonction pure virtual dans la vtable pointe vers `__cxa_pure_virtual`. Si elle venait à être invoquée (par exemple, en appelant une fonction virtual depuis un ctor de base avant que la classe dérivée ne soit instanciée), elle appelle `std::terminate` — le programme plante immédiatement. C'est le mécanisme de verrouillage à l'exécution de l'abstraction, au-delà de l'interdiction d'instanciation vérifiée par le compiler.