# PHILOSOPHY.md

*"Make it correct, make it clear, make it concise, make it fast. In that order."* — Wietse Venema

Un document boussole pour la piscine C++ de 42. Cinq piliers à assimiler, ainsi que l'état d'esprit qui transforme *"écrire du C++"* en *"penser en C++"*. Lisez-le une première fois maintenant, puis revenez-y dès que chaque pilier apparaît dans vos modules.

---

## 🧭 La philosophie en une ligne

> **"You don't pay for what you don't use, and what you do use, you couldn't write better by hand."** — Bjarne Stroustrup

Chaque décision de conception du C++ découle de ce principe. Le langage refuse de masquer la machine, et refuse de vous faire payer pour des abstractions que vous n'avez pas demandées. **Contrôle total, responsabilité totale.**

---

## 📑 TL;DR — les cinq piliers

| # | Pilier | Slogan | Vu pour la première fois dans |
|---|---|---|---|
| 1 | [Object Lifetime & RAII](#1.%20Object%20Lifetime%20%26%20RAII) | "Les ressources naissent et meurent avec les objets." | Mod01 |
| 2 | [Value Semantics & OCF](#2.%20Value%20Semantics%20%26%20Orthodox%20Canonical%20Form) | "Que signifie copier cet objet ?" | Mod02 |
| 3 | [Const-Correctness](#3.%20Const-Correctness%20%26%20Type%20Safety) | "Laissez le compiler appliquer vos contrats." | Mod02 |
| 4 | [Static + Dynamic Polymorphism](#4.%20Polymorphism%20%E2%80%94%20Static%20AND%20Dynamic) | "Décidez au compile time quand vous le pouvez ; au runtime quand vous le devez." | Mod03 / Mod07 |
| 5 | [Memory Layout & Zero-Overhead](#5.%20Memory%20Layout%20%26%20Zero-Overhead%20Abstraction) | "Sachez en quoi votre code se compile." | Mod02+ implicitement |

---

## 1. Object Lifetime & RAII

> *"En C++, un objet n'est pas seulement de la donnée — c'est une entité qui existe entre deux moments précis : sa construction et sa destruction."*

### Le concept

Chaque objet a une naissance (constructor) et une mort (destructor). Entre les deux, son destructor *sera* exécuté — c'est une garantie absolue du langage. Des idiomes entiers du langage reposent sur ce principe.

**RAII** = **R**esource **A**cquisition **I**s **I**nitialization. Liez n'importe quelle ressource — mémoire, handle de fichier, mutex, socket, verrou — à la durée de vie d'un objet. Lorsque l'objet meurt (sortie de scope, exception levée, retour de fonction), la ressource est libérée *automatiquement*.

### Pourquoi c'est important

En C, `fopen` vous oblige à ne pas oublier `fclose` sur chaque chemin d'exécution :

```c
FILE *f = fopen("data.txt", "r");
if (!f) return -1;
if (parse(f) < 0) {
    fclose(f);          // n'oubliez pas celui-ci
    return -1;
}
if (validate(f) < 0) {
    fclose(f);          // …ni celui-là
    return -1;
}
fclose(f);              // …ni celui-ci non plus
return 0;
```

En C++, le destructor gère chaque chemin :

```cpp
{
    std::ifstream f("data.txt");      // ← acquiert le fichier
    if (!f) return -1;
    parse(f);
    if (bad)
        throw std::runtime_error("nope");  // ← nettoie quand même
}                                     // ← f.~ifstream() ferme le fichier. Toujours.
```

**Adieu à des catégories entières de bugs** : descripteurs de fichiers non fermés, sockets orphelins, mutexes jamais relâchés, `delete`s oubliés.

### Le même pattern, pour chaque ressource

```cpp
class Buffer {
    char *_data;
    size_t _size;
public:
    Buffer(size_t n) : _data(new char[n]), _size(n) {}  // acquisition
    ~Buffer() { delete[] _data; }                       // libération — garantie
};
```

Un `Buffer` sur la stack ? La mémoire sur le heap est libérée à la sortie du scope. En tant que membre d'une autre class ? Libéré quand le parent meurt. Dans un `std::vector<Buffer>` ? Chacun d'eux est libéré à la destruction du conteneur. **Le pattern est composable.**

### Où vous le rencontrerez

- **Mod01 ex02–06** : lorsque vous commencez à écrire `Zombie`, vous apprenez en réalité la différence entre des zombies alloués sur la stack (auto-nettoyage) et des zombies alloués sur le heap (votre responsabilité).
- **Mod02+** : toute class ayant un `new` dans son constructor a besoin d'un `delete` dans son destructor. Le RAII indique que *"c'est la responsabilité du même objet."*

📍 Voir : [`fundamentals/MEMORY.md`](fundamentals/MEMORY.md) · [`lexique/NEW.md`](../lexique/NEW.md) · [`lexique/DELETE.md`](../lexique/DELETE.md)

---

## 2. Value Semantics & Orthodox Canonical Form

> *"Un objet C++ devrait se comporter comme un `int` — copiable, assignable, destructible, sans surprise."*

### Le concept

C, Rust, Java, Python — ils adoptent tous par défaut des sémantiques différentes. Le comportement par défaut du C++ est la **value semantics** : lors d'une assignation ou d'un passage par valeur, vous obtenez une *copie*. Rapide (juste de la mémoire), local (aucun effet de bord par aliasing), explicite.

Mais pour que la value semantics fonctionne sur *votre* class, vous devez au langage **quatre membres** :

```cpp
class Fixed {
public:
    Fixed();                                  // 1. default ctor
    Fixed(const Fixed& other);                // 2. copy ctor
    Fixed& operator=(const Fixed& other);     // 3. copy-assignment
    ~Fixed();                                 // 4. destructor
};
```

C'est la **Forme Canonique Orthodoxe** (OCF) — le terme employé par 42 pour ce que la communauté C++ appelle la **Rule of Three**.

### La Rule of Three (C++98)

> *Si vous écrivez **l'un** des trois membres suivants [destructor, copy constructor, copy assignment], vous avez presque certainement besoin des **trois**.*

Pourquoi ? Si votre class gère une ressource (raw pointer, fichier, etc.), la copie **générée par le compiler** fera un mauvais choix — elle réalisera une copie superficielle (shallow copy) du pointer, et désormais deux objets penseront en être les propriétaires :

```
Before copy:                   After shallow copy:
┌──────┐                      ┌──────┐    ┌──────┐
│ obj1 │──→ [heap data]       │ obj1 │─┐  │ obj2 │
└──────┘                      └──────┘ ├─→[heap data]
                                       │
                              obj2 ────┘
                              ↑
                              Now both will free the same memory → 💥
```

→ Double-free à la destruction. Crash, undefined behavior, crise de nerfs de valgrind.

### Le changement profond d'état d'esprit

Pour chaque class, posez-vous la question : ***"Que signifie copier cet objet ?"***

- Pour un type de type valeur (`Fixed`, `Vector3D`) : "dupliquer les bits" — simple.
- Pour un type possédant un pointer (`Buffer` ci-dessus) : "faire une copie profonde (deep copy) du buffer" — doit être implémenté.
- Pour un type qui "ne devrait jamais être copié" (`std::ifstream`, wrapper de mutex) : "rendre la copie impossible à la compilation" — en C++98, déclarez-les en `private` sans corps.

Vous **décidez** — mais vous devez trancher. Le langage ne choisira pas pour vous.

### Le pattern canonique

```cpp
Fixed::Fixed(const Fixed& other) : _value(other._value) {
    std::cout << "Copy constructor called" << std::endl;
}

Fixed& Fixed::operator=(const Fixed& other) {
    std::cout << "Assignment operator called" << std::endl;
    if (this != &other)             // protection contre l'auto-assignation
        _value = other._value;
    return *this;                   // retour de *this pour le chaînage
}
```

Trois détails piègent souvent :
1. **Le copy ctor prend une `const T&`** — pas `T` (récursion infinie !) et pas `T&` (ne peut pas se lier à des temporaires).
2. **`operator=` retourne une `T&`** — pour que l'écriture `a = b = c` fonctionne.
3. **Vérification de l'auto-assignation** — `a = a` doit être sans effet, et non déclencher une destruction.

### Où vous le rencontrerez

- **Mod02** : la quasi-totalité du module enseigne l'OCF à travers `Fixed`.
- **Mod03** : chaque derived class a besoin de sa propre OCF (ou d'utiliser explicitement celle de la base).
- **Mod04** : le virtual destructor intègre l'OCF.
- **Mod05–08** : elle est systématiquement présente.

📍 Voir : [`oop/ORTHODOX_CANONICAL_FORM.md`](oop/ORTHODOX_CANONICAL_FORM.md) · [`oop/OPERATOR_OVERLOADING.md`](oop/OPERATOR_OVERLOADING.md)

---

## 3. Const-Correctness & Type Safety

> *"Exprimez vos intentions dans le type. Faites du compiler votre ingénieur QA."*

### Le concept

Le système de types du C++ est son outil le plus sous-estimé. La plupart des débutants considèrent les types comme des *contraintes qu'ils doivent respecter*. Les experts les considèrent comme des *contrats dont ils tirent parti*.

```cpp
int          getValue() const;             // ne modifie pas *this
const int&   getValue() const;             // renvoie une ref en lecture seule (pas de copie)
void         setValue(int v);              // passage par valeur (peu coûteux pour les int)
void         setName(const std::string&);  // const-ref : pas de copie, pas de mutation
```

Chaque `const` est une **promesse** — envers l'appelant, le lecteur, et votre futur vous. Le compiler la fait respecter. Vous essayez de modifier un membre `const` dans une méthode `const` ? Le build échoue. Vous essayez de passer un objet `const` à un setter non-`const` ? Le build échoue. Ce n'est pas contraignant — c'est *le langage qui vous évite un bug*.

### L'idée sous-jacente : rendre les états illégaux impossibles à représenter

Si une fonction ne doit jamais modifier son argument → prenez-le en `const`.
Si une méthode ne doit jamais modifier l'objet → marquez-la `const`.
Si une variable ne doit jamais changer → déclarez-la `const`.
Si une fonction ne doit jamais lever d'exception → utilisez `throw()` (spécification C++98).

Chacun de ces choix ferme une porte par laquelle les bugs pourraient entrer.

### L'exemple de deux interfaces

```cpp
// Mauvais — chaque appelant doit lire le code source pour savoir ce qui est sûr
class Account {
    int balance;
    std::string owner;
public:
    int          getBalance();
    std::string  getOwner();
    void         deposit(int amount);
};

// Bon — les signatures suffisent à documenter le comportement
class Account {
    int _balance;
    std::string _owner;
public:
    int                  getBalance() const;       // const → "je ne modifierai rien"
    const std::string&   getOwner() const;         // const-ref → "pas de copie, lecture seule"
    void                 deposit(int amount);      // non-const → "je vais modifier l'état"
};
```
En lisant le second, l'appelant sait immédiatement ce qu'il peut appeler en toute sécurité sur une `const Account&`. Pas besoin de plonger dans le code source.

### L'antisèche `const`

```cpp
const int x;            // x est un int qui ne peut pas être modifié
int *p;                 // p est un pointer vers un int (les deux sont mutables)
const int *p;           // p pointe vers un const int (données immuables, p mutable)
int * const p;          // p est un const pointer vers un int (p immuable, données mutables)
const int * const p;    // les deux sont immuables
const std::string& s;   // reference vers un string const — *l'*idiome de paramètre par excellence
```

**Lire de droite à gauche** : `const int * const p` → « p est un const pointer vers un const int ».

### Le changement d'état d'esprit

Avant d'écrire le corps d'une fonction, écrivez sa **signature comme un contrat complet**. Ensuite, implémentez.

### Où vous le rencontrez

- **Mod00** : dès que vous écrirez votre première méthode, vous vous demanderez *« est-ce que cela devrait être const ? »*. La réponse est *presque toujours oui*.
- **Mod02** : `getRawBits() const` et `setRawBits(int)` — le contraste canonique.
- **Mod04+** : la const-correctness dans les arbres d'héritage devient subtile. Persévérez.

📍 Voir : [`lexique/CONST.md`](../lexique/CONST.md)

---

## 4. Le polymorphisme — Statique ET Dynamique

> *« Il existe deux façons d'écrire du code qui fonctionne pour plusieurs types : choisir à la compilation (templates), ou choisir à l'exécution (virtual). Les deux sont de premier ordre en C++. »*

### L'idée

La plupart des langages ne proposent qu'un seul polymorphisme. Le C++ propose les **deux**, délibérément, car ils ont des coûts différents :

| | **Statique (templates)** | **Dynamique (virtual)** |
|---|---|---|
| Type choisi | À la compilation | À l'exécution |
| Coût par appel | **Zéro** (souvent inliné) | Une indirection (recherche dans la vtable) |
| Taille du binaire | Plus grande (une copie par type) | Plus petite |
| Utiliser quand | Le type est connu à la compilation, perf max | Collections hétérogènes, plugins |
| Module | Mod07 | Mod03–04 |

### Statique — templates

```cpp
template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

int main() {
    max<int>(3, 7);          // génère max<int>
    max<double>(1.5, 2.0);   // génère max<double>
    max<std::string>("a", "b"); // génère max<std::string>
}
```

Le compiler **génère une version spécialisée** pour chaque type que vous utilisez. Chaque point d'appel saute vers du code machine spécifique avec un surcoût nul à l'exécution.

### Dynamique — fonctions virtuelles

```cpp
class Animal {
public:
    virtual void speak() const = 0;       // pure virtual → abstract
    virtual ~Animal() {}                  // virtual dtor → obligatoire !
};

class Dog : public Animal {
public:
    void speak() const { std::cout << "Woof!\n"; }
};

class Cat : public Animal {
public:
    void speak() const { std::cout << "Meow!\n"; }
};

Animal *zoo[] = { new Dog(), new Cat(), new Dog() };
for (int i = 0; i < 3; ++i)
    zoo[i]->speak();      // dispatché à l'exécution via la vtable
```

Chaque objet polymorphique transporte un **vptr** caché qui pointe vers la **vtable** de sa class — un tableau de pointeurs de fonctions. Appeler `zoo[i]->speak()` devient *« chercher l'entrée N dans la vtable de cet objet »* — une indirection, puis un appel normal.

### Schéma mémoire (héritage simple)

```
new Dog():                              Dog::vtable (.rodata):
┌─────────────────────┐                 ┌───────────────────────┐
│ vptr                │────────────────►│ &Dog::speak           │
├─────────────────────┤                 │ &Dog::~Dog            │
│ Dog members…        │                 └───────────────────────┘
└─────────────────────┘
```

### Le piège qui définit le Mod04 : destructeur virtuel

```cpp
class Base { ~Base() {} };                    // MAUVAIS : dtor non-virtual
class Derived : public Base { /* possède de la heap */ };

Base *p = new Derived();
delete p;     // ← seul Base::~Base s'exécute. Derived::~Derived est ignoré. LEAK.
```

Rendez `~Base` virtual — et maintenant `delete p` recherche le destructeur dans la vtable, trouve `~Derived`, l'exécute (qui s'enchaîne ensuite vers `~Base`). **Rendez toujours les destructeurs virtuels dans toute class destinée à être une class de base.**

### La philosophie : ne payez pas pour un dispatch dont vous n'avez pas besoin

Si vous connaissez le type à la compilation → template, coût nul.
Si vous avez véritablement besoin de choisir à l'exécution (liste hétérogène, système de plugins) → virtual, une indirection.
Si vous utilisez `virtual` par réflexe sous prétexte de « POO » → vous payez pour rien.

### Le changement d'état d'esprit

Demandez-vous *« quand est-ce que je connais réellement le type ? »*
- À la compilation → template.
- À l'exécution → virtual.

### Où vous le rencontrez

- **Mod03** : héritage, chaînes de ctor/dtor, le problème du diamant.
- **Mod04** : virtual, abstract, interfaces — le cœur du dispatch dynamique.
- **Mod07** : function et class templates.
- **Mod08–09** : la STL est constituée de templates *de fond en comble*.

📍 Voir : [`oop/POLYMORPHISM.md`](oop/POLYMORPHISM.md) · [`advanced/TEMPLATES.md`](advanced/TEMPLATES.md) · [`lexique/VIRTUAL.md`](../lexique/VIRTUAL.md) · [`lexique/TEMPLATE.md`](../lexique/TEMPLATE.md)

---

## 5. Agencement mémoire et abstraction à coût zéro

> *« Sachez en quoi votre code est compilé. Lignes de cache, indirections, copies — tout cela est bien réel. »*

### L'idée

C'est le pilier qui sépare les programmeurs C++ des *bons* programmeurs C++. Le C++ expose la machine. Une fois que vous comprenez :

- **Stack vs heap** — la stack est rapide (déplacement de pointer, nettoyage automatique) ; la heap est flexible mais lente (ballet de l'allocateur, nettoyage manuel).
- **Pointer chase** — chaque `->` est potentiellement un cache miss.
- **Taille des objets** — les fonctions virtuelles coûtent 8 octets (`vptr`) ; les references coûtent un pointer ; le padding peut doubler la taille d'une struct.
- **Passage par valeur vs passage par const-ref** — copier un `std::string` alloue de la mémoire ; copier un `int` est gratuit.
- **Inlining** — les petites fonctions définies dans les headers peuvent être inlinées à néant ; la même fonction dans un `.cpp` ne peut pas traverser les frontières des unités de traduction.

…vous commencez à écrire du code qui *semble* de haut niveau mais se compile en un assembly optimisé. **C'est la promesse du coût zéro (zero-overhead).**

### L'affirmation célèbre

Ces trois implémentations se compilent en un **code machine identique** sous `-O2` :

```cpp
int  add_function(int a, int b)      { return a + b; }
inline int add_inlined(int a, int b) { return a + b; }
template <typename T> T add_tpl(T a, T b) { return a + b; }
```

Vous ne payez pas pour l'abstraction. Le compiler voit à travers elle.

### Tailles que vous devriez intérioriser (Apple Silicon, 64-bit)

```cpp
sizeof(char)          == 1
sizeof(int)           == 4
sizeof(long)          == 8
sizeof(double)        == 8
sizeof(void*)         == 8
sizeof(std::string)   == 24      // optimisation pour petites chaînes (SSO)
sizeof(Empty)         == 1       // une class vide a toujours une identité
sizeof(WithVirtual)   == 8       // un vptr apparaît dès que vous ajoutez `virtual`
```

Chaque `virtual` que vous ajoutez à une class coûte un pointer par objet. Peu importe le nombre de méthodes virtuelles — le coût est d'un seul vptr (une seule vtable par class, partagée par toutes les instances).

### La véritable leçon : l'intuition stack-vs-heap

```cpp
void fast() {
    Fixed f;                   // stack: 1 instruction (sub rsp, 16)
    use(f);
}                              // 1 instruction pour libérer (add rsp, 16)

void slow() {
    Fixed *f = new Fixed();    // heap: malloc → recherche dans la free list → peut-être sbrk → peut-être lock
    use(*f);
    delete f;                  // heap: free → fusionne → peut-être split
}
```

Les deux sont corrects. Le premier est **littéralement des centaines de fois plus rapide** lorsqu'il est appelé dans une boucle. *Utilisez la heap quand vous y êtes obligé* (durée de vie plus longue que la fonction, taille inconnue à la compilation, polymorphisme), pas parce que cela semble « plus dynamique ».

### Des outils pour développer cette intuition

- `sizeof(T)` affiché dans le `main()` — vérification rapide de la réalité
- `c++ -S file.cpp -o -` → voir l'assembly pour du code non optimisé
- `c++ -O2 -S file.cpp -o -` → voir ce que l'optimiseur en a fait
- [godbolt.org](https://godbolt.org) → l'assembly côte à côte avec le code source, en ligne
- `valgrind` → intercepte les use-after-free, les leaks, les lectures non initialisées
- `valgrind --tool=cachegrind` → profileur de cache-miss (pour quand les performances vous importent)

**Essayez ceci une fois pendant la piscine** : prenez votre `Fixed` du Mod02, compilez avec `c++ -O2 -S Fixed.cpp -o -`, et regardez la sortie pour `Fixed::operator+`. Vous ne regarderez plus jamais le C++ de la même façon.

### Le changement d'état d'esprit

N'optimisez pas prématurément — mais **comprenez ce que coûtent vos abstractions**. Choisissez ensuite les abstractions qui ne coûtent rien.

### Où vous le rencontrez

- **Implicite partout**, mais moments explicites :
  - **Mod02 ex02** : `Fixed::operator+` — copies vs references
  - **Mod04** : `sizeof` d'une class avec du virtual
  - **Mod07** : coût d'instanciation des templates (taille du binaire)
  - **Mod08** : agencement mémoire des conteneurs STL (`vector` est contigu, `list` est une chasse aux pointeurs)

📍 Voir : [`fundamentals/MEMORY.md`](fundamentals/MEMORY.md) · [`lexique/SIZEOF.md`](../lexique/SIZEOF.md) · [`lexique/INLINE.md`](../lexique/INLINE.md)

---

## 🧠 L'état d'esprit

Les cinq piliers constituent *ce qu'il faut* apprendre. Tout aussi important est *la façon* de penser en codant.

### a) La durée de vie d'abord, le comportement ensuite
Avant de vous demander *« que fait cette class ? »*, demandez-vous *« combien de temps vit-elle, qui la possède, comment meurt-elle ? »*. La plupart des bugs en C++ sont des bugs de durée de vie déguisés.

### b) Rester sceptique face à la magie
Rien ne se produit automatiquement. Si un constructeur s'est exécuté, *c'est que le langage l'a appelé*. Si de la mémoire a été libérée, *un destructeur ou un `delete` l'a fait*. Retracez-le. Le compiler est déterministe.
### c) Le compiler comme collaborateur
`-Wall -Wextra -Werror` n'est pas négociable. Un warning est une aide gratuite offerte par le compiler. Les flags stricts de 42 existent pour une bonne raison — ils interceptent des bugs que le langage seul ne verrait pas.

### d) Cppreference, pas Stack Overflow
[en.cppreference.com](https://en.cppreference.com) est le visage lisible par l'humain de la norme. Quand vous vous demandez *"que retourne `std::string::find` si rien n'est trouvé ?"* — allez là-bas. SO est souvent obsolète, souvent faux, souvent orienté C++11+.

### e) Lisez votre propre assembly, au moins une fois
Compilez avec `c++ -O2 -S yourfile.cpp -o -`. Vous n'avez pas besoin de le comprendre en profondeur, mais voir votre boucle `for` devenir 3 instructions pendant qu'un appel virtual devient un `jmp *(rdi)` développe une intuition qu'aucune lecture ne remplace.

### f) Patient avec vous-même, impitoyable avec le code
Le C++ est difficile. Le Mod02 vous frustrera. Le Mod04 vous fera dessiner des schémas. L'état d'esprit à adopter : *Je m'attends à être désorienté, je m'attends à des bugs, je vais creuser jusqu'à comprendre.* Pas *"pourquoi est-ce si dur, ce langage est cassé."* Le langage n'est pas cassé — c'est un outil précis aux arêtes tranchantes.

### g) Construisez petit, construisez souvent, valgrind toujours
1. Écrivez 20 lignes.
2. `make && ./prog`.
3. `valgrind --leak-check=full ./prog`.
4. Corrigez le moindre problème, puis continuez.

N'écrivez jamais 200 lignes en espérant que ça passe. Le C++ punit cela.

---

## 🗺️ Carte des modules — où chaque pilier apparaît

```
Mod00 ─── Pilier 3 (const) introduit via les member functions
       └── Pilier 1 (lifetime) implicite — objets sur la stack uniquement

Mod01 ─── Pilier 1 (RAII) devient explicite — Zombie sur la stack vs heap
       └── Pilier 5 (memory) — premier aperçu du coût de new/delete

Mod02 ─── Pilier 2 (OCF) — tout le module
       ├── Pilier 3 (const) s'approfondit — getRawBits() const
       └── Pilier 5 (sizeof, layout) — Fixed doit être petit

Mod03 ─── Pilier 4 (dynamic polymorphism, partie de base) — virtual + inheritance
       └── Pilier 1 (lifetime) — chaînes de ctor/dtor

Mod04 ─── Pilier 4 (polymorphism complet) — abstract, interface, virtual dtor
       └── Pilier 5 (coût de la vtable) — sizeof inclut maintenant le vptr

Mod05 ─── Piliers 1 + 3 — les exceptions interagissent avec les destructors
       └── try/catch, exception safety

Mod06 ─── Pilier 3 — les casts expriment l'intention de type (static/dynamic/const/reinterpret)

Mod07 ─── Pilier 4 (static polymorphism) — templates
       └── Pilier 5 — le template bloat est réel, apprenez à le gérer

Mod08–09 ─ Piliers 4 + 5 — la STL est la synthèse de tout
```

Quand vous commencez un nouveau module, revenez ici, relisez le pilier correspondant, puis ouvrez le sujet. Le framework d'abord, les détails ensuite.

---

## 📚 Comment utiliser ce document

- **Maintenant (début de piscine)** : parcourez le TL;DR. Lisez attentivement les piliers 1, 2, 3. Surv側にez le 4 et le 5.
- **Avant chaque nouveau module** : relisez le pilier correspondant dans la Carte des modules.
- **Quand vous êtes bloqué** : cherchez dans ce doc le mot-clé sur lequel vous butez. Le principe sous-jacent s'y trouve.
- **Fin de piscine** : relisez l'ensemble. La moitié des concepts qui semblaient abstraits au jour 1 paraîtra désormais évidente.

---

## 🌌 Pensée finale

> Le C++ ne récompense pas l'ingéniosité. Il récompense la **compréhension**.
>
> Le programmeur C++ senior n'est pas celui qui a des astuces — c'est celui qui peut répondre à *"que se passe-t-il, en memory, quand cette ligne s'exécute ?"* sans hésiter.
>
> Les cinq piliers sont le moyen de construire cette compréhension. Tout le reste — STL, exceptions, smart pointers, move semantics — n'est que *l'un de ces piliers portant des vêtements différents*.

Maîtrisez-les. Faites confiance au processus. Le langage se révélera de lui-même.

🔥 *Pèlerin, poursuis ta route.*