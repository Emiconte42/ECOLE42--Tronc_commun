# `using` — Trois rôles différents pour un seul mot-clé

> **TL;DR.** `using` accomplit **trois** tâches distinctes : (1) introduire un nom unique depuis un namespace dans le scope actuel ; (2) introduire le contenu d'un namespace *tout entier* (`using namespace`) ; (3) réexposer un membre de base hérité dans une derived class. L'utilisation de `using` comme typedef en C++11 constitue un quatrième rôle (non disponible en C++98 à 42).

Lié : [`NAMESPACE.md`](NAMESPACE.md) · [`TYPEDEF.md`](TYPEDEF.md) · [`INHERITANCE.md`](../notions/oop/INHERITANCE.md)

---

## 1. Rôle n°1 — `using`-declaration (nom unique)

```cpp
using std::cout;
using std::endl;

cout << "hello" << endl;     // désormais non qualifié
```

Introduit `std::cout` dans le scope actuel. Après la ligne `using`, `cout` et `std::cout` désignent la même chose.

```
  before:                     after:
  ─────────                   ──────
  cout         → not found    cout       → std::cout
  std::cout    → found
```

### Quand l'utiliser

- Dans un `.cpp` pour un ou deux noms très fréquemment utilisés (`cout`, `endl`, `string`).
- Dans le corps d'une fonction pour des raisons pratiques locales.

### Quand NE PAS l'utiliser

- Dans un header. Chaque `.cpp` qui inclut le header hérite de cette introduction, polluant ainsi ses scopes.
- Pour des noms qui entrent en conflit avec le code local (`std::min`, `std::distance` — pièges fréquents).

---

## 2. Rôle n°2 — `using`-directive (namespace complet)

```cpp
using namespace std;

cout << "hi";
string s;
vector<int> v;
```

Introduit **tout** le contenu de `std` dans le scope actuel. Rapide à écrire, long à débugger. Les noms proviennent désormais d'un ensemble beaucoup plus vaste, et les ambiguïtés sont faciles à créer.

### Conseil strict pour 42

**Jamais de `using namespace std;` dans un header.** Chaque TU qui inclut le header subit cette importation massive.

Dans les fichiers `.cpp`, c'est toléré par certaines normes, désapprouvé par d'autres. L'habitude la plus sûre : écrire explicitement `std::`.

```cpp
// 42-clean
std::cout << "..." << std::endl;
std::string s;
std::vector<int> v;
```

---

## 3. Rôle n°3 — réexposer un membre hérité

```cpp
class Base {
public:
    void f(int);
};

class Derived : public Base {
public:
    void f(double);    // masque Base::f(int) par son nom
};

Derived d;
d.f(1.0);              // OK
d.f(42);               // appelle f(double), avec conversion int → double
                        // n'appelle PLUS Base::f(int) — elle est masquée par son nom
```

Ajouter `using` réintroduit `Base::f(int)` dans le scope de Derived :

```cpp
class Derived : public Base {
public:
    using Base::f;     // ramène Base::f dans le namespace de Derived
    void f(double);
};

Derived d;
d.f(42);               // appelle désormais Base::f(int)
d.f(1.0);              // appelle Derived::f(double)
```

Il s'agit de **name lookup**, pas de droits d'accès. La member function resterait inaccessible si elle était `private` — `using` n'accorde aucun accès, il introduit simplement le nom dans le scope afin que la résolution de noms (name lookup) le trouve.

### Cas d'usage : modifier l'accès d'un membre hérité

```cpp
class Base {
public:
    void publicFn();
};

class Derived : private Base {
public:
    using Base::publicFn;     // repasse en public dans Derived
};

Derived d;
d.publicFn();                  // OK — publicFn est désormais public dans Derived
```

C'est une astuce classique pour exposer sélectivement des parties d'une base héritée de manière `private`.

---

## 4. Rôle n°4 — `using` comme alternative à `typedef` (C++11+, PAS en C++98 à 42)

```cpp
using FnPtr  = int (*)(int);                  // C++11 — équivalent à typedef
using IntVec = std::vector<int>;

template <typename T>                         // alias templates — typedef ne peut pas faire cela
using Vec = std::vector<T>;
```

Plus propre que `typedef` pour les function pointers, supporte les templates. En C++98 à 42, vous ne disposez pas de cela ; utilisez `typedef`.

---

## 5. Point de vue du compiler — `using` ne concerne que le name lookup

Les déclarations et directives `using` **ne génèrent pas de code**. Elles modifient les tables de name lookup du compiler pour le scope actuel :

