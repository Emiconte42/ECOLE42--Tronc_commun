# `std::set` — Clés uniques triées

> **TL;DR.** `std::set<T>` est un conteneur associatif **trié, contenant uniquement des clés**. Implémentation : un **arbre bicolore (red-black tree)**. Chaque opération (insert, erase, find) est en O(log n). Les itérateurs sont **bidirectionnels, stables, et parcourent l'arbre dans l'ordre de tri**. Les clés sont *immuables* via les itérateurs — modifier une clé sur place briserait l'invariant de tri. Pour un petit N ou des charges de travail de type écriture-unique-lectures-multiples, un `vector` trié + `lower_bound` surpasse souvent `set`.

Associé : [`MULTISET.md`](MULTISET.md) · [`MAP.md`](MAP.md) · [`VECTOR.md`](VECTOR.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <set>`

---

## 1. Modèle mental

Un arbre bicolore est un **arbre binaire de recherche auto-équilibré** — chaque chemin de la racine à une feuille a approximativement la même longueur, de sorte que chaque opération est en O(log n) dans le pire des cas (pas seulement en moyenne).

```
   set<int> after inserting 5, 3, 8, 1, 4, 7, 9

                    5 (B)
                   / \
                3 (R) 8 (R)
                / \   / \
            1(B) 4(B) 7(B) 9(B)

   ── B = black, R = red. Color rules keep the tree balanced.
   ── In-order traversal (left → root → right) yields: 1 3 4 5 7 8 9
```

Ce que cela signifie pour vous :
- **Chaque élément représente une allocation distincte sur le heap** (un nœud d'arbre par clé).
- **Les itérateurs parcourent l'arbre dans l'ordre de tri** — `for (it = s.begin(); it != s.end(); ++it)` visite 1, 3, 4, 5, 7, 8, 9 dans cet ordre, quel que soit l'ordre d'insertion.
- **Aucun doublon.** L'insertion d'une clé existante est une opération sans effet (et `insert` renvoie `(iterator, false)` pour vous l'indiquer).
- **Les clés sont triées à l'aide de `operator<` par défaut** (ou via un comparateur que vous fournissez).

---

## 2. Construction

```cpp
#include <set>
#include <functional>          // pour std::greater

std::set<int> a;                                    // vide, tri croissant par defaut
std::set<int> b(arr, arr + 10);                     // depuis une plage — deduplique et trie automatiquement
std::set<int> c(b);                                  // copie

std::set<int, std::greater<int> > d;                // tri decroissant

// Comparateur personnalise
struct ByLength {
    bool operator()(const std::string &a, const std::string &b) const {
        return a.size() < b.size();
    }
};
std::set<std::string, ByLength> e;
```

**Le comparateur doit être un ordre faible strict (strict weak ordering).** `comp(a,b)` et `comp(b,a)` ne peuvent pas être tous les deux vrais. Si l'on a à la fois `!comp(a,b)` et `!comp(b,a)`, `set` les considère comme équivalents — ce qui signifie que **seul l'un d'entre eux au maximum réside dans le set**.

---

## 3. API principale

| Méthode                   | Ce qu'elle fait                                             | Coût       |
|---------------------------|-------------------------------------------------------------|------------|
| `insert(x)`               | Insère si absent. Renvoie `pair<iterator, bool>`.           | O(log n)   |
| `insert(it, x)`           | Idem, avec un itérateur d'indication/hint (proche du point d'insertion). | O(1) amorti si le hint est correct |
| `insert(first, last)`     | Insertion en masse d'une plage.                             | O(m log n) |
| `erase(it)`               | Supprime l'élément pointé.                                  | O(1) amorti |
| `erase(key)`              | Supprime l'élément égal à `key`. Renvoie 1 si supprimé, 0 si absent. | O(log n) |
| `erase(first, last)`      | Supprime une plage.                                         | O(log n + m) |
| `find(key)`               | Renvoie l'itérateur, ou `end()` s'il n'est pas trouvé.       | O(log n)   |
| `count(key)`              | 0 ou 1 pour `set` (il est unique). Utilisez `find` si possible — même coût, sémantiquement plus clair. | O(log n) |
| `lower_bound(key)`        | Premier élément ≥ `key`.                                    | O(log n)   |
| `upper_bound(key)`        | Premier élément > `key`.                                    | O(log n)   |
| `equal_range(key)`        | `pair(lower_bound, upper_bound)`. Pour `set`, la plage contient 0 ou 1 élément. | O(log n) |
| `clear()`                 | Détruit chaque nœud. O(n).                                  | O(n)       |
| `size()` / `empty()`      | O(1).                                                       | O(1)       |
| `begin()` / `end()` / `rbegin()` / `rend()` | Itérateurs bidirectionnels dans l'ordre de tri. | O(1)       |

**Pas de `operator[]`, pas de `at`, pas d'accès aléatoire.** Le set est trié par *valeur*, non indexé par position.

---

## 4. La valeur de retour `pair<iterator, bool>` de `insert`

C'est la pièce maîtresse :

```cpp
std::set<int> s;
std::pair<std::set<int>::iterator, bool> r = s.insert(42);
if (r.second)
    std::cout << "newly inserted\n";
else
    std::cout << "was already present\n";
// dans les deux cas, r.first pointe vers l'element de valeur 42
```

**Pourquoi une pair et pas simplement un bool ?** Parce que vous fournir l'itérateur ne coûte rien de plus (l'arbre a justement navigué jusqu'à cet emplacement pour prendre sa décision), et que vous souhaitez souvent effectuer immédiatement une action avec l'élément inséré ou existant.

---

## 5. Stabilité des itérateurs — le super-pouvoir discret de set

| Opération             | Itérateurs / références invalidés ?                             |
|-----------------------|-----------------------------------------------------------------|
| `insert(x)`           | **Aucun.** Tous les itérateurs existants restent valides.       |
| `erase(it)`           | Uniquement `it` lui-même.                                       |
| `erase(key)`          | Uniquement l'itérateur vers cette clé (si elle existait).       |
| `clear()`             | Tous les itérateurs (chaque nœud est libéré).                   |
| `swap(other)`         | Aucun.                                                          |

Il s'agit d'une garantie plus forte que celle de vector : dans un `set`, vous pouvez conserver un itérateur à travers **n'importe quel nombre d'insertions** et il continuera de pointer vers le même élément. Les nœuds de l'arbre ne sont jamais déplacés — leurs liaisons sont simplement modifiées.

---

## 6. Les clés sont immuables via les itérateurs

```cpp
std::set<int>::iterator it = s.find(5);
// *it = 99;       // ERREUR — *it est `const int &`
                    // (libstdc++/libc++ le definissent comme `const T` selon la specification)
```

Si vous pouviez modifier la clé, vous violeriez l'ordre de tri — l'élément se retrouverait à une mauvaise position dans l'arbre, et find/insert renverraient des résultats incorrects. La norme empêche cela en fournissant un accès `const` à travers les itérateurs.

Pour "modifier" un élément d'un set : appliquez `erase` dessus, puis faites un `insert` de la nouvelle valeur.

---

## 7. Conseils d'efficacité

### 7.1 Utiliser `lower_bound` / `upper_bound` pour les requêtes par plage

```cpp
// Trouver tous les elements dans [10, 20]
std::set<int>::iterator it = s.lower_bound(10);
std::set<int>::iterator end = s.upper_bound(20);
for (; it != end; ++it)
    std::cout << *it << " ";
```

`find(key) == end()` vérifie si « cet élément exact est présent » ; `lower_bound(key)` répond à « où se placerait-il » — ce qui est également utile pour un « insert avec hint ».

### 7.2 Utiliser `insert` avec hint pour une insertion en masse déjà triée

```cpp
std::set<int>::iterator hint = s.end();
for (std::size_t i = 0; i < sorted_data.size(); ++i)
    hint = s.insert(hint, sorted_data[i]);   // O(1) amorti si le hint est correct
```

Pour des données dont on sait déjà qu'elles sont triées, cela transforme une insertion groupée en `n log n` en environ O(n).

### 7.3 Si vous construisez une seule fois pour interroger plusieurs fois — le vector trié l'emporte

```cpp
std::vector<int> v(arr, arr + n);
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
// ...
bool found = std::binary_search(v.begin(), v.end(), key);
```

Même recherche en O(log n), mais **une seule allocation** contre n allocations, et une **mémoire contiguë** permettant aux accès cache de dominer largement. Pour n < ~1000, cela bat souvent `std::set` d'un facteur 3 à 10× dans les mesures réelles.

Utilisez `std::set` lorsque vous entrelacez des insertions et des recherches ; utilisez un vector trié lorsque la phase de construction se termine avant que la phase de requête ne commence.

### 7.4 `count(key)` vs `find(key)` — préférez `find`

Les deux sont en O(log n). `find` est sémantiquement plus clair et vous donne l'itérateur. N'utilisez `count` que lorsque vous voulez littéralement juste le booléen (et même dans ce cas, `find(k) != end()` est plus lisible).

### 7.5 Passage par `const &`

Copier un set copie chaque nœud — soit n allocations. Passez toujours par reference.

---

## 8. Quand NE PAS utiliser set

| Si vous avez besoin de…                            | Choisissez…                                    |
|----------------------------------------------------|------------------------------------------------|
| Clés en double                                     | `multiset`                                     |
| Association clé → valeur                           | `map`                                          |
| Recherche en O(1) (sans importance pour l'ordre)   | `unordered_set` du C++11+ — **interdit en C++98 / 42** |
| Accès trié mais écriture unique / lectures multiples | `vector` trié + `binary_search`              |
| Parcours selon l'ordre d'insertion                 | `vector` (dédoublonné manuellement si besoin)  |

---

## 9. Particularités C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_set`   | C++11       | Utiliser `std::set` (O(log n) au lieu de O(1) en moy.). |
| `emplace` / `emplace_hint` | C++11   | `insert` — implique une copie.              |
| Move semantics         | C++11       | Passer par `const &`.                       |
| Initializer-list ctor  | C++11       | Construire via le ctor de plage depuis un tableau C temporaire. |
| Comparateurs lambda    | C++11       | Struct de foncteur avec `operator()`.       |
---

## 10. Pièges

- **Les keys sont `const` à travers les iterators.** Vous ne pouvez pas faire `*it = x;`. Faites erase + insert si vous devez modifier.
- **Le comparator doit être un strict weak ordering.** Renvoyer true pour à la fois `comp(a,b)` et `comp(b,a)` est un UB.
- **Deux keys sont « équivalentes » si aucune n'est `<` l'autre** — pas nécessairement égales via `operator==`. Avec un comparator comme `ByLength`, `"abc"` et `"def"` sont équivalents et seul le premier inséré subsiste.
- **`insert` renvoie `pair<iterator, bool>`.** Oublier `.first` pour récupérer l'iterator est un bug fréquent.
- **Chaque node est une allocation heap distincte.** Les sets contenant des millions de petites keys ont un overhead mémoire énorme comparé à un vector trié.
- **`erase(key)` renvoie le nombre d'éléments supprimés** (toujours 0 ou 1 pour un set), pas un iterator. `erase(it)` renvoie `void` en C++98 (il ne renvoie le prochain iterator qu'à partir de C++11). Pour supprimer pendant l'itération, le pattern C++98 sûr est :
  ```cpp
  for (std::set<int>::iterator it = s.begin(); it != s.end(); ) {
      if (should_erase(*it)) {
          std::set<int>::iterator dead = it++;
          s.erase(dead);
      } else {
          ++it;
      }
  }
  ```

