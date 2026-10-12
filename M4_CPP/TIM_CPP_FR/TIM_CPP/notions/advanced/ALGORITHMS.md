# Référence des fonctions de la STL — Conteneurs & Algorithmes

> **TL;DR.** Deux moitiés. La **Partie 1** couvre chaque *member function* de conteneur digne d'intérêt, sous forme de matrice multi-conteneurs complétée de notes par famille — ce que font `push_back`, `insert`, `find`, `erase`, `operator[]` et où ils existent. La **Partie 2** est la **bibliothèque `<algorithm>`**, exhaustive pour C++98, regroupée par fonction (recherche → modification → tri → opérations sur ensembles → heap → min/max → permutations). La **Partie 3** complète le tout avec `<numeric>` et `<functional>`. Tout est annoté avec la complexité et les pièges de C++98 (car la moitié des appels "standards" trouvés en ligne sont en C++11+ et `-std=c++98 -Werror` les rejette).

Voir aussi : [`STL.md`](STL.md) (le carrefour conceptuel) · [`TEMPLATES.md`](TEMPLATES.md) · [`MEMBER_FUNCTION_POINTERS.md`](MEMBER_FUNCTION_POINTERS.md) · analyses approfondies des conteneurs dans [`../containers/INDEX.md`](../containers/INDEX.md) · [`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md)

Headers à inclure :
```cpp
#include <algorithm>   // sort, find, copy, remove, transform, ... (le gros du sujet)
#include <numeric>     // accumulate, inner_product, partial_sum, adjacent_difference
#include <functional>  // less, greater, plus, bind2nd, ptr_fun, ...
#include <iterator>    // back_inserter, ostream_iterator, distance, advance
#include <utility>     // pair, make_pair
```

---

## Table des matières

**Partie 0 — Comment lire ce document**
- [Le contrat des itérateurs sur lequel repose chaque algorithme](#Le%20contrat%20des%20it%C3%A9rateurs%20sur%20lequel%20repose%20chaque%20algorithme)
- [La liste des pièges C++98 (à mémoriser)](#La%20liste%20des%20pi%C3%A8ges%20C%2B%2B98%20%28%C3%A0%20m%C3%A9moriser%29)

**Partie 1 — Member functions des conteneurs**
1. [Fonctions communes à (presque) tous les conteneurs](#1.%20Fonctions%20communes%20%C3%A0%20%28presque%29%20tous%20les%20conteneurs)
2. [Conteneurs de séquence — `vector` / `deque` / `list`](#2.%20Conteneurs%20de%20s%C3%A9quence%20%E2%80%94%20vector%20%2F%20deque%20%2F%20list)
3. [Membres spécifiques de `std::list`](#3.%20Membres%20sp%C3%A9cifiques%20de%20std%3A%3Alist)
4. [Adaptateurs de conteneurs — `stack` / `queue` / `priority_queue`](#4.%20Adaptateurs%20de%20conteneurs%20%E2%80%94%20stack%20%2F%20queue%20%2F%20priority_queue)
5. [Conteneurs associatifs — `set` / `map` / `multiset` / `multimap`](#5.%20Conteneurs%20associatifs%20%E2%80%94%20set%20%2F%20map%20%2F%20multiset%20%2F%20multimap)
6. [`std::string` en tant que conteneur](#6.%20std%3A%3Astring%20en%20tant%20que%20conteneur)

**Partie 2 — La bibliothèque `<algorithm>` (exhaustive, C++98)**
7. [Opérations de séquence non modifiantes](#7.%20Op%C3%A9rations%20de%20s%C3%A9quence%20non%20modifiantes)
8. [Opérations de séquence modifiantes](#8.%20Op%C3%A9rations%20de%20s%C3%A9quence%20modifiantes)
9. [Partitionnement](#9.%20Partitionnement)
10. [Tri](#10.%20Tri)
11. [Recherche binaire (sur plages triées)](#11.%20Recherche%20binaire%20%28sur%20plages%20tri%C3%A9es%29)
12. [Fusion & opérations sur ensembles (sur plages triées)](#12.%20Fusion%20%26%20op%C3%A9rations%20sur%20ensembles%20%28sur%20plages%20tri%C3%A9es%29)
13. [Opérations de tas (heap)](#13.%20Op%C3%A9rations%20de%20tas%20%28heap%29)
14. [Min / max](#14.%20Min%20%2F%20max)
15. [Permutations & comparaisons](#15.%20Permutations%20%26%20comparaisons)

**Partie 3 — Compléments**
16. [`<numeric>`](#16.%20%3Cnumeric%3E)
17. [`<functional>` — foncteurs & adaptateurs](#17.%20%3Cfunctional%3E%20%E2%80%94%20foncteurs%20%26%20adaptateurs)
18. [`<iterator>` — le ciment](#18.%20%3Citerator%3E%20%E2%80%94%20le%20ciment)
19. [Livre de recettes d'idiomes](#19.%20Livre%20de%20recettes%20d%27idiomes)
20. [Ce qui N'EST PAS en C++98](#20.%20Ce%20qui%20N%27EST%20PAS%20en%20C%2B%2B98)

---

## Le contrat des itérateurs sur lequel repose chaque algorithme

Chaque algorithme prend une **plage semi-ouverte** `[first, last)` — `last` désigne l'élément *juste après* le dernier élément qui vous intéresse, exactement comme `end()`. L'élément situé à `last` n'est jamais manipulé.

```
   [first, last)   signifie   first, first+1, ..., last-1     (last exclu)

     v.begin()                                  v.end()
        │                                          │
        v                                          v
      ┌───┬───┬───┬───┬───┐
      │ a │ b │ c │ d │ e │
      └───┴───┴───┴───┴───┘
```

Un algorithme n'a jamais connaissance du conteneur — il appelle seulement `*it`, `++it`, éventuellement `--it` ou `it + n`. Les opérations qu'il est autorisé à appeler définissent la **category** de l'iterator (voir [`STL.md` §7](STL.md#7.%20Iterator%20categories)). Conséquence pratique :

| Category | Conteneurs | Algorithmes qui en ont besoin |
|---|---|---|
| Random-access (`it + n`, `it[n]`, `it < it2`) | `vector`, `deque`, raw pointers, `string` | `sort`, `nth_element`, `partial_sort`, `random_shuffle`, la version rapide de `lower_bound` |
| Bidirectionnel (`++`, `--`) | `list`, `set`, `map`, `multiset`, `multimap` | `reverse`, `next_permutation`, `inplace_merge`, `stable_partition` |
| Forward (`++` uniquement, multi-pass) | `forward_list`* | `remove`, `unique`, `replace`, `rotate` |
| Input / Output (passe unique) | itérateurs de flux | `copy`, `find`, `count`, `transform`, `accumulate` |

\* `forward_list` est apparu en C++11 — absent de votre boîte à outils. Mentionné pour être exhaustif.

**La règle qui piège tout le monde :** `std::sort` nécessite des itérateurs à accès aléatoire (random-access), ainsi **vous ne pouvez pas utiliser `std::sort` sur une `std::list` ou un `std::set`**. `list` possède sa propre fonction membre `.sort()` ; `set` est déjà trié.

---

## La liste des pièges C++98 (à mémoriser)

Ceux-ci ont l'air standards, compilent sur les chaînes d'outils modernes, et **échouent sous `-std=c++98 -Werror`**. L'évaluation se fait en C++98.

| Vous pourriez vouloir utiliser… | En réalité | Remplacement C++98 |
|---|---|---|
| `std::copy_if` | **C++11** | `std::remove_copy_if` avec un prédicat inversé |
| `std::all_of` / `any_of` / `none_of` | **C++11** | `std::find_if` / `std::count_if` et comparer |
| `std::find_if_not` | **C++11** | `std::find_if` avec `std::not1(pred)` |
| `std::iota` | **C++11** (`<numeric>`) | boucle manuelle, ou `std::generate` avec un foncteur compteur |
| `std::is_sorted` / `is_heap` | **C++11** | `std::adjacent_find(b, e, greater)` pour vérifier le tri |
| `std::min({a,b,c})` (init-list) | **C++11** | imbriquer : `std::min(a, std::min(b, c))` |
| `map::at(k)` | **C++11** | `find` puis comparer avec `end()` |
| `it = s.erase(it)` sur **set/map** | erase renvoie **`void`** en C++98 | `s.erase(it++);` (idiome de post-incrémentation) |
| `auto`, range-`for`, lambdas | **C++11** | expliciter le type ; boucle par indice/iterator ; **foncteurs** (Partie 3) |
| `vector::data()` | **C++11** | `&v[0]` (UB si vide — protéger avec `!empty()`) |
| `emplace` / `emplace_back` | **C++11** | `insert` / `push_back` (coûte une copie) |

> `std::erase(it)` sur un conteneur de **séquence** (`vector`/`deque`/`list`) renvoie **bien** le prochain iterator en C++98 — seuls les conteneurs **associatifs** diffèrent. Cette asymétrie est la surprise la plus fréquente en C++98.

---

# Partie 1 — Member functions des conteneurs

## 1. Fonctions communes à (presque) tous les conteneurs

Celles-ci existent sur pratiquement tous les conteneurs standards (à l'exception des adaptateurs — ils sont volontairement restreints, voir §4).

| Membre | Renvoie | Coût | Notes |
|---|---|---|---|
| `size()` | `size_type` | O(1)† | Nombre d'éléments. †`list::size()` est O(n) sur certaines anciennes libstdc++ — véritable O(1) depuis C++11. |
| `empty()` | `bool` | O(1) | **Toujours préférer à `size() == 0`** — correct pour tous les conteneurs, y compris les listes à `size()` en O(n). |
| `max_size()` | `size_type` | O(1) | Plafond théorique. Rarement utile. |
| `swap(other)` | `void` | O(1) | Échange les structures internes (pointeurs / racines d'arbres). Ne lance jamais d'exception, ne copie jamais les éléments. |
| `begin()` / `end()` | iterator | O(1) | `const_iterator` lorsque le conteneur est `const`. |
| `rbegin()` / `rend()` | reverse_iterator | O(1) | Parcourt de l'arrière vers l'avant. |
| `clear()` | `void` | O(n) | Détruit tous les éléments. **Ne libère pas la capacité d'un `vector`** (voir [`VECTOR.md` §9.9](../containers/VECTOR.md)). |
| `operator==`, `!=` | `bool` | O(n) | Élément par élément ; requiert `T::operator==`. |
| `operator<`, `>`, `<=`, `>=` | `bool` | O(n) | Lexicographique ; requiert `T::operator<`. |

La construction est également uniforme — chaque conteneur supporte la construction par défaut, par copie, par plage (range), et (pour les conteneurs de séquence) par remplissage (fill) :

```cpp
std::vector<int> a;                       // vide
std::vector<int> b(other);                // copie
std::vector<int> c(first, last);          // range : copie [first, last) depuis n'importe quel objet itérable
std::vector<int> d(5, 42);                // fill : cinq 42   (conteneurs de séquence uniquement)
```

Le **constructeur par plage** est le convertisseur universel — il copie depuis *n'importe quelle* paire d'itérateurs compatibles, y compris un tableau C ou un type de conteneur différent :

```cpp
int raw[] = {3, 1, 2};
std::set<int>    s(raw, raw + 3);         // tableau → set (trie + déduplique)
std::list<int>   l(s.begin(), s.end());   // set     → list
std::vector<int> v(l.begin(), l.end());   // list    → vector
```

---

## 2. Conteneurs de séquence — `vector` / `deque` / `list`

Tous les détails par conteneur (modèle mental, invalidation, efficacité) se trouvent dans [`VECTOR.md`](../containers/VECTOR.md), [`DEQUE.md`](../containers/DEQUE.md), [`LIST.md`](../containers/LIST.md). Voici la matrice consolidée des **disponibilités de fonctions**.

| Membre | `vector` | `deque` | `list` | Coût | Note |
|---|:---:|:---:|:---:|---|---|
| `operator[]` / `at()` | ✅ | ✅ | ❌ | O(1) | `at` vérifie les limites → lance `out_of_range` ; `[]` est un UB hors limites. Pas d'accès aléatoire sur `list`. |
| `front()` / `back()` | ✅ | ✅ | ✅ | O(1) | UB si vide (pas d'exception). |
| `push_back()` / `pop_back()` | ✅ | ✅ | ✅ | O(1)* | `vector` push_back est en O(1) *amorti* (réallocation) ; `deque`/`list` sont en O(1) strict. |
| `push_front()` / `pop_front()` | ❌ | ✅ | ✅ | O(1) | Absent de `vector` — l'insertion au début y est en O(n). |
| `insert(pos, x)` | ✅ | ✅ | ✅ | vec/deque O(n), list O(1) | Surcharges `(pos, n, x)` et `(pos, first, last)` présentes partout. |
| `erase(pos)` / `erase(b, e)` | ✅ | ✅ | ✅ | vec/deque O(n), list O(1) | **Renvoie l'iterator suivant** (conteneurs de séquence). |
| `assign(n, x)` / `assign(first, last)` | ✅ | ✅ | ✅ | O(n) | Remplace tout le contenu ; peut réutiliser l'espace alloué. |
| `resize(n[, x])` | ✅ | ✅ | ✅ | O(n) | Agrandit (remplissage par défaut ou avec `x`) / rétrécit (détruit la fin). |
| `capacity()` / `reserve(n)` | ✅ | ❌ | ❌ | O(1) / O(n) | **`vector` uniquement.** Préalloue pour éviter les réallocations. |
| `swap(other)` | ✅ | ✅ | ✅ | O(1) | |
| `data()` | ✅ | ❌ | ❌ | O(1) | **C++11** — utiliser `&v[0]` en C++98. |
```cpp
std::vector<int> v;
v.reserve(100);                 // vector uniquement : une seule allocation au départ
v.push_back(10);                // O(1) amorti
v.insert(v.begin(), 5);         // O(n) — décale tout vers la droite ; deque/list ne le font pas

std::deque<int> d;
d.push_front(1);                // O(1) aux deux extrémités — tout l'intérêt de deque
d.push_back(2);

std::list<int> l(v.begin(), v.end());
std::list<int>::iterator it = l.begin();
++it;
l.insert(it, 99);               // O(1) — aucun décalage dans une liste chaînée
it = l.erase(it);               // O(1), renvoie le suivant (règle des sequence containers)
```

> **L'invalidation des iterator diffère fortement.** `vector` : tout agrandissement invalide *absolument tout*. `deque` : un push à l'une des extrémités invalide les iterators mais **conserve les references valides**. `list` : rien n'est invalidé sauf le node supprimé. La page de chaque container contient le tableau complet.

---

## 3. Les fonctions membres spéciales de `std::list`

`list` échange le random access contre des opérations structurelles en O(1). Ces fonctions membres existent *uniquement* sur `list` (et `forward_list`), et c'est la raison pour laquelle vous la choisiriez. Détails complets dans [`LIST.md`](../containers/LIST.md).

| Member | Ce qu'il fait | Coût |
|---|---|---|
| `splice(pos, other)` | Déplace **tous** les nodes de `other` dans `*this` avant `pos`. Aucune copie, aucune allocation — simple réassignation de pointer. `other` finit vide. | O(1) |
| `splice(pos, other, it)` | Déplace l'unique node `it` depuis `other`. | O(1) |
| `splice(pos, other, first, last)` | Déplace une plage de nodes. | O(1)† |
| `remove(value)` | Supprime **chaque** élément `== value`. (Fonction membre du container — distincte de `std::remove`, qui ne peut pas réellement supprimer.) | O(n) |
| `remove_if(pred)` | Supprime chaque élément satisfaisant `pred`. | O(n) |
| `unique()` | Réduit les éléments identiques **consécutifs** à un seul. Triez d'abord pour une déduplication globale. | O(n) |
| `merge(other)` | Fusionne deux listes **triées** en une seule liste triée, en déplaçant les nodes (`splice`, aucune copie). `other` finit vide. | O(n) |
| `sort()` | Trie sur place (tri fusion / merge sort). Utilisez ceci — **`std::sort` ne fonctionnera pas sur une list.** | O(n log n) |
| `reverse()` | Inverse l'ordre des nodes sur place. | O(n) |

\† Le `splice` d'une plage est en O(1) en C++98 si vous ne demandez pas `size()` après coup sur une implémentation à taille en O(n) ; traitez cela comme du O(1) pour la réassignation, et O(distance) si la taille doit être recalculée.

```cpp
std::list<int> a, b;
// ... remplir les deux, triées ...
a.merge(b);                     // a := union triée (avec doublons), b := vide. Zéro copie.
a.unique();                     // supprime les doublons consécutifs
a.remove_if(IsNegative());      // supprime tous les négatifs en une seule passe O(n)
```

> **Pourquoi `list::remove` et non `std::remove` ?** La fonction libre `std::remove` (Partie 2, §8) ne fait que *décaler* les survivants et vous fournit une nouvelle fin logique — elle ne peut pas réduire la taille du container, car les algorithmes n'ont pas connaissance des containers. `list::remove` est une vraie fonction membre qui détache et libère les nodes.

---

## 4. Container adapters — `stack` / `queue` / `priority_queue`

Les adapters **enveloppent** un sequence container et exposent une interface délibérément minimale. **Pas d'iterators, pas de `begin()`, pas de random access.** Détails : [`STACK.md`](../containers/STACK.md), [`QUEUE.md`](../containers/QUEUE.md), [`PRIORITY_QUEUE.md`](../containers/PRIORITY_QUEUE.md).

| Member | `stack` (LIFO) | `queue` (FIFO) | `priority_queue` (max-heap) |
|---|:---:|:---:|:---:|
| `push(x)` | ✅ | ✅ | ✅ (O(log n)) |
| `pop()` | ✅ | ✅ | ✅ (O(log n)) — **renvoie `void`** |
| `top()` | ✅ | ❌ | ✅ (O(1)) |
| `front()` / `back()` | ❌ | ✅ / ✅ | ❌ |
| `size()` / `empty()` | ✅ | ✅ | ✅ |
| Conteneur sous-jacent par défaut | `deque` | `deque` | `vector` |

```cpp
std::stack<int> s;
s.push(1); s.push(2);
int x = s.top();                // 2  — lire AVANT le pop
s.pop();                        // void ! cela ne renvoie PAS l'élément

std::priority_queue<int> pq;    // max-heap par défaut
pq.push(3); pq.push(9); pq.push(1);
pq.top();                       // 9
// min-heap : passer le comparateur (voir §17)
std::priority_queue<int, std::vector<int>, std::greater<int> > minpq;
```

> **La règle d'or :** `pop()` sur les trois adapters renvoie `void`. Appelez toujours `top()` (ou `front()`) *d'abord*, puis `pop()`. Appeler `top()` sur un adapter vide est un UB — protégez l'appel avec `empty()`.

Vous pouvez changer le container sous-jacent au niveau du type : `std::stack<int, std::vector<int> >`. `queue` interdit un sous-jacent en `vector` (elle a besoin de `pop_front`).

---

## 5. Associative containers — `set` / `map` / `multiset` / `multimap`

Triés, basés sur des clés, en O(log n), adossés à un arbre équilibré. Les iterators sont **bidirectionnels** et parcourent dans **l'ordre trié des clés**. Détails : [`SET.md`](../containers/SET.md), [`MAP.md`](../containers/MAP.md), [`MULTISET.md`](../containers/MULTISET.md), [`MULTIMAP.md`](../containers/MULTIMAP.md).

| Member | `set` | `map` | `multiset` | `multimap` | Coût | Note |
|---|:---:|:---:|:---:|:---:|---|---|
| `insert(x)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Les versions uniques renvoient `pair<iterator,bool>` ; les versions multi renvoient `iterator`. |
| `insert(hint, x)` | ✅ | ✅ | ✅ | ✅ | O(1) amorti | Un hint correct rend l'insertion en masse en O(n) au lieu de O(n log n). |
| `erase(key)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Renvoie le **nombre d'éléments supprimés** (`size_type`). |
| `erase(it)` | ✅ | ✅ | ✅ | ✅ | O(1) amorti | **Renvoie `void` en C++98** — utilisez `erase(it++)`. |
| `find(key)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Iterator vers l'élément, ou `end()`. |
| `count(key)` | ✅ | ✅ | ✅ | ✅ | O(log n) | 0/1 pour les conteneurs uniques ; décompte réel pour les conteneurs multiples. |
| `lower_bound(k)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Premier élément **qui n'est pas inférieur à** `k`. |
| `upper_bound(k)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Premier élément **strictement supérieur à** `k`. |
| `equal_range(k)` | ✅ | ✅ | ✅ | ✅ | O(log n) | `pair(lower_bound, upper_bound)` — l'outil quotidien pour les multi-containers. |
| `operator[](k)` | ❌ | ✅ | ❌ | ❌ | O(log n) | **map uniquement.** Insère une valeur par défaut en cas d'absence — le piège classique. |
| `key_comp()` / `value_comp()` | ✅ | ✅ | ✅ | ✅ | O(1) | Les objets de comparaison, si vous devez reproduire l'ordre de tri. |

```cpp
std::map<std::string, int> ages;
ages["Alice"] = 30;                          // [] insère si absent
ages.insert(std::make_pair("Bob", 25));      // insert n'écrasera pas un élément existant

// Lire SANS risque d'insertion :
std::map<std::string, int>::iterator it = ages.find("Carol");
if (it != ages.end())
    std::cout << it->first << " = " << it->second << "\n";   // pair<const K, V>

// multi-container : récupérer toutes les valeurs pour une clé
std::multimap<std::string, int> mm;
std::pair<std::multimap<std::string,int>::iterator,
          std::multimap<std::string,int>::iterator> r = mm.equal_range("k");
for (std::multimap<std::string,int>::iterator i = r.first; i != r.second; ++i)
    std::cout << i->second << "\n";
```

> **Deux pièges C++98 spécifiques à ces containers :**
> 1. **`erase(it)` renvoie `void`.** L'habitude prise avec les sequence containers `it = m.erase(it)` ne compilera pas. Utilisez l'idiome de post-incrémentation :
> ```cpp
> for (std::map<K,V>::iterator it = m.begin(); it != m.end(); ) {
>     if (cond(it)) m.erase(it++);   // évaluer it, avancer, PUIS supprimer l'ancienne position
>     else          ++it;
> }
> ```
> 2. **Utilisez la fonction *membre* `lower_bound`, pas celle de `<algorithm>`.** `std::lower_bound(s.begin(), s.end(), k)` *fonctionne* sur un `set` mais est en **O(n)** — l'algorithme libre a besoin d'un random access pour effectuer la recherche binaire, et les iterators d'arbres n'avancent que d'un pas à la fois. `s.lower_bound(k)` parcourt l'arbre en véritable O(log n).

---

## 6. `std::string` en tant que container

`std::string` est `std::basic_string<char>` — un sequence container doté de méthodes supplémentaires pour le texte. Il prend en charge `operator[]`, `at`, `begin`/`end`, `push_back`, `insert`, `erase`, `size`, `empty`, `clear`, ainsi que les méthodes spécifiques aux chaînes `find`, `substr`, `c_str`, `append`, `+`. Liste complète des méthodes : [`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md). Puisque ses iterators sont à accès aléatoire, chaque algorithme de `<algorithm>` fonctionne dessus :

```cpp
std::string s = "hello";
std::sort(s.begin(), s.end());                              // "ehllo"
std::reverse(s.begin(), s.end());                           // "ollhe"
std::transform(s.begin(), s.end(), s.begin(), ::toupper);   // "OLLHE"
```

---

# Partie 2 — La bibliothèque `<algorithm>` (exhaustive, C++98)

Chaque algorithme ci-dessous se trouve dans `<algorithm>` et existe en C++98. Tout élément C++11+ est signalé au §20. Motifs de nommage à assimiler :
- Suffixe **`_if`** → prend un *predicate* au lieu d'une valeur (`find` → `find_if`).
- Suffixe **`_copy`** → écrit les résultats dans une plage de sortie distincte au lieu de modifier sur place (`remove` → `remove_copy`).
- Beaucoup prennent un foncteur **comparateur / predicate** optionnel à la fin — cette surcharge est listée sur la même ligne.

## 7. Opérations sur les séquences ne modifiant pas les éléments

Celles-ci lisent une plage ; elles ne réorganisent ni n'écrivent jamais les éléments.

| Fonction | Forme de la signature | Valeur de retour | Coût | Ce qu'elle fait |
|---|---|---|---|---|
| `for_each` | `(first, last, fn)` | une copie de `fn` | O(n) | Appelle `fn(*it)` sur chaque élément. Renvoie le foncteur — pratique pour les accumulateurs **avec état**. |
| `find` | `(first, last, val)` | iterator | O(n) | Premier élément `== val`, sinon `last`. |
| `find_if` | `(first, last, pred)` | iterator | O(n) | Premier élément où `pred(*it)` est vrai. |
| `find_end` | `(first, last, s_first, s_last)` | iterator | O(n·m) | Dernière occurrence de la sous-plage. |
| `find_first_of` | `(first, last, s_first, s_last)` | iterator | O(n·m) | Premier élément qui est égal à *n'importe quel* élément de la deuxième plage. |
| `adjacent_find` | `(first, last[, pred])` | iterator | O(n) | Première position où deux éléments **consécutifs** correspondent. Niez pour une vérification `is_sorted`. |
| `count` | `(first, last, val)` | `difference_type` | O(n) | Combien d'éléments `== val`. |
| `count_if` | `(first, last, pred)` | `difference_type` | O(n) | Combien d'éléments satisfont `pred`. |
| `mismatch` | `(first, last, first2[, pred])` | `pair<It1,It2>` | O(n) | Première position où deux plages diffèrent. |
| `equal` | `(first, last, first2[, pred])` | `bool` | O(n) | Deux plages sont-elles égales élément par élément ? (La deuxième plage doit être de longueur ≥.) |
| `search` | `(first, last, s_first, s_last)` | iterator | O(n·m) | Première occurrence d'une sous-plage (recherche de style sous-chaîne). |
| `search_n` | `(first, last, count, val)` | iterator | O(n) | Première séquence de `count` éléments consécutifs égaux à `val`. |
```cpp
// Recherche linéaire — fonctionne sur N'IMPORTE QUEL container (c'est ce que wrap easyfind dans CPP08) :
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 42);
if (it != v.end()) { /* trouvé à *it */ }

// for_each avec un functor à état renvoie le functor, contenant le résultat :
struct Sum { long total; Sum() : total(0) {} void operator()(int n) { total += n; } };
Sum s = std::for_each(v.begin(), v.end(), Sum());
std::cout << s.total << "\n";

// count_if avec un functor paramétré (voir §17) :
int big = std::count_if(v.begin(), v.end(), GreaterThan(100));
```

## 8. Opérations de modification de séquence

Celles-ci écrivent dans une plage ou la réordonnent. **La famille `remove`/`unique` ne RÉDUIT PAS la taille du container** — voir l'idiome erase-remove (§19).

| Fonction | Forme de signature | Retourne | Coût | Ce qu'elle fait |
|---|---|---|---|---|
| `copy` | `(first, last, d_first)` | iterator de fin de sortie | O(n) | Copie vers l'avant dans `d_first…`. Les plages ne doivent pas se chevaucher de gauche à droite. |
| `copy_backward` | `(first, last, d_last)` | début de sortie | O(n) | Copie de l'arrière vers l'avant — à utiliser lorsque les plages se chevauchent et que dest est *après* src. |
| `swap` | `(a, b)` | `void` | O(1) | Échange deux objets. (Dans `<algorithm>` en C++98 ; déplacé dans `<utility>` en C++11.) |
| `swap_ranges` | `(first, last, first2)` | fin de la 2nde | O(n) | Échange élément par élément deux plages de même longueur. |
| `iter_swap` | `(it_a, it_b)` | `void` | O(1) | Échange les deux éléments pointés. |
| `transform` | `(first, last, d_first, unary_fn)` | fin de sortie | O(n) | Écrit `fn(*it)` dans dest. **Opération de map.** |
| `transform` | `(first, last, first2, d_first, binary_fn)` | fin de sortie | O(n) | Combine deux plages : `fn(*a, *b)`. |
| `replace` | `(first, last, old, new)` | `void` | O(n) | Remplace chaque `old` par `new` sur place. |
| `replace_if` | `(first, last, pred, new)` | `void` | O(n) | Remplace là où `pred` est vrai. |
| `replace_copy` / `replace_copy_if` | `(…, d_first, …)` | fin de sortie | O(n) | Pareil, en écrivant dans une plage dest. |
| `fill` | `(first, last, val)` | `void` | O(n) | Définit chaque élément à `val`. |
| `fill_n` | `(first, n, val)` | fin de sortie | O(n) | Définit les `n` premiers éléments. |
| `generate` | `(first, last, gen)` | `void` | O(n) | Assigne `gen()` à chacun (functor appelé par élément). |
| `generate_n` | `(first, n, gen)` | fin de sortie | O(n) | `n` premiers à partir de `gen()`. |
| `remove` | `(first, last, val)` | nouvelle fin logique | O(n) | **Décale** les non-`val` vers l'avant ; la fin est résiduelle. À associer avec `erase`. |
| `remove_if` | `(first, last, pred)` | nouvelle fin logique | O(n) | Pareil, par prédicat. |
| `remove_copy` / `remove_copy_if` | `(…, d_first, …)` | fin de sortie | O(n) | Copie uniquement les éléments conservés. `remove_copy_if` + `not1` constitue le `copy_if` de C++98. |
| `unique` | `(first, last[, pred])` | nouvelle fin logique | O(n) | Regroupe les éléments égaux **consécutifs**. Trier d'abord pour une déduplication globale. |
| `unique_copy` | `(first, last, d_first[, pred])` | fin de sortie | O(n) | Copie avec les doublons consécutifs supprimés. |
| `reverse` | `(first, last)` | `void` | O(n) | Inverse sur place (nécessite bidirectional). |
| `reverse_copy` | `(first, last, d_first)` | fin de sortie | O(n) | Copie inversée dans dest. |
| `rotate` | `(first, middle, last)` | `void` (C++98) | O(n) | Effectue une rotation afin que `middle` devienne le nouveau premier élément. |
| `rotate_copy` | `(first, middle, last, d_first)` | fin de sortie | O(n) | Copie avec rotation. |
| `random_shuffle` | `(first, last[, rng])` | `void` | O(n) | Mélange aléatoire. **Déprécié en C++14 / retiré en C++17** — valide en C++98, mais sachez que c'est une impasse ; le code moderne utilise `std::shuffle`. |

```cpp
// transform = map. Met au carré chaque élément dans un nouveau vector :
std::vector<int> out(v.size());
std::transform(v.begin(), v.end(), out.begin(), Square());

// transform avec deux entrées = zip-with. Somme élément par élément de a et b dans c :
std::transform(a.begin(), a.end(), b.begin(), c.begin(), std::plus<int>());

// fill / generate
std::fill(v.begin(), v.end(), 0);
std::generate(v.begin(), v.end(), Counter());   // 0,1,2,3,... si Counter() renvoie puis fait ++

// remove ne supprime PAS (n'erase pas) — voir §19 pour l'idiome complet :
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

> **Pourquoi `remove` ne peut pas supprimer (à lire une fois pour économiser des heures) :** les algorithmes ne manipulent que des iterators, jamais le container lui-même, ils ne peuvent donc pas appeler `erase` ni modifier `size()`. `std::remove` fait la seule chose possible — décaler les éléments restants vers l'avant et renvoyer l'endroit où se terminent désormais les "vraies" données. La fin résiduelle existe toujours. Vous terminez le travail avec le propre `erase(new_end, end())` du container.

## 9. Partitionnement

| Fonction | Signature | Retourne | Coût | Ce qu'elle fait |
|---|---|---|---|---|
| `partition` | `(first, last, pred)` | point de partition | O(n) | Réordonne pour que tous les éléments `pred`-vrai précèdent tous les `pred`-faux. **L'ordre à l'intérieur des groupes n'est pas préservé.** |
| `stable_partition` | `(first, last, pred)` | point de partition | O(n log n) | Pareil, mais **préserve l'ordre relatif** au sein de chaque groupe. |

L'iterator renvoyé pointe sur le premier élément du second groupe (faux) :

```cpp
std::vector<int>::iterator mid =
    std::stable_partition(v.begin(), v.end(), IsEven());
// [v.begin(), mid) sont pairs (ordre d'origine conservé) ; [mid, v.end()) sont impairs.
```

## 10. Tri

| Fonction | Signature | Coût | Ce qu'elle fait |
|---|---|---|---|
| `sort` | `(first, last[, cmp])` | O(n log n) | Introsort sur place. **Non stable.** Nécessite des random-access iterators. |
| `stable_sort` | `(first, last[, cmp])` | O(n log n)† | Préserve l'ordre des éléments équivalents. †O(n log²n) s'il ne peut pas allouer de mémoire temporaire. |
| `partial_sort` | `(first, middle, last[, cmp])` | O(n log k) | Trie seulement ce qui est nécessaire pour que `[first, middle)` contienne les k plus petits, triés. Le reste n'est pas spécifié. |
| `partial_sort_copy` | `(first, last, d_first, d_last[, cmp])` | O(n log k) | Les k plus petits dans une dest séparée (éventuellement plus petite). |
| `nth_element` | `(first, nth, last[, cmp])` | **O(n)** en moyenne | Place l'élément qui *serait* à `nth` s'il était trié ; tout ce qui précède lui est ≤, tout ce qui suit lui est ≥. La méthode rapide pour trouver une médiane / un percentile. |

```cpp
std::sort(v.begin(), v.end());                          // croissant
std::sort(v.begin(), v.end(), std::greater<int>());     // décroissant (functor, §17)
std::sort(v.begin(), v.end(), ByLength());              // personnalisé : strict weak ordering

// "Les 3 plus petits, triés ; le reste m'importe peu" — moins coûteux qu'un tri complet :
std::partial_sort(v.begin(), v.begin() + 3, v.end());

// Médiane sans trier l'ensemble — O(n) :
std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
int median = v[v.size()/2];
```

> **Contrat du comparateur :** `cmp(a, b)` doit être un **strict weak ordering** — renvoyer `true` uniquement lorsque `a` précède strictement `b`, et `cmp(a, a)` doit être `false`. Un comparateur `<=` (renvoyant vrai pour des éléments égaux) le viole et peut provoquer un **segfault** ou boucler à l'infini. En cas de doute, basez-vous sur le modèle de `std::less`.

## 11. Recherche binaire (sur plages triées)

Celles-ci nécessitent que la plage soit déjà **triée selon le même critère**. Toutes effectuent O(log n) *comparaisons* — mais n'atteignent O(log n) au *total* que sur des random-access iterators ; sur une `list`, elles se dégradent en O(n) car l'avancement de l'iterator est linéaire (utilisez la version membre du container sur `set`/`map`).

| Fonction | Retourne | Ce qu'elle fait |
|---|---|---|
| `lower_bound(first, last, val[, cmp])` | iterator | Première position où `val` pourrait être inséré sans briser l'ordre — c.-à-d. premier élément **non inférieur à** `val`. |
| `upper_bound(first, last, val[, cmp])` | iterator | Premier élément **strictement supérieur à** `val`. |
| `equal_range(first, last, val[, cmp])` | `pair<It,It>` | `(lower_bound, upper_bound)` — toute la séquence d'éléments égaux à `val`. |
| `binary_search(first, last, val[, cmp])` | `bool` | Uniquement "est-il présent ?" — aucune position. |

```cpp
std::sort(v.begin(), v.end());
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), key);
bool found = (it != v.end() && *it == key);   // lower_bound seul ne confirme pas l'égalité

// C'est le pattern de BitcoinExchange (CPP09) : "valeur à la date la plus proche <= requête".
// lower_bound donne le premier >= requête ; reculez d'un pas pour "le plus proche non postérieur".
```

## 12. Fusion & opérations sur ensembles (sur plages triées)

Les deux plages d'entrée doivent être **triées**. La sortie est triée. C'est ainsi que l'on effectue des unions/intersections sans construire de `set`.

| Fonction | Retourne | Ce qu'elle fait |
|---|---|---|
| `merge(f1,l1, f2,l2, d_first[, cmp])` | fin de sortie | Fusionne deux plages triées en une seule dest triée. |
| `inplace_merge(first, middle, last[, cmp])` | `void` | Fusionne deux sous-plages triées consécutives `[first,middle)` et `[middle,last)` sur place. (Pilier du tri fusion / PmergeMe.) |
| `includes(f1,l1, f2,l2[, cmp])` | `bool` | La seconde plage triée est-elle un sous-ensemble de la première ? |
| `set_union(f1,l1, f2,l2, d_first[, cmp])` | fin de sortie | A ∪ B (chaque élément max(countA, countB) fois). |
| `set_intersection(…)` | fin de sortie | A ∩ B. |
| `set_difference(…)` | fin de sortie | A \ B (dans A, pas dans B). |
| `set_symmetric_difference(…)` | fin de sortie | Dans exactement l'un de A ou B. |

```cpp
// a, b sont des vectors triés. Calcule l'intersection dans c :
std::vector<int> c;
std::set_intersection(a.begin(), a.end(),
                      b.begin(), b.end(),
                      std::back_inserter(c));   // back_inserter : agrandit c au fur et à mesure (§18)
```
## 13. Opérations de tas (heap)

Un tas ici est un max-heap stocké dans une plage à accès aléatoire (généralement un `vector`). C'est ce que `std::priority_queue` utilise sous le capot — n'utilisez ces fonctions directement que lorsque vous avez besoin de la forme sous forme de tableau.

| Fonction | Signature | Coût | Ce qu'elle fait |
|---|---|---|---|
| `make_heap(first, last[, cmp])` | | O(n) | Réorganise la plage en un heap valide ; le max à `first`. |
| `push_heap(first, last[, cmp])` | | O(log n) | Après avoir effectué un `push_back` d'un nouvel élément, le remonte à sa place. |
| `pop_heap(first, last[, cmp])` | | O(log n) | Déplace le max vers `last-1` ; vous effectuez ensuite un `pop_back`. |
| `sort_heap(first, last[, cmp])` | | O(n log n) | Transforme un heap en une plage triée (détruit la propriété de tas). |

```cpp
std::vector<int> v;  // ...filled...
std::make_heap(v.begin(), v.end());           // v[0] is now the max
v.push_back(99);
std::push_heap(v.begin(), v.end());           // restore heap including the new 99
std::pop_heap(v.begin(), v.end());            // max moved to back
int max = v.back();  v.pop_back();            // and removed
```

> `std::is_heap` / `std::is_heap_until` sont **C++11** — non disponibles.

## 14. Min / max

| Fonction | Retour | Coût | Note |
|---|---|---|---|
| `min(a, b[, cmp])` | plus petite valeur | O(1) | Deux arguments seulement en C++98 — **pas d'init-list** `min({a,b,c})`. Imbriquez : `min(a, min(b, c))`. |
| `max(a, b[, cmp])` | plus grande valeur | O(1) | Même restriction. |
| `min_element(first, last[, cmp])` | iterator | O(n) | Iterator vers le plus petit élément. `end()` si la plage est vide. |
| `max_element(first, last[, cmp])` | iterator | O(n) | Iterator vers le plus grand élément. |

```cpp
int lo = *std::min_element(v.begin(), v.end());
int hi = *std::max_element(v.begin(), v.end());
// longestSpan() in CPP08's Span exercise is exactly hi - lo.
```

> `std::minmax` et `std::minmax_element` (une seule passe pour les deux) sont **C++11**.

## 15. Permutations & comparaison

| Fonction | Retour | Coût | Ce qu'elle fait |
|---|---|---|---|
| `next_permutation(first, last[, cmp])` | `bool` | O(n) | Réorganise vers la permutation suivante dans l'ordre lexicographique. Renvoie `false` et revient à la première permutation (triée) lorsqu'il s'agissait de la dernière. |
| `prev_permutation(first, last[, cmp])` | `bool` | O(n) | La permutation précédente. |
| `lexicographical_compare(f1,l1, f2,l2[, cmp])` | `bool` | O(n) | `<` dans l'ordre lexicographique entre deux plages. Le moteur derrière l'`operator<` des conteneurs. |

```cpp
std::vector<int> v;  // must be SORTED to enumerate all permutations
std::sort(v.begin(), v.end());
do {
    /* use this permutation of v */
} while (std::next_permutation(v.begin(), v.end()));
```

---

# Partie 3 — Compagnons

## 16. `<numeric>`

Quatre réductions numériques. **`std::iota` est C++11 — non disponible ici.**

| Fonction | Signature | Retour | Ce qu'elle fait |
|---|---|---|---|
| `accumulate` | `(first, last, init[, binop])` | le résultat de la réduction | Somme, ou réduction avec une opération binaire personnalisée. |
| `inner_product` | `(first1, last1, first2, init[, op1, op2])` | le résultat | Produit scalaire, ou réduction généralisée sur deux plages. |
| `partial_sum` | `(first, last, d_first[, binop])` | fin de sortie | Cumuls partiels : `out[i] = in[0]+…+in[i]`. |
| `adjacent_difference` | `(first, last, d_first[, binop])` | fin de sortie | `out[0]=in[0]`, `out[i]=in[i]-in[i-1]`. Inverse de `partial_sum`. |

```cpp
int total = std::accumulate(v.begin(), v.end(), 0);                 // sum
long prod  = std::accumulate(v.begin(), v.end(), 1L, std::multiplies<long>());  // product
double dot = std::inner_product(a.begin(), a.end(), b.begin(), 0.0);            // a·b
```

> **Choisissez le bon type d'`init`.** `accumulate(v.begin(), v.end(), 0)` sur un `vector<double>` accumule dans un **`int`** et tronque — le type de l'accumulateur *est* le type d'`init`. Utilisez délibérément `0.0` (ou `0L`).

## 17. `<functional>` — functors & adaptateurs

Le C++98 ne dispose pas de lambdas. Un **functor** est une class avec un `operator()` ; vous passez une *instance* là où un appelable est attendu. Le standard en fournit de prêts à l'emploi. (Pour aller plus loin : [`STL.md` §10](STL.md#10.%20Functors%20%E2%80%94%20C%2B%2B98%27s%20answer%20to%20lambdas), et `operator()` lui-même dans [`MEMBER_FUNCTION_POINTERS.md`](MEMBER_FUNCTION_POINTERS.md).)

**Functors prêts à l'emploi** (chacun paramétré par un template sur `T`) :

| Groupe | Functors |
|---|---|
| Arithmétique | `plus`, `minus`, `multiplies`, `divides`, `modulus`, `negate` |
| Comparaison | `equal_to`, `not_equal_to`, `greater`, `less`, `greater_equal`, `less_equal` |
| Logique | `logical_and`, `logical_or`, `logical_not` |

```cpp
std::sort(v.begin(), v.end(), std::greater<int>());                  // descending
std::transform(a.begin(), a.end(), b.begin(), out.begin(), std::plus<int>());
std::priority_queue<int, std::vector<int>, std::greater<int> > minheap;  // min-heap
```

**Adaptateurs** — transforment des fonctions ordinaires / des functors binaires en un prédicat unaire attendu par un algorithme. (Tous **obsolètes en C++11**, mais ils constituent la boîte à outils de C++98. Le remplacement moderne est constitué par les lambdas, que vous n'avez pas.)

| Adaptateur | Transforme… | …en |
|---|---|---|
| `not1(pred)` | un prédicat unaire | sa négation |
| `not2(pred)` | un prédicat binaire | sa négation |
| `bind1st(f, x)` | le binaire `f` | l'unaire `f(x, ·)` |
| `bind2nd(f, x)` | le binaire `f` | l'unaire `f(·, x)` |
| `ptr_fun(fp)` | un pointer vers une fonction libre | un functor (afin de pouvoir lui appliquer `bind`/`not`) |
| `mem_fun(&C::m)` | une fonction membre | un functor prenant un `C*` |
| `mem_fun_ref(&C::m)` | une fonction membre | un functor prenant un `C&` |

```cpp
// "count elements > 5" using bind2nd + greater, no custom functor:
int n = std::count_if(v.begin(), v.end(),
                      std::bind2nd(std::greater<int>(), 5));

// "copy out the elements that are NOT spaces" — C++98 copy_if:
std::remove_copy_if(s.begin(), s.end(), std::back_inserter(out),
                    std::bind2nd(std::equal_to<char>(), ' '));

// call a member on every element of a vector<Widget*>:
std::for_each(ptrs.begin(), ptrs.end(), std::mem_fun(&Widget::draw));
```

En pratique, ces adaptateurs deviennent vite illisibles (les imbrications `bind2nd(not2(...))`). Pour tout ce qui n'est pas trivial, **écrivez un functor nommé** — c'est plus clair et c'est l'idiome standard du cursus 42 :

```cpp
struct GreaterThan {
    int n;
    GreaterThan(int v) : n(v) {}
    bool operator()(int x) const { return x > n; }
};
int n = std::count_if(v.begin(), v.end(), GreaterThan(5));
```

Pour rendre un functor utilisable avec `bind`, faites-le hériter de `std::unary_function<Arg, Result>` ou `std::binary_function<Arg1, Arg2, Result>` (ils fournissent simplement les `typedef`s recherchés par les adaptateurs).

## 18. `<iterator>` — le ciment

Les algorithmes qui *écrivent* (`copy`, `transform`, `remove_copy`, `set_*`) ont besoin d'un iterator de destination. Si le conteneur de destination est vide, vous ne pouvez pas leur passer `dest.begin()` (il n'y a nulle part où écrire). Les **iterators d'insertion** résolvent ce problème en appelant `push_back` / `insert` sous le capot.

| Utilitaire | Ce qu'il fait |
|---|---|
| `back_inserter(c)` | Output iterator qui effectue des `push_back` dans `c`. Le plus utilisé. |
| `front_inserter(c)` | Effectue des `push_front` (donc `deque`/`list`, pas `vector`). |
| `inserter(c, pos)` | Effectue des `insert` à la position `pos` (fonctionne aussi pour les conteneurs associatifs). |
| `ostream_iterator<T>(os, sep)` | Output iterator qui écrit dans un flux — affiche une plage en une ligne. |
| `istream_iterator<T>(is)` | Input iterator qui lit des `T` depuis un flux jusqu'à EOF. |
| `distance(first, last)` | Nombre d'étapes entre deux iterators (O(1) pour l'accès aléatoire, O(n) sinon). |
| `advance(it, n)` | Déplace `it` de `n` (la seule façon portable d'appliquer `+n` à un iterator qui n'est pas à accès aléatoire). |

