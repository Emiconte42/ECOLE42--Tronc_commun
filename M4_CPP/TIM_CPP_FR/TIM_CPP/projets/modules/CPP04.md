# Module C++ 04 — Guide d'apprentissage

**Thème du module :** *Polymorphisme de sous-type — dynamic dispatch, classes
abstraites et interfaces.*

Le Mod 03 vous a apporté l'héritage sans le polymorphisme. Ce module ajoute
la pièce manquante : lorsque vous manipulez un `Dog` à travers un `Animal*`,
l'appel à `makeSound()` doit aboyer, et non émettre un "bruit d'animal" générique.
C'est le **dynamic dispatch**, et le mécanisme sous-jacent
(vtables, vptrs) est au cœur du Mod 04.

Voir aussi : le fichier par module [`TOPICS.md`](../../../../LEVEL4/C++1/Mod04/TOPICS.md)
et [`oop/POLYMORPHISM.md`](../../notions/oop/POLYMORPHISM.md) pour la référence
complète.

---

## La seule phrase à mémoriser

> **`virtual`** fait en sorte que les appels de méthodes soient résolus en fonction du
> **type réel** de l'objet (le type que vous avez instancié avec `new`), et non du type
> statique du pointer ou de la reference à travers lequel vous le manipulez.

Oubliez `virtual` → le compiler choisit la fonction à la compilation
en fonction du type déclaré de la variable (static dispatch). Ajoutez
`virtual` → le compiler génère du code qui lit une **vtable** par class
à l'exécution et choisit le bon override (dynamic dispatch).

---

## La vtable, en un schéma

Pour toute class ayant au moins une méthode virtual :

```
class Animal              Animal's vtable
┌───────┐                 ┌──────────────────────┐
│ vptr ─┼──────────────►  │ &Animal::makeSound   │
│ type  │                 │ &Animal::~Animal     │
└───────┘                 └──────────────────────┘

class Dog : public Animal  Dog's vtable
┌───────┐                 ┌──────────────────────┐
│ vptr ─┼──────────────►  │ &Dog::makeSound      │   ← override
│ type  │                 │ &Dog::~Dog           │   ← override
│ brain │                 └──────────────────────┘
└───────┘
```

Chaque instance possède un `vptr` caché dans son header (généralement 8
octets). `p->makeSound()` devient *"charger le vptr de p ; charger le slot N ;
appeler via le pointeur."* Deux lectures mémoire supplémentaires, généralement
déjà dans le cache.

---

## Exercice 00 — Polymorphisme

### Ce que vous apprenez
**Le mot-clé `virtual` et la différence entre static et
dynamic dispatch**, observés côte à côte.

### Structure des classes
- `Animal` — `protected std::string type` ; `makeSound()` virtual.
  Concrète (instanciable).
- `Dog`, `Cat` — définissent `type`, font l'override de `makeSound()`.
- `WrongAnimal` / `WrongCat` — même structure, **sans `virtual`**.

### L'extrait clé dans le `main`

```cpp
Animal const* i = new Dog();
Animal const* j = new Cat();
i->makeSound();   // "bark"  ← le virtual dispatch fonctionne
j->makeSound();   // "meow"
delete i;
delete j;

WrongAnimal const* k = new WrongCat();
k->makeSound();   // "wrong animal sound" générique  ← static dispatch
delete k;
```

Ce contraste résume l'intégralité de l'exercice.

### Astuces
- **Rendez `~Animal()` virtual** même si l'ex00 n'en a pas besoin — l'ex01
  en a besoin, et vous l'oublierez si vous n'en prenez pas l'habitude.
- `c++98` n'a pas de mot-clé `override`. Signalez l'overriding en faisant
  correspondre la signature exacte. Une faute de frappe dans un type de paramètre
  crée silencieusement une nouvelle méthode au lieu d'en faire l'override — exécutez
  le code et testez.
- `type` est `protected` pour que les ctors dérivés puissent le définir sans passer
  par un setter.

### Piège
**N'oubliez pas le `const` sur `makeSound() const`.** Si la base a
`virtual void makeSound() const;` et que la dérivée a `virtual void
makeSound();` (sans const), la dérivée **ne fait pas d'override** — c'est
une nouvelle méthode qui masque l'ancienne. Le dispatch se rabat sur la base.

---

## Exercice 01 — I don't want to set the world on fire

### Ce que vous apprenez
**La deep copy de classes possédant des ressources**, et **la raison d'être
de la règle du destructeur virtual**.