```
   before 'using std::cout;':
       lookup 'cout'  → not found
       lookup 'std::cout' → found

   after 'using std::cout;':
       lookup 'cout'  → resolves via the using-decl to std::cout
       lookup 'std::cout' → still found
```

Le symbole réel dans le binaire reste inchangé. Il n'y a aucun alias au niveau du linker — les deux noms pointent vers le même symbole `_ZSt4cout`.

---

## 6. Astuces et conseils

### 6.1 Hygiène des headers

Ne placez jamais de déclarations ou directives `using` au niveau d'un namespace scope dans un header. Placez-les toujours à l'intérieur de fonctions ou de classes — ou qualifiez simplement les noms explicitement.

### 6.2 Ciblé > directive

```cpp
// à préférer
using std::cout;
using std::endl;

// à éviter
using namespace std;
```

Vous choisissez explicitement les noms que vous utilisez réellement ; les surprises sont réduites au minimum.

### 6.3 Dans les templates, préférez la qualification

```cpp
template <typename T>
void f(T value) {
    std::cout << value;          // explicite — pas de surprise due à un using en amont
}
```

Les templates sont instanciés dans la TU de l'utilisateur ; toutes les déclarations `using` présentes au moment de l'instanciation peuvent affecter le lookup. Soyez explicite.

### 6.4 Réexposer des operators

```cpp
class Base {
public:
    Base& operator=(const Base&);
};

class Derived : public Base {
public:
    using Base::operator=;
    Derived& operator=(const Derived&);
};
```

Sans `using Base::operator=`, l'`operator=` de la derived class masquerait totalement celui hérité. Parfois, vous avez besoin des deux — `using` les réexpose.

### 6.5 Héritage multiniveau

Vous pouvez appliquer `using` à un membre de base à travers plusieurs niveaux :

```cpp
class A { public: void f(); };
class B : public A {};
class C : public B {
public:
    using A::f;          // valide, même si f provient du grand-parent
};
```

### 6.6 Ne faites pas de `using namespace` à l'intérieur d'une class

```cpp
class C {
    using namespace std;     // ERREUR — les directives ne sont pas autorisées au class scope
};
```

`using namespace` ne s'utilise qu'au namespace scope ou au function scope. `using std::cout;` (une using-*declaration*, et non une directive) est autorisée au class scope.

---

## 7. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `'cout' was not declared in this scope` | Aucun `using` et aucun préfixe `std::` | Soit `using std::cout;`, soit `std::cout` |
| Ambiguous overload après `using namespace std;` | Les fonctions de la std entrent en collision avec votre code | Utiliser un `using` ciblé ou qualifier explicitement |
| Fonction de base héritée introuvable dans Derived | Masquée par une fonction du même nom | `using Base::name;` pour réexposer |
| `'using namespace' allowed only at namespace and function scope` | Tentative à l'intérieur d'une class | Passer à la forme `using std::name;` |

---

## 8. Résumé visuel

```
              ┌──────────────────────────────────────────────┐
              │   keyword: using                              │
              └────────────────────┬─────────────────────────┘
                                   │
        ┌─────────────────┬────────┴────────┬────────────────┐
        ▼                 ▼                 ▼                ▼
   using ns::name     using namespace ns   using Base::fn    using A = T;
   (declaration)      (directive)          (re-expose        (C++11 alias)
                                            inherited)
   ─────────────────  ─────────────────    ─────────────     ──────────────
   bring one name     bring all names      undo name         alternative to
   from a namespace   from a namespace     hiding when a     typedef. supports
   into scope.        into scope.          derived class     template aliases.
   targeted —         broad — avoid in     overrides only    not in C++98.
   safer.             headers.             one overload.

   using is name-lookup only. zero runtime cost. zero new symbols.
```

---

## 9. Pratique

1. Pourquoi `using namespace std;` dans un header est-il dangereux ? *(Chaque TU qui inclut le header hérite de cette importation massive → collisions de noms, overloads inattendus.)*
2. Que fait `using Base::f;` dans une class `Derived` qui définit son propre `f` ? *(Réexpose les overloads de la classe de base ; sans cela, le `f` de la derived class masque le `f` de base par son nom.)*
3. Le mot-clé `using` est-il résolu au runtime ? *(Non — c'est purement du name lookup au compile-time.)*
4. En C++98, peut-on créer un alias pour `std::vector<int>` avec `using` ? *(Non — c'est la syntaxe d'alias de C++11. Utilisez `typedef std::vector<int> IntVec;`.)*