```cpp
// Grow the destination as the algorithm produces output:
std::vector<int> evens;
std::remove_copy_if(v.begin(), v.end(), std::back_inserter(evens), IsOdd());

// Print a range without a loop:
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
std::cout << "\n";

// Read all ints from stdin into a vector:
std::vector<int> nums((std::istream_iterator<int>(std::cin)),
                       std::istream_iterator<int>());   // note: extra parens dodge the
                                                        // "most vexing parse"
```

---

## 19. Livre de recettes d'idiomes

La poignée de patterns composés qui reviennent constamment. Mémorisez leurs formes.

**Erase–remove — supprimer toutes les correspondances en O(n) :**
```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());          // by value
v.erase(std::remove_if(v.begin(), v.end(), IsBad()), v.end());     // by predicate
```
`std::remove` décale les éléments conservés vers l'avant et renvoie la nouvelle fin ; `erase` tronque la fin morte. Sans cela, vous écririez un erase dans une boucle, ce qui est en O(n²) et facile à rater. (Sur une `list`, préférez la fonction membre `l.remove_if(IsBad())`.)

**Sort + unique — déduplication globale :**
```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
```
`unique` ne supprime que les doublons *consécutifs*, vous devez donc d'abord trier.

**Ajuster la capacité d'un vector (pas de `shrink_to_fit` en C++98) :**
```cpp
std::vector<int>(v).swap(v);     // copy holds exactly size() capacity; swap it in
std::vector<int>().swap(v);      // or fully release: empty + capacity 0
```