### Structure des classes
- `Brain` — `std::string ideas[100]`. Plutôt basique, pas de pointers.
- `Dog` / `Cat` — possèdent un `Brain*` privé. Le ctor fait `new Brain()` ;
  le dtor fait `delete brain`.
- Main : array de `Animal*`, moitié `Dog`, moitié `Cat`, puis `delete`
  chacun à travers le pointer de base.

### La règle du destructeur virtual

```cpp
Animal* a = new Dog();
delete a;
```

- Si `~Animal()` est **virtual**, le compiler émet une recherche dans la vtable
  → `~Dog()` s'exécute d'abord, ce qui supprime le `Brain`, puis
  s'enchaîne avec `~Animal()`. Propre.
- Si `~Animal()` n'est **pas virtual**, le compiler code en dur un
  appel direct à `~Animal()`. `~Dog()` ne s'exécute jamais → fuite du `Brain`.
  **Comportement indéfini (undefined behaviour)** selon le standard.

Règle : **lorsque vous avez l'intention de supprimer un objet à travers un pointer de base,
le dtor de base doit être virtual.** `-Wnon-virtual-dtor` émet un avertissement à ce sujet ;
valgrind intercepte la fuite.

### Deep copy — pourquoi et comment

```cpp
Dog basic;
Dog copy = basic;    // copy ctor
```

Le copy ctor par défaut (généré par le compiler) effectue une copie membre par membre —
`basic` et `copy` pointent tous les deux vers le **même `Brain`**. Lorsque `copy`
est détruit, il supprime le cerveau. Le `brain` de `basic` devient un dangling pointer. Lorsque
`basic` est détruit : double-free.

La solution — copy ctor + copy-assign personnalisés :

```cpp
Dog::Dog(Dog const& src) : Animal(src), brain(new Brain(*src.brain)) {
    std::cout << "Dog copy constructed" << std::endl;
}

Dog& Dog::operator=(Dog const& other) {
    if (this != &other) {
        Animal::operator=(other);
        delete brain;                           // libère l'ancien
        brain = new Brain(*other.brain);        // deep-copy du nouveau
    }
    return *this;
}
```

Trois éléments à remarquer :
1. **Transmettre la copie de la tranche de base** — `Animal(src)` / `Animal::operator=(other)`.
2. **Supprimer l'ancien avant d'allouer le nouveau** dans `operator=`.
   Sinon, fuite.
3. **Protection contre l'auto-assignation (self-assignment guard)** — sinon `delete brain; brain = new
   Brain(*other.brain);` libère l'élément que vous êtes sur le point de copier.

### Astuces
- **Testez la deep-copy** : modifiez les idées de `copy`, affichez
  celles de `basic` — elles doivent différer. Si ce n'est pas le cas, votre copie
  est shallow.
- **Valgrind** : `--show-leak-kinds=all ./animals`. Toute fuite ou
  invalid-read signifie que le dtor virtual ou le pattern de deep-copy est
  incorrect.
- La class `Brain` elle-même n'a pas besoin d'OCF personnalisé — un array de
  `std::string` effectue déjà une deep-copy pour chaque élément.

### Piège
**L'array de `Animal*` dans le main**. Créez chacun d'eux avec `new Dog()` ou
`new Cat()`, stockez-les en tant que `Animal*`. L'upcast est implicite — aucun
cast n'est nécessaire. Pour les `delete` : `for (i=0; i<N; ++i) delete
zoo[i];`.

---

## Exercice 02 — Abstract class

### Ce que vous apprenez
**Les fonctions pure virtual et les classes abstraites.**

### La syntaxe

```cpp
class Animal {
public:
    virtual void makeSound() const = 0;   // pure virtual
};
```

`= 0` à la fin de la déclaration la marque comme pure. Une class possédant
au moins une méthode pure virtual est **abstraite** — vous ne pouvez pas l'instancier :

```cpp
Animal a;    // ← erreur de compilation : Animal est abstraite
Animal* p = new Dog();    // ← tout va bien
```

### Le contrat

Toute class concrète dérivée **doit faire l'override** de chaque fonction pure virtual
de la base. Oubliez-en une seule → la classe dérivée devient également abstraite → également
non instanciable. Contrainte vérifiée à la compilation : impossible d'oublier.

### Astuces
- Renommez `Animal` → `AAnimal` (la convention du préfixe `A` pour
  les classes abstraites) si vous souhaitez correspondre au nommage
  de `AMateria` dans l'ex03. Optionnel mais cohérent d'un point de vue stylistique.
- **Ne rendez pas le destructeur pure virtual.** Vous *pouvez*
  le faire (`virtual ~Animal() = 0;` + définition de corps), mais c'est un anti-pattern ici.
  Un simple virtual convient parfaitement.
- Tout ce qui vient de l'ex01 doit continuer à compiler et à se comporter de la même manière —
  la partie `Dog`/`Cat` reste inchangée.

### Piège
Même les classes abstraites ont des vtables et des vptrs. `sizeof(Animal)` reste
inchangé par rapport à l'ex01. Le langage refuse simplement de vous laisser
instancier la classe abstraite.

---

## Exercice 03 — Interface & récapitulatif (bonus)

### Ce que vous apprenez
**Les interfaces (classes purement abstraites) et le design pattern Prototype.**

### Les éléments en jeu

- **`AMateria`** — abstraite. `getType()`, `clone()` (pure), `use()`.
- **`Ice`**, **`Cure`** — concrètes. Font l'override de `clone()` et `use()`.
- **`ICharacter`** — interface purement abstraite. `getName`, `equip`,
  `unequip`, `use`.
- **`Character`** — implémente `ICharacter`. Inventaire de 4 emplacements.
- **`IMateriaSource`** — purement abstraite. `learnMateria`,
  `createMateria`.
- **`MateriaSource`** — implémente `IMateriaSource`. Jusqu'à 4
  modèles.

### Le pattern Prototype

`MateriaSource::createMateria(type)` doit retourner un `AMateria*`
du bon type concret — mais `MateriaSource` ne connaît pas les
types dérivés (pas de `if type == "ice" return new Ice()`). Solution :
conserver un modèle de chaque type, appeler `clone()` sur le modèle :

```cpp
AMateria* Ice::clone() const { return new Ice(); }

