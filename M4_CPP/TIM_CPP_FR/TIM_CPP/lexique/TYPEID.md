# `typeid` — Demander au Runtime : « Quel est le type de cet objet ? »

> **TL;DR.** `typeid(expr)` renvoie une reference `std::type_info` identifiant le type. Pour les types polymorphiques accessibles via un pointer/reference de base, la reponse est le **type dynamique**. Pour tout le reste, c'est le **type statique** calcule a la compilation.

En lien : [`VIRTUAL.md`](VIRTUAL.md) · [`CASTS.md`](CASTS.md) · [`POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md)

---

## 1. La forme

```cpp
#include <typeinfo>

int x = 0;
const std::type_info& ti = typeid(x);
std::cout << ti.name();              // chaine definie par l'implementation, ex: "i" pour int

std::cout << (typeid(x) == typeid(int));     // 1 (true)
std::cout << (typeid(x) == typeid(double));  // 0 (false)
```

Faites toujours `#include <typeinfo>`. Le resultat est une reference vers un objet `std::type_info` — comparable, mais non copiable, non constructible.

---

## 2. Type statique vs dynamique

```cpp
class Animal { public: virtual ~Animal() {} };
class Dog : public Animal {};

Animal *p = new Dog();
typeid(*p);                  // typeid de Dog — type DYNAMIQUE, car Animal est polymorphique
typeid(p);                   // typeid de Animal* — type STATIQUE du pointer

Animal a;
typeid(a);                   // typeid de Animal — DYNAMIQUE == STATIQUE

Animal &r = *p;
typeid(r);                   // typeid de Dog — DYNAMIQUE pour une ref polymorphique
```

Regle :

```
   si expr est une glvalue (lvalue ou non-temporaire) d'un type de class polymorphique
       → type dynamique (regarde le vptr au runtime)
   sinon
       → type statique (calcule a la compilation)
```

**Class polymorphique** = une class avec au moins une fonction virtual. Si votre class n'a aucun virtual, `typeid` donne toujours le type statique.

---

## 3. Exemple concret

```cpp
class Shape {
public:
    virtual ~Shape() {}
};
class Circle    : public Shape {};
class Square    : public Shape {};

Shape *shapes[3];
shapes[0] = new Circle();
shapes[1] = new Square();
shapes[2] = new Shape();

for (int i = 0; i < 3; i++) {
    if      (typeid(*shapes[i]) == typeid(Circle))  std::cout << "circle\n";
    else if (typeid(*shapes[i]) == typeid(Square))  std::cout << "square\n";
    else                                             std::cout << "shape\n";
}
```

Sortie :
```
circle
square
shape
```

`typeid(*shapes[i])` passe par le vptr pour trouver le type dynamique. Avec une class Shape non polymorphique, les trois afficheraient « shape ».

---

## 4. Vision materiel/runtime — la vtable transporte le `type_info`

Pour les classes polymorphiques, le compiler attache un pointer vers le `std::type_info` du type *a la vtable* :

```
   vtable de Circle :
   ┌──────────────────────────┐
   │  &Circle's type_info     │  ← typeid(circle) lit ceci
   │  &Circle::~Circle        │
   │  &Circle::draw           │
   │  ...                     │
   └──────────────────────────┘

   vtable de Square :
   ┌──────────────────────────┐
   │  &Square's type_info     │
   │  &Square::~Square        │
   │  ...                     │
   └──────────────────────────┘
```

Ainsi, `typeid(*p)` pour un `p` polymorphique effectue :

```
   1. Chargement du vptr depuis *p (une lecture memoire).
   2. Lecture du pointer type_info depuis la vtable (une lecture memoire).
   3. Renvoi de la reference vers le type_info.
```

Pour les types non polymorphiques, `typeid(x)` est purement a la compilation — le compiler connait le type statique et insere une reference vers un objet `type_info` fixe.

L'objet `type_info` lui-meme est une variable globale, construite au demarrage du programme, avec une adresse unique par type. C'est pourquoi `==` entre deux references `type_info` est une simple comparaison de pointer — tres rapide.

---

## 5. `name()` — defini par l'implementation

```cpp
typeid(int).name();              // "i" (gcc/clang Itanium ABI), "int" (MSVC)
typeid(std::string).name();      // une longue chaine mangled sur gcc
```

`name()` renvoie un `const char*` choisi par l'implementation — generalement le nom mangle. Pour une forme lisible par un humain sur gcc/clang, utilisez `c++filt` :

```bash
$ ./prog | c++filt -t
```

Ou `abi::__cxa_demangle` par programmation. Non portable, non standardise.

N'utilisez `name()` que pour le debogage. Pour la comparaison de types, utilisez l'`operator==` :

```cpp
if (typeid(x) == typeid(int)) { ... }
```

---

## 6. `typeid` et les exceptions

`typeid` peut lever deux exceptions :

### 6.1 `std::bad_typeid` — dereferencement de pointer nul

```cpp
Shape *p = 0;
typeid(*p);              // leve std::bad_typeid (uniquement quand *p est polymorphique)
```

Si l'operande est un dereferencement polymorphique d'un pointer nul, vous obtenez `std::bad_typeid`. Pour les types non polymorphiques, `typeid(*p)` ne dereference pas reellement (c'est une resolution statique) — mais le standard indique tout de meme que le comportement est indefini ; en pratique, la plupart des compilers ne levent rien.

### 6.2 `std::bad_cast`

Ne provient pas directement de `typeid` — mais de `dynamic_cast`. Voir [`CASTS.md`](CASTS.md).

---

## 7. `typeid` vs `dynamic_cast`

```cpp
Animal *p = ...;
if (typeid(*p) == typeid(Dog)) {
    // c'est EXACTEMENT un Dog
    Dog *d = static_cast<Dog*>(p);
}

if (Dog *d = dynamic_cast<Dog*>(p)) {
    // c'est un Dog ou quelque chose qui derive de Dog
}
```

- `typeid` verifie l'egalite **exacte** de type.
- `dynamic_cast` verifie la **convertibilite** (fonctionne aussi pour les classes derivees).

Utilisez `dynamic_cast` pour demander « est-ce un Dog ou n'importe quoi qui derive de Dog ? ». Utilisez `typeid` pour demander « est-ce *exactement* un Dog ? ».

En pratique : `dynamic_cast` est l'outil privilegie. L'utilisation de `typeid` pour l'identification de type est plus rare.

---

## 8. Astuces et conseils

### 8.1 Toujours inclure `<typeinfo>`

Oublier l'include est la cause n°1 des erreurs « `'type_info' is not a member of 'std'` ».

### 8.2 Utilisez-le pour le debogage, pas pour le flux de controle

```cpp
// debogage :
std::cout << "got a: " << typeid(*ptr).name() << '\n';

// flux de controle :
if (typeid(*ptr) == typeid(Dog)) { ... }     // fragile — on ajoute de nouveaux types et on oublie de mettre a jour
```

Si vous faites des branchements conditionnels selon le type, demandez-vous si une fonction virtual ne ferait pas mieux le travail.

### 8.3 Le RTTI peut etre desactive — soyez vigilant

g++ possede `-fno-rtti` pour eviter d'emettre les type_info. Certaines bases de code embarquees font cela pour reduire la taille. Si le RTTI est desactive, `typeid` et `dynamic_cast` peuvent ne pas fonctionner ou donner des informations limitees. Les builds standards de 42 conservent le RTTI active.

### 8.4 Comparaison a travers differentes unites de traduction

Les objets `type_info` sont uniques par type **au sein d'un programme** — mais si votre programme charge des bibliotheques dynamiques (.so, .dll), chaque bibliotheque peut avoir sa propre copie du meme `type_info`. Comparer par pointer peut donner de faux negatifs. Le standard fournit `before()` pour l'ordonnancement et recommande de comparer via `==`/`!=`, ce que l'implementation doit faire fonctionner a travers les bibliotheques sur la plupart des plateformes.

Pour les modules de 42 (binaire unique, pas de dlopen), cela ne s'applique pas.

### 8.5 type_info n'est pas copiable

```cpp
std::type_info ti = typeid(int);    // ERREUR
std::type_info &ref = typeid(int);  // OK — liaison d'une reference
```

Stockez-le toujours par reference (ou pointer vers const), jamais par valeur.

---

## 9. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `'type_info' is not a member of 'std'` | `#include <typeinfo>` manquant | Ajouter l'include |
| `typeid(*p)` a renvoye le type statique | La class n'a aucune fonction virtual | Ajouter un destructeur virtual (ou n'importe quelle fonction virtual) — requis pour la rendre polymorphique |
| `name()` surprenant comme `"i"` au lieu de `"int"` | Defini par l'implementation ; gcc/clang utilisent des noms mangled | Demangler pour l'affichage uniquement ; utiliser `==` pour comparer |
| `bad_typeid` levee | Pointer nul dereference dans un typeid polymorphique | Verifier la nullite au prealable |

---

## 10. Resume visuel

```
              ┌──────────────────────────────────────────────┐
              │    typeid(expr)                              │
              │      → const std::type_info&                  │
              │                                                │
              │    glvalue polymorphique → type dynamique     │
              │    tout le reste         → type statique      │
              └────────────────────┬───────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────────┐
              ▼                    ▼                         ▼
       comparaison ==          .name()                   stockage
       ──────────────          ────────                   ────────
       comparaison de          chaine definie par        les objets type_info
       pointer rapide.         l'implementation          resident en memoire
       s'utilise pour :        (mangled sur gcc).        statique par type.
       "est-ce exactement      debogage uniquement.      la vtable transporte
       le type X ?"                                      le pointer pour les
                                                          types polymorphiques.

       doit inclure <typeinfo>.
       comparer les pointers via reference : const std::type_info&.
       leve bad_typeid lors du dereferencement polymorphique d'un pointer nul.
```

---

## 11. Pratique

1. Pourquoi `typeid(*p)` pour `Animal *p = new Dog();` vaut-il Dog quand Animal a un destructeur virtual, mais Animal sinon ? *(Les types polymorphiques utilisent le vptr pour trouver le type dynamique a l'execution ; les types non polymorphiques utilisent le type statique connu a la compilation.)*
2. Que renvoie `typeid(int).name()` ? *(Defini par l'implementation — sur gcc/clang c'est `"i"` ; sur MSVC c'est `"int"`. Ne vous fiez pas au format.)*
3. Quand est-ce que `typeid` est moins utile que `dynamic_cast` ? *(Lorsque vous voulez savoir « est-ce un Dog *ou une quelconque sous-classe de Dog* » plutot que « est-ce exactement un Dog ».)*
4. Pourquoi le resultat de `typeid` est-il renvoye sous forme de *reference* ? *(`std::type_info` n'est pas copiable ; un unique objet `type_info` existe par type et est reference depuis plusieurs endroits.)*