**Supprimer les pointers détenus par un conteneur, puis vider :**
```cpp
for (std::vector<T*>::iterator it = v.begin(); it != v.end(); ++it)
    delete *it;
v.clear();
```
Pas de smart pointers en C++98 (`std::auto_ptr` est corrompu à l'intérieur des conteneurs — ne le stockez jamais dedans). Vous êtes responsable du `delete`. Voir [`../fundamentals/MEMORY.md`](../fundamentals/MEMORY.md).
**Supprimer pendant l'itération d'un container associatif (le contournement pour le retour `void`) :**
```cpp
for (std::map<K,V>::iterator it = m.begin(); it != m.end(); ) {
    if (drop(it)) m.erase(it++);   // post-incrémentation : it avance, l'ancienne copie est supprimée
    else          ++it;
}
```

**Afficher n'importe quel range en une ligne :**
```cpp
std::copy(c.begin(), c.end(), std::ostream_iterator<T>(std::cout, " "));
```

---

## 20. Ce qui N'EST PAS dans C++98

Liste rapide de rejet — si vous voyez ces éléments dans un tutoriel, cherchez la forme C++98 ci-dessus.

| Symbole | Introduit dans | Utiliser à la place |
|---|---|---|
| `copy_if` | C++11 | `remove_copy_if` + `not1` / prédicat inversé |
| `all_of`, `any_of`, `none_of` | C++11 | `find_if` / `count_if` + comparaison |
| `find_if_not` | C++11 | `find_if` + `not1(pred)` |
| `is_sorted`, `is_sorted_until` | C++11 | `adjacent_find` avec un comparateur `>` |
| `is_heap`, `is_heap_until` | C++11 | — (faites simplement confiance à votre propre `make_heap`) |
| `min`/`max` avec `{init list}` | C++11 | imbriquer des appels à deux arguments |
| `minmax`, `minmax_element` | C++11 | `min_element` + `max_element` séparés |
| `iota` | C++11 (`<numeric>`) | boucle manuelle ou `generate` + functor compteur |
| `shuffle` | C++11 | `random_shuffle` (lui-même supprimé après C++17) |
| `move`, `move_backward` | C++11 | `copy` / `copy_backward` |
| `emplace`, `emplace_back`, `emplace_front` | C++11 | `insert` / `push_back` / `push_front` |
| `cbegin`, `cend` | C++11 | `const_iterator` à partir d'un container `const` |
| `auto`, range-`for`, lambdas | C++11 | écrire le type explicitement ; boucle explicite ; functors (§17) |
| `std::function`, `std::bind` | C++11 | function pointers + `bind1st`/`bind2nd`, ou functors nommés |
| `unordered_set`, `unordered_map` | C++11 | `set` / `map` (O(log n), pas O(1), mais trié) |

---

> **Résumé en une ligne.** Les containers vous fournissent `push_back` / `insert` / `find` / `erase` (attention à ceux qui retournent `void` en C++98) ; `<algorithm>` vous fournit tout ce qui opère sur `[first, last)` — recherche, modification, tri, opérations sur les ensembles (set-ops), heap, min/max ; liez-les avec des functors et des insert iterators. Quand une fonction a l'air standard mais ne compile pas sous `-std=c++98`, vérifiez le §20.