AMateria* MateriaSource::createMateria(std::string const& type) {
    for (int i = 0; i < 4; ++i)
        if (templates[i] && templates[i]->getType() == type)
            return templates[i]->clone();
    return 0;   // c++98: NULL fonctionne très bien aussi
}
```

Ajouter un nouveau type de materia (ex. `Fire`) = écrire la class + son
`clone()` + enregistrer un modèle via `learnMateria`. Aucune modification
dans `MateriaSource`.

### Deep-copy de l'inventaire de `Character`

```cpp
Character& Character::operator=(Character const& other) {
    if (this != &other) {
        for (int i = 0; i < 4; ++i) {
            delete inventory[i];
            inventory[i] = other.inventory[i]
                         ? other.inventory[i]->clone()
                         : 0;
        }
        name = other.name;
    }
    return *this;
}
```
`clone()` est la façon de « copier un objet polymorphique sans connaître son type ». C'est **l'**idiome par excellence pour ce genre d'ownership.

### Le contrat d'ownership — `unequip` ne delete pas

```cpp
void Character::unequip(int idx) {
    if (idx < 0 || idx >= 4) return;
    inventory[idx] = 0;    // retire de l'inventory, PAS de delete
}
```

L'appelant possède désormais ce que contenait l'inventory. Il peut le rééquiper ailleurs ou le jeter au sol. Si `unequip` appelait delete, le pointer de l'appelant deviendrait dangling — sans qu'il puisse s'en apercevoir.

### Conseils
- **Itération null-safe.** Les slots de l'inventory commencent à `0` (null). Vérifiez toujours `if (inventory[i])` avant d'appeler des méthodes.
- **Inventory plein / mauvais index** — no-op, pas d'affichage (sauf si le sujet spécifie le contraire).
- **`clone()` ne doit pas copier la string de type.** Construisez un nouvel `Ice()` (qui définit son propre type dans son ctor), et non `new Ice(*this)` (qui copierait le type via le copy ctor de base).
- **`c++98` utilise `0` ou `NULL`** pour les null pointers — pas de `nullptr`.

### Piège
Le dtor de `MateriaSource` doit delete chaque template non-null. Le dtor de `Character` doit delete chaque slot d'inventory non-null. Les materias laissées au sol (obtenues via `unequip`) sont sous la responsabilité du main — les tests vérifient cela.

---

## Ce que ce module vous apporte

- Vous pouvez lire une hiérarchie de class et prédire quelle méthode s'exécute à chaque appel.
- Le virtual destructor est un réflexe, pas une corvée.
- La deep-copy de pointers polymorphiques est un pattern que vous pouvez écrire sans réfléchir (`clone()` + `delete` de l'ancien + assignation du nouveau).
- Pure virtual + abstract class + convention d'interface (préfixe `A*` / `I*`) devient une seconde nature.

Après le Mod 04, C++ ressemble enfin à du C++ — et non à du C avec de la syntaxe en plus. Les modules restants (templates, STL, iterators, containers intelligents) s'appuient sur cette base.