---

## 11. Exemple pratique — mots uniques triés

```cpp
#include <set>
#include <string>
#include <iostream>

int main(void)
{
    const char *words[] = {"pear", "apple", "banana", "apple", "cherry", "pear"};
    std::set<std::string> unique_sorted(words, words + 6);

    for (std::set<std::string>::const_iterator it = unique_sorted.begin();
         it != unique_sorted.end(); ++it)
        std::cout << *it << "\n";
    // apple
    // banana
    // cherry
    // pear

    if (unique_sorted.find("apple") != unique_sorted.end())
        std::cout << "found apple\n";

    std::pair<std::set<std::string>::iterator, bool> r = unique_sorted.insert("date");
    std::cout << (r.second ? "added date\n" : "date was already there\n");
    return 0;
}
```

Construisez le set à partir d'un range (dédoublonne et trie automatiquement), itérez dans l'ordre trié, recherchez par key, observez le retour `pair<iterator, bool>`. Chaque opération est en O(log n).

---

> **Résumé en une ligne.** `std::set` est un container trié, à keys uniques, en log-n, reposant sur un red-black tree. Les iterators restent valides lors des inserts ; les keys sont immutables à travers eux. Pour les datasets petits ou créés en une seule fois, un vector trié + binary_search est souvent plus rapide.