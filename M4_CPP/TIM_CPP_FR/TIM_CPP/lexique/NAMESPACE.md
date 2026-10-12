# `namespace` — Boîtes logiques pour les noms

> **TL;DR.** Un namespace est un scope nommé. Tout ce qui y est déclaré doit être qualifié avec `Name::` depuis l'extérieur, empêchant ainsi les collisions entre bibliothèques qui utiliseraient par hasard les mêmes identifiants. Les namespaces peuvent s'imbriquer, avoir des alias et fusionner à travers plusieurs fichiers.

Associé : [`USING.md`](USING.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#6.3%20Namespaces) · [`EXTERN.md`](EXTERN.md)

---

## 1. Le problème résolu par `namespace`

```cpp
// libA.hpp
int log(int x);

// libB.hpp
double log(double x);

// main.cpp
#include "libA.hpp"
#include "libB.hpp"
log(2.0);     // ambigu ? collision ? → casse-tête
```

Deux bibliothèques qui partagent un même nom entrent en conflit. Les namespaces donnent à chaque bibliothèque son propre scope :

```cpp
namespace mathA { int    log(int);    }
namespace mathB { double log(double); }

mathA::log(2);
mathB::log(2.0);
```

Aucune collision. Les noms complets sont `mathA::log` et `mathB::log` — des symboles différents.

---

## 2. Déclaration et utilisation

```cpp
// déclaration
namespace ft {
    int max(int a, int b) { return a > b ? a : b; }

    class Container {
        // ...
    };
}

// utilisation — trois façons
ft::max(1, 2);             // qualification explicite — préférable dans les headers

using ft::max;             // introduit un nom dans le scope
max(1, 2);

using namespace ft;        // introduit tout dans le scope (à éviter dans les headers)
max(1, 2);
Container c;
```

La standard library réside dans `std::`. `std::cout`, `std::string`, `std::cin`, `std::vector<int>`.

---

## 3. Vision du compiler/linker

Un namespace ajoute son nom au **symbole manglé** de tout ce qu'il contient.

```cpp
namespace ft { void f(); }
namespace gh { void f(); }
```

```
   mangled (Itanium ABI):
   ft::f()  →  _ZN2ft1fEv
   gh::f()  →  _ZN2gh1fEv
```

Deux symboles distincts, aucune collision lors du link. Inspectez :

```bash
$ nm prog | grep ' f'
0000000000401120 T _ZN2ft1fEv
0000000000401140 T _ZN2gh1fEv
```

Sans namespaces, les deux généreraient le mangling `_Z1fv` et le linker refuserait l'opération.

---

## 4. Namespaces imbriqués

```cpp
namespace lib {
    namespace internal {
        int helper();
    }
    int  api();
}

lib::internal::helper();      // explicite
lib::api();
```

La syntaxe C++98 impose une imbrication explicite (un `namespace` par niveau). C++17 permet `namespace lib::internal { ... }`. Pas à 42.

---

## 5. Les directives `using`

### 5.1 Ciblée — introduire un seul nom

```cpp
using std::cout;
cout << "hello\n";        // non qualifié
```

C'est tout à fait acceptable dans les fichiers `.cpp` lorsqu'utilisé judicieusement.

### 5.2 Globale — introduire tous les noms

```cpp
using namespace std;       // déverse tout depuis std dans le scope actuel
cout << "hello\n";
string s;
vector<int> v;
```

**Ne mettez jamais `using namespace std;` dans un header.** Chaque TU qui inclut le header hérite de ce déversement → collisions, mauvaises surprises de masquage de noms (name-shadowing), cassures lorsque la standard library ajoute de nouveaux noms.

Préférence de la norme 42 : écrivez `std::` explicitement. Cela ne représente que trois caractères de plus et évite des heures de débogage.

```cpp
// style 42 :
std::cout << contact.getName() << '\n';
```

### 5.3 À l'intérieur des fonctions, `using` convient très bien

```cpp
void process() {
    using std::cout;
    cout << "...";        // scope local ; pas de pollution globale
}
```

---

## 6. Alias de namespace

```cpp
namespace longLibraryName {
    int doStuff();
}

namespace ll = longLibraryName;
ll::doStuff();
```

Un alias court permet de conserver des qualifications lisibles sans avoir à tout importer dans le scope.

---

## 7. Le namespace anonyme — scope local au fichier

```cpp
namespace {
    int helper() { return 42; }      // visible uniquement dans cette TU
    int counter = 0;
}
```

Équivalent à `static` pour les fonctions libres et les variables — linkage interne. L'alternative idiomatique C++ au `static` de scope fichier.

```
   namespace ___unnamed_<TU-id> { … }
                ↑
       le compiler invente un nom unique par TU,
       puis effectue implicitement 'using namespace ___unnamed_<TU-id>;'
```

Contrairement à `static`, les namespaces anonymes fonctionnent également pour les **types** (vous pouvez placer une class à l'intérieur pour rendre la class privée à la TU).

---

## 8. Les namespaces sont ouverts — vous pouvez les enrichir

```cpp
// dans math.hpp
namespace ft {
    int abs(int);
}

// dans un autre header, math_extra.hpp
namespace ft {
    int gcd(int, int);
}
```

Les deux contribuent à `ft`. Le compiler les fusionne. Cela permet à une bibliothèque de répartir ses symboles sur de nombreux fichiers sans vous forcer à tout placer dans un seul bloc `namespace { ... }`.

```
   header1: namespace ft { abs }
   header2: namespace ft { gcd }
   
   final ft = { abs, gcd }
```

---

## 9. Argument-dependent lookup (ADL)

Une fonctionnalité subtile mais utile : si vous appelez une fonction avec des arguments de types appartenant au namespace N, le compiler recherche également dans le namespace N les overloads correspondantes.

```cpp
namespace ft {
    struct Vec { int x, y; };
    std::ostream& operator<<(std::ostream&, const Vec&);
}

int main() {
    ft::Vec v;
    std::cout << v;       // fonctionne — l'ADL trouve ft::operator<<
}
```

L'ADL est la raison pour laquelle `std::cout << "string"` fonctionne — `operator<<(std::ostream&, const char*)` réside dans `std`, et le type de `std::cout` déclenche la recherche. Sans l'ADL, vous devriez écrire `std::operator<<(std::cout, "string")` partout.

---

## 10. Conseils & astuces

### 10.1 Toujours préfixer par `std::` selon la norme 42

```cpp
std::cout << "..." << std::endl;
std::string s;
std::vector<int> v;
```

Oui, c'est verbeux. Oui, cela rend le code facile à chercher avec grep et sans ambiguïté.

### 10.2 Utiliser un namespace de projet

Pour les projets plus importants, encapsulez votre code dans votre propre namespace :

```cpp
namespace mylib {
    class Foo { /* ... */ };
    void do_thing();
}
```

Cela vous procure une barrière nette face aux bibliothèques tierces.

### 10.3 Ne pas trop imbriquer

```cpp
namespace company { namespace product { namespace module { namespace impl {
    /* code profondément imbriqué */
}}}}
```

Une profondeur de trois niveaux est largement suffisante. Au-delà, les qualifications deviennent illisibles.

### 10.4 Namespace anonyme > static de scope fichier pour le code non trivial

```cpp
// approche C++ moderne :
namespace { void helper(); }

// ancienne approche C :
static void helper();
```

Les deux fonctionnent ; le namespace anonyme est préféré et fonctionne également pour les types.

### 10.5 Alias pour les instanciations de template longues

```cpp
namespace utils {
    typedef std::map<std::string, std::vector<int> > NamedSeries;
}

utils::NamedSeries data;
```

Notez l'espace dans `> >` — requis en C++98 (sinon `>>` est analysé comme l'opérateur de décalage à droite). C++11+ supprime cette contrainte.

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `'foo' was not declared in this scope` | `namespace::` ou `using` oublié | Qualifier : `ns::foo()` ou ajouter `using ns::foo;` |
| Overload ambiguë | Plusieurs `using namespace` ont importé le même nom | Utiliser un `using` ciblé à la place |
| Header pollution | `using namespace std;` dans un header | Le retirer ; qualifier explicitement |
| Surprise d'ADL — mauvais `operator<<` sélectionné | Plusieurs namespaces définissent le même opérateur | Bien ancrer les types ; envisager une qualification explicite |

---

## 12. Résumé visuel

```
                ┌────────────────────────────────────────┐
                │  namespace ns { … }                     │
                │                                          │
                │  scope nommé. les noms internes ont      │
                │  besoin de la qualification ns:: depuis  │
                │  l'extérieur.                            │
                └────────────────────┬─────────────────────┘
                                     │
            ┌────────────────────────┼────────────────────────┐
            ▼                        ▼                        ▼
     utilisation explicite     using ns::name           using namespace ns
       ns::foo()                importe un seul nom      importe tous les noms
       (préféré,                (ok dans les .cpp,       (JAMAIS dans les
        surtout dans les        ciblé)                   headers ; avec
        headers)                                         parcimonie dans les .cpp)

       ouvert : plusieurs fichiers contribuent à un même namespace.
       imbriqué : namespace a::b — écrire 'a::b::name'.
       alias : namespace court = long;
       anonyme : local au fichier — remplace 'static' pour les symboles.

       std::  est le namespace de la standard library.
       ADL :   le compiler cherche aussi dans le namespace d'un
              argument pour trouver les overloads correspondantes.
```

---

## 13. Pratique

1. Pourquoi `using namespace std;` dans un header est-il une mauvaise idée ? *(Cela pollue chaque TU qui inclut le header ; cela peut entrer en collision avec les noms de l'utilisateur ; cela peut casser lorsque std ajoute de nouveaux noms.)*
2. Quelle est la différence entre une fonction `static` de scope fichier et une fonction dans un namespace anonyme ? *(Fonctionnellement équivalent ; le namespace anonyme est l'idiome C++ et fonctionne aussi pour les types.)*
3. Qu'est-ce que l'ADL ? *(Argument-dependent lookup — lorsque vous appelez `f(x)` avec `x` d'un type appartenant au namespace N, le compiler recherche aussi `f` dans N.)*
4. Pourquoi `std::vector<std::vector<int>>` est-il invalide en C++98 mais `std::vector<std::vector<int> >` (avec l'espace) valide ? *(C++98 analyse `>>` comme l'opérateur de décalage à droite ; l'espace lève l'ambiguïté. C++11 corrige le parser.)*