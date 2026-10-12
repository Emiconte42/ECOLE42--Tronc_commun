# La STL — Guide complet

La **Standard Template Library** est la collection de structures de données et d'algorithmes prêts à l'emploi, génériques et efficaces de la bibliothèque standard C++. Apprenez-la une fois et vous n'aurez plus jamais à réimplémenter une linked list ou un sort à la main.

La STL repose sur trois piliers :

```
    CONTAINERS         ITERATORS           ALGORITHMS
    (store data)  ◄──►  (walk data)  ◄──►  (process data)

    vector              begin/end            sort
    list                ++/--                find
    map                  *                   copy
    stack               -> and .             accumulate
    ...                 ...                  ...
```

Le coup de génie est que les algorithmes ne savent pas sur quel container ils travaillent — ils communiquent avec des **iterators**, que chaque container fournit. Un seul `sort` fonctionne sur chaque container dont les iterators sont à random-access. Un seul `find` fonctionne sur presque tout.

---

## Table des matières

1. [La philosophie](#1.%20The%20philosophy)
2. [Vue d'ensemble des containers](#2.%20Containers%20overview)
3. [Sequence containers](#3.%20Sequence%20containers)
4. [Container adapters](#4.%20Container%20adapters)
5. [Associative containers](#5.%20Associative%20containers)
6. [Iterators](#6.%20Iterators)
7. [Catégories d'iterators](#7.%20Iterator%20categories)
8. [Algorithms](#8.%20Algorithms)
9. [`std::pair` et `std::make_pair`](#9.%20std%3A%3Apair%20and%20std%3A%3Amake_pair)
10. [Functors — la réponse de C++98 aux lambdas](#10.%20Functors%20%E2%80%94%20C%2B%2B98%27s%20answer%20to%20lambdas)
11. [Dans CPP08 — iterators + containers avec template](#11.%20In%20CPP08%20%E2%80%94%20iterators%20%2B%20templated%20containers)
12. [Dans CPP09 — choisir le bon container](#12.%20In%20CPP09%20%E2%80%94%20picking%20the%20right%20container)
13. [Aide-mémoire : quel container pour quel usage](#13.%20Cheatsheet%3A%20which%20container%20when)
14. [Règles empiriques](#14.%20Rules%20of%20thumb)
15. [Pièges courants](#15.%20Gotchas)

---

## 1. The philosophy

La STL sépare trois concepts que les langages confondent généralement :

- **Ce que vous stockez** — le container.
- **La façon dont vous le parcourez** — l'iterator.
- **Ce que vous en faites** — l'algorithm.

Un `std::sort` se moque de savoir s'il trie un array, un `vector` ou un `deque`. Il appelle `*it`, `++it`, `swap`. Tout iterator qui supporte ces opérations fonctionne. C'est l'unité de composition.

```cpp
std::vector<int> v; v.push_back(3); v.push_back(1); v.push_back(2);
std::sort(v.begin(), v.end());       // 1, 2, 3

int arr[] = {3, 1, 2};
std::sort(arr, arr + 3);              // les raw pointers sont aussi des iterators
```

C'est pourquoi les templates sont essentiels : la STL fonctionne parce que chaque container expose des types et des opérations sur lesquels les algorithms peuvent compter. Voir [TEMPLATES.md](TEMPLATES.md).

---

## 2. Containers overview

Trois familles :

| Famille | Exemples | Accès par |
|---|---|---|
| **Sequence** | [`vector`](../containers/VECTOR.md), [`list`](../containers/LIST.md), [`deque`](../containers/DEQUE.md), [`string`](../io-errors/STRING_FUNCTIONS.md) | Position (index, iterator) |
| **Container adapter** | [`stack`](../containers/STACK.md), [`queue`](../containers/QUEUE.md), [`priority_queue`](../containers/PRIORITY_QUEUE.md) | Opérations restreintes sur une séquence |
| **Associative** | [`set`](../containers/SET.md), [`map`](../containers/MAP.md), [`multiset`](../containers/MULTISET.md), [`multimap`](../containers/MULTIMAP.md) | Clé |

> **Besoin d'approfondir ?** Cette page est une vue d'ensemble de haut niveau. Chaque container ci-dessus renvoie à une analyse approfondie dédiée dans [`cpp/containers/`](../containers/INDEX.md) — API complète, tableau de complexité, règles d'invalidation des iterators, conseils d'efficacité et exemples pratiques par container.

Tous nécessitent :
```cpp
#include <vector>     // et ainsi de suite — un header par container
```

---

## 3. Sequence containers

### `std::vector<T>` — le choix par défaut

Un tableau dynamique (dynamic array). Mémoire contiguë, random access rapide, push_back en O(1) amorti.

```cpp
#include <vector>

std::vector<int> v;
v.push_back(10);
v.push_back(20);
v[0];                            // 10 — O(1)
v.size();                        // 2
v.begin(); v.end();              // iterators

std::vector<int> w(5, 42);       // [42, 42, 42, 42, 42]
std::vector<int> x(v);           // copie
std::vector<int> y(v.begin(), v.end());   // copie par plage
```

**Quand l'utiliser :** votre choix par défaut. Rapide, cache-friendly, simple.

Opérations clés :

| Opération | Coût |
|---|---|
| `v[i]` | O(1) |
| `v.push_back(x)` | O(1) amorti |
| `v.pop_back()` | O(1) |
| `v.insert(it, x)` | O(n) — décalages |
| `v.erase(it)` | O(n) — décalages |
| `v.size()`, `v.empty()` | O(1) |

**Piège :** l'insertion/suppression invalide les iterators (et parfois tous les iterators, si la capacité doit augmenter).

### `std::list<T>` — doubly-linked list

Insertion/suppression rapides n'importe où, pas de random access.

```cpp
#include <list>

std::list<int> l;
l.push_back(1);
l.push_front(0);
l.size();                        // 2
// l[0];                         // NON — list n'a pas d'operator[]

std::list<int>::iterator it = l.begin();
++it;
l.insert(it, 99);                // insère avant la position — O(1)
l.erase(it);                     // O(1)
```

**Quand l'utiliser :** nombreuses insertions/suppressions au milieu ; vous n'avez pas besoin de random access ; vous avez besoin de la stabilité des iterators/references après insertion.

### `std::deque<T>` — double-ended queue

Accessible en random access comme un vector, mais push/pop aux deux extrémités est en O(1). Implémenté sous forme d'une séquence de blocs — *non* contiguë.

```cpp
#include <deque>

std::deque<int> d;
d.push_back(1);
d.push_front(0);
d[0];                            // 0 — O(1)
```

**Quand l'utiliser :** vous avez besoin de push/pop rapides aux deux extrémités **et** d'un random access. Sinon, vector.

### `std::string`

Techniquement un sequence container (il s'agit de `std::basic_string<char>`). Voir [STRING_FUNCTIONS.md](../io-errors/STRING_FUNCTIONS.md).

---

## 4. Container adapters

Ceux-ci **encapsulent** un sequence container et exposent une interface restreinte.

### `std::stack<T>` — LIFO

```cpp
#include <stack>

std::stack<int> s;
s.push(1);
s.push(2);
s.top();                         // 2
s.pop();                         // supprime le sommet ; renvoie void (PAS l'élément !)
s.size();
s.empty();
```

Pas d'iterators. Pas de random access. `pop` ne renvoie rien — vous appelez d'abord `top()`, puis `pop()`.

**Container sous-jacent par défaut :** `std::deque`. Vous pouvez le changer :
```cpp
std::stack<int, std::vector<int> > sv;
```

### `std::queue<T>` — FIFO

```cpp
#include <queue>

std::queue<int> q;
q.push(1);
q.push(2);
q.front();                       // 1 — le plus ancien
q.back();                        // 2 — le plus récent
q.pop();                         // supprime l'élément de tête
```

### `std::priority_queue<T>` — max-heap par défaut

```cpp
#include <queue>

std::priority_queue<int> pq;
pq.push(3);
pq.push(1);
pq.push(5);
pq.top();                        // 5 — le plus grand en premier
pq.pop();
```

Min-heap : passez `std::greater<T>` comme comparateur (voir §10).

---

## 5. Associative containers

Ceux-ci maintiennent les éléments **triés par clé** (en utilisant `<` par défaut) et offrent find/insert/erase en O(log n). Implémentés sous forme de BST équilibrés (arbres rouge-noir, généralement).

### `std::set<T>` — valeurs uniques et triées

```cpp
#include <set>

std::set<int> s;
s.insert(3);
s.insert(1);
s.insert(3);                     // ignoré — déjà présent
// L'itération parcourt dans l'ordre trié :
for (std::set<int>::iterator it = s.begin(); it != s.end(); ++it)
    std::cout << *it << ' ';     // "1 3"

s.find(3);                       // iterator vers 3, ou s.end() si absent
s.count(3);                      // 0 ou 1
s.erase(3);                      // par valeur
```

### `std::map<Key, Value>` — clés uniques, triées par clé

```cpp
#include <map>

std::map<std::string, int> ages;
ages["Alice"] = 30;
ages["Bob"]   = 25;

std::map<std::string, int>::iterator it = ages.find("Alice");
if (it != ages.end())
    std::cout << it->first << " = " << it->second << '\n';

// L'itération parcourt les clés dans l'ordre trié
for (std::map<std::string, int>::iterator it = ages.begin(); it != ages.end(); ++it)
    std::cout << it->first << " = " << it->second << '\n';
```

**Le type d'élément est `std::pair<const Key, Value>`.** C'est pourquoi vous utilisez `it->first` (clé) et `it->second` (valeur).

**Piège :** `ages["nobody"]` **insère une entrée construite par défaut** si la clé n'est pas présente. Utilisez `find` si vous voulez éviter cela.

### `std::multiset<T>` et `std::multimap<Key, Value>`

Comme `set` / `map`, mais les doublons sont autorisés.

```cpp
std::multiset<int> ms;
ms.insert(1); ms.insert(1); ms.insert(2);
ms.count(1);                     // 2
```

---

## 6. Iterators

Un iterator est un pointeur généralisé (« generalized pointer »). Il répond aux mêmes opérations qu'un pointer : `*it`, `++it`, `it1 == it2`.

Chaque container possède ces types imbriqués :
```cpp
std::vector<int>::iterator       it;
std::vector<int>::const_iterator cit;
std::vector<int>::reverse_iterator rit;
```

Chaque container expose :
```cpp
c.begin();       // iterator vers le premier élément
c.end();         // iterator vers « un après le dernier » — PAS un élément valide
c.rbegin();      // reverse iterator démarrant au dernier élément
c.rend();        // reverse iterator à « un avant le premier »
```
### La boucle canonique

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it << '\n';
```

- `!=` et non `<` — de nombreuses catégories d'iterators ne prennent pas en charge `<`.
- `++it` et non `it++` — le préfixe évite une copie inutile.
- `*it` déréférence vers l'élément. `it->member` fonctionne lorsque l'élément est une class.

### Iterator ou pointer ?

Les raw pointers **sont** des iterators pour les tableaux C :
```cpp
int arr[] = {3, 1, 2};
std::sort(arr, arr + 3);          // arr et arr+3 sont des iterators
```

Le `.data()` de n'importe quel container (C++11) vous donne un pointer, mais en C++98, itérez avec `begin()`/`end()`.

---

## 7. Catégories d'iterators

Tous les iterators ne prennent pas en charge toutes les opérations. Il existe cinq **catégories**, chacune étant un sur-ensemble de la précédente :

```
InputIterator        ── lecture seule unique, ++
   │
OutputIterator       ── écriture seule unique, ++
   │
ForwardIterator      ── lecture/écriture, ++ (list, set)
   │
BidirectionalIterator ── ++ et --          (list, map, set)
   │
RandomAccessIterator ── +n, -n, [], <       (vector, deque, raw pointers)
```

Les algorithmes spécifient la catégorie dont ils ont besoin. `std::sort` nécessite un accès aléatoire (random-access) ; c'est pourquoi vous ne pouvez pas appliquer `std::sort` à un `std::list`. (`list` possède sa propre fonction membre `.sort()` exactement pour cette raison.)

---

## 8. Algorithmes

`#include <algorithm>` et `#include <numeric>`. Les algorithmes prennent des plages d'iterators `[first, last)` — `last` étant un élément après la fin, tout comme `end()`.

> **Besoin de la liste complète ?** Ceci est la liste restreinte. Le catalogue C++98 **exhaustif** — chaque `<algorithm>` regroupé par fonction, plus `<numeric>`, `<functional>`, ainsi qu'une matrice des fonctions membres inter-containers — se trouve dans [`ALGORITHMS.md`](ALGORITHMS.md).

### Ceux que vous utiliserez réellement

| Algorithme | Ce qu'il fait |
|---|---|
| `std::sort(first, last)` | Trie sur place (random-access) |
| `std::sort(first, last, cmp)` | Trie avec un comparateur personnalisé |
| `std::find(first, last, val)` | Trouve la première occurrence ; retourne un iterator ou `last` |
| `std::find_if(first, last, pred)` | Trouve le premier élément correspondant au prédicat |
| `std::count(first, last, val)` | Compte les occurrences |
| `std::copy(first, last, dest)` | Copie la plage vers un iterator de sortie |
| `std::fill(first, last, val)` | Remplit la plage avec une valeur |
| `std::transform(first, last, dest, fn)` | Applique `fn` à chaque élément ; écrit les résultats dans `dest` |
| `std::for_each(first, last, fn)` | Applique `fn` à chaque élément (résultat ignoré) |
| `std::accumulate(first, last, init)` | Somme (ou réduction avec une opération binaire personnalisée) — dans `<numeric>` |
| `std::min_element(first, last)` | Iterator vers le plus petit élément |
| `std::max_element(first, last)` | Iterator vers le plus grand élément |
| `std::reverse(first, last)` | Inverse sur place |
| `std::unique(first, last)` | Supprime les doublons consécutifs ; retourne la nouvelle fin |
| `std::remove_if(first, last, pred)` | Déplace les éléments non correspondants au début ; retourne la nouvelle fin |

### L'idiome erase-remove

`remove` et `remove_if` ne suppriment pas réellement — ils **décalent** les éléments conservés vers l'avant et retournent la nouvelle fin logique. Vous devez enchaîner avec `erase` :

```cpp
v.erase(std::remove_if(v.begin(), v.end(), is_odd), v.end());
```

Cela devient plus simple après l'avoir vu trois fois. D'ici là, c'est déroutant.

---

## 9. `std::pair` et `std::make_pair`

Un `std::pair<T1, T2>` contient deux valeurs. Les éléments d'une `map` sont des paires.

```cpp
#include <utility>

std::pair<std::string, int> p("Alice", 30);
p.first;                         // "Alice"
p.second;                        // 30

std::pair<std::string, int> q = std::make_pair("Bob", 25);   // types déduits
```

**`make_pair` déduit les deux types**, vous évitant de les écrire deux fois. C'est la raison pour laquelle elle existe.

---

## 10. Foncteurs — la réponse de C++98 aux lambdas

Pas de lambdas en C++98. L'alternative : les **foncteurs** — des classes avec `operator()` :

```cpp
struct IsEven {
    bool operator()(int n) const { return n % 2 == 0; }
};

std::vector<int> evens;
std::copy_if(v.begin(), v.end(), std::back_inserter(evens), IsEven());
//                                                           ^^^^^^^
//                                               instanciation, passage comme callable
```

(`copy_if` est en C++11 ; en C++98, utilisez `std::remove_copy_if` avec la négation.)

### Foncteurs intégrés

`<functional>` en propose beaucoup : `std::less<T>`, `std::greater<T>`, `std::plus<T>`, `std::equal_to<T>`, ...

```cpp
std::sort(v.begin(), v.end(), std::greater<int>());   // tri par ordre décroissant
```

### Foncteurs avec état

Contrairement aux simples function pointers, les foncteurs peuvent transporter des données :

```cpp
struct GreaterThan {
    int threshold;
    GreaterThan(int t) : threshold(t) {}
    bool operator()(int n) const { return n > threshold; }
};

std::count_if(v.begin(), v.end(), GreaterThan(10));   // combien sont > 10
```

C'est exactement ce que font les lambdas en C++11 — capturer l'état dans un foncteur anonyme. Mais vous êtes en C++98, donc vous écrivez la class.

---

## 11. Dans CPP08 — iterators + containers template

- **ex00 — `easyfind`** — une fonction template `easyfind(container, value)` qui retourne un iterator vers la première occurrence, ou lève une exception s'il est absent. Fonctionne sur tout container fournissant `begin()`, `end()`, et `iterator`. C'est votre premier aperçu de l'écriture de code générique *pour* la STL.

- **ex01 — `Span`** — une class stockant jusqu'à `N` nombres avec :
  - `addNumber(int)` — lève une exception si elle est pleine.
  - `shortestSpan()` — |différence| minimale entre deux nombres quelconques.
  - `longestSpan()` — max moins min.
  - Une overload qui ajoute une plage via des iterators : `void addNumbers(InputIt first, InputIt last)`.

  Astuce : triez une seule fois, puis `shortestSpan` se résume à un simple parcours linéaire des différences adjacentes. Utilisez `std::min_element` / `std::max_element` pour `longestSpan`.

- **ex02 — `MutantStack`** — héritez de `std::stack<T>` (oui, vraiment) et ajoutez des iterators. `std::stack` n'expose pas d'iterators par défaut, mais le container sous-jacent le fait. Vous les exposez via :
  ```cpp
  template <typename T>
  class MutantStack : public std::stack<T> {
  public:
      typedef typename std::stack<T>::container_type::iterator iterator;
      iterator begin() { return this->c.begin(); }
      iterator end()   { return this->c.end(); }
  };
  ```
  `this->c` est le container sous-jacent `protected` dont le standard garantit la présence. `typename` est obligatoire (voir [TEMPLATES.md §5](TEMPLATES.md)).

---

## 12. Dans CPP09 — choisir le bon container

- **ex00 — Bitcoin Exchange** — lisez un CSV de dates et de prix ; lisez les requêtes en entrée ; affichez la valeur à cette date (ou la date antérieure la plus proche). C'est le territoire de **`std::map<std::string, float>`** — recherche en O(log n) et itération triée.

- **ex01 — Reverse Polish Notation** — évaluez des expressions en notation polonaise inverse (RPN) comme `"8 9 * 9 - 9 - 9 - 4 - 1 +"`. Exercice classique sur **`std::stack`**.

- **ex02 — PmergeMe (Ford-Johnson)** — implémentez le tri par insertion-fusion en utilisant **deux containers différents** (vector + deque, ou vector + list). Le but est d'observer comment le même algorithme se comporte sur des structures de stockage différentes.

À la fin de CPP09, vous aurez choisi le bon container pour trois formes de problèmes différentes. C'est là toute la compétence.

---

## 13. Cheatsheet : quel container utiliser et quand

| Vous avez besoin de… | Orientez-vous vers |
|---|---|
| Par défaut, accès aléatoire rapide | `std::vector` |
| Nombreuses insertions/suppressions au milieu, stabilité des iterators | `std::list` |
| push/pop rapides aux deux extrémités | `std::deque` |
| LIFO | `std::stack` |
| FIFO | `std::queue` |
| Toujours récupérer le min ou le max | `std::priority_queue` |
| Valeurs uniques et triées | `std::set` |
| Recherche par clé | `std::map` |
| Doublons autorisés par clé | `std::multimap` / `std::multiset` |
| Taille fixe connue à la compilation | Tableau C ou `T[N]` en tant que membre |
| Une chaîne de caractères | `std::string` |

---

## 14. Règles empiriques

- **Par défaut, utilisez `std::vector`.** Ne changez que lorsque vous pouvez nommer l'opération spécifique qui est lente.
- **Préférez `++it` à `it++` dans les boucles.** La forme préfixée est moins coûteuse pour les types d'iterators.
- **Les boucles basées sur les plages (range-based loops) n'existent pas en C++98.** Vous écrivez `for (It it = c.begin(); it != c.end(); ++it)` — habituez-vous-y.
- **`it != end()`, jamais `it < end()`.** Tous les iterators ne prennent pas en charge `<`.
- **Ne conservez pas d'iterators à travers des modifications.** Les insertions/suppressions peuvent les invalider.
- **`map::find` plutôt que `map::operator[]`** lorsque vous voulez simplement vérifier une présence — `[]` effectue une insertion.
- **Passez les containers par `const T&`** — ils possèdent de la mémoire sur le heap ; la copie est coûteuse.
- **`typename` est obligatoire** lors de la désignation de types dépendants dans les templates (`typename std::vector<T>::iterator it`).

---

## 15. Pièges à éviter

- **`end()` n'est pas un élément.** Le déréférencer constitue un comportement indéfini (Undefined Behavior). Il marque la position "un élément après le dernier".
- **`erase` retourne l'iterator suivant** pour les sequence containers (et pour `map`/`set` en C++11). Écrivez `it = v.erase(it);`, et non `v.erase(it); ++it;`.
- **`push_back` peut invalider les iterators** pointant dans un `vector` si la capacité augmente. Si vous avez besoin d'iterators stables, utilisez `list`.
- **`map::operator[]` insère une valeur par défaut** lorsque la clé est manquante. Utilisez `find` pour vérifier la présence sans insérer.
- **`std::sort` a besoin d'iterators à accès aléatoire (random-access).** Utilisez `list::sort()` (fonction membre) sur les listes.
- **Les comparateurs doivent définir un ordre faible strict (strict weak ordering).** `!(a < b) && !(b < a)` signifie égalité. Des comparateurs incohérents peuvent boucler indéfiniment ou provoquer un segfault.
- **C++98 n'a pas de `std::unique_ptr`** — uniquement `std::auto_ptr`, qui a un comportement étrange (voir [MEMORY.md](../fundamentals/MEMORY.md#7.%20RAII%20%E2%80%94%20the%20central%20idea)) et qui doit être évité sauf nécessité absolue.
- **Ne dérivez pas publiquement des containers de la STL** sauf pour des besoins très ciblés comme MutantStack. Les containers STL n'ont pas de destructeurs virtuels ; la suppression polymorphique entraîne des fuites de mémoire.
- **`remove` ne supprime pas.** Enchaînez avec `erase`. L'idiome est `v.erase(std::remove(v.begin(), v.end(), x), v.end());`.