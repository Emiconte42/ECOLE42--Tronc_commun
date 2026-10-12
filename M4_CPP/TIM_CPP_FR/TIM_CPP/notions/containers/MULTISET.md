# `std::multiset` — Clés triées avec doublons

> **TL;DR.** `std::multiset<T>` est un `std::set<T>` sans la contrainte d'unicité. Même arbre rouge-noir, mêmes opérations en O(log n), même stabilité des iterators — mais **insérer une clé existante ajoute un doublon** au lieu d'être une opération sans effet (no-op). Deux conséquences : `insert` renvoie simplement un `iterator` (pas de `bool`), et `erase(key)` supprime **tous les éléments correspondants**, pas un seul.

Associé : [`SET.md`](SET.md) · [`MULTIMAP.md`](MULTIMAP.md) · [`MAP.md`](MAP.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <set>` *(oui — le même header que `std::set`)*

---

## 1. Modèle mental

Même arbre que set, mais un nœud peut avoir des éléments frères avec des clés équivalentes :

```
   multiset<int> after inserting 5, 3, 8, 5, 4, 5

                    5
                   / \
                3    5
                 \    \
                  4    8
                       /
                      5

   In-order traversal yields: 3 4 5 5 5 8
   Notice: three 5s, all kept.
```

Les éléments équivalents sont stockés **dans un ordre stable** (leur ordre d'insertion) depuis C++11. En C++98, l'ordre parmi les éléments équivalents est **défini par l'implémentation** (*implementation-defined*) — généralement l'ordre d'insertion dans libstdc++/libc++, mais ne vous y fiez pas.

---

## 2. Les deux différences d'API par rapport à `set`

### 2.1 `insert(x)` renvoie simplement un `iterator`

```cpp
std::set<int>      s; std::pair<std::set<int>::iterator, bool>  r1 = s.insert(5);
std::multiset<int> m; std::multiset<int>::iterator              r2 = m.insert(5);
```

Il n'y a pas de booléen à renvoyer — l'insertion réussit toujours (même si des doublons existent). L'iterator pointe vers le nœud nouvellement inséré.

### 2.2 `erase(key)` supprime **chaque** élément correspondant

```cpp
std::multiset<int> m;
m.insert(5); m.insert(5); m.insert(5);
std::cout << m.erase(5) << "\n";      // 3 — erased all three
std::cout << m.size()    << "\n";      // 0
```

Si vous souhaitez n'en supprimer **qu'un seul**, utilisez `erase(iterator)` après `find` :

```cpp
std::multiset<int>::iterator it = m.find(5);
if (it != m.end())
    m.erase(it);                        // erases just that one node
```

C'est le piège silencieux lors du portage de `set` vers `multiset`. `m.erase(5)` sur un multiset contenant `{5, 5, 5, 7}` laisse `{7}`, et non `{5, 5, 7}`.

---

## 3. `count` et `equal_range` sont vraiment utiles ici

Pour `set`, `count(key)` vaut toujours 0 ou 1. Pour `multiset`, c'est « combien d'occurrences de cette clé » — et `equal_range(key)` vous donne la paire d'iterators pour parcourir précisément celles-ci :

```cpp
std::multiset<int> m;
m.insert(1); m.insert(2); m.insert(2); m.insert(2); m.insert(3);

std::cout << m.count(2) << "\n";               // 3

std::pair<std::multiset<int>::iterator, std::multiset<int>::iterator>
    r = m.equal_range(2);
for (std::multiset<int>::iterator it = r.first; it != r.second; ++it)
    std::cout << *it << " ";                    // 2 2 2
std::cout << "\n";
```

`equal_range(k)` n'est rien d'autre que `(lower_bound(k), upper_bound(k))` regroupés ensemble. Pour `multiset`, cela vous donne la séquence contiguë de clés équivalentes en O(log n + m).

---

## 4. API principale (différences avec set en **gras**)

| Méthode                   | Retour / comportement                                         | Coût       |
|---------------------------|---------------------------------------------------------------|------------|
| `insert(x)`               | **`iterator` (pas de bool — réussit toujours).**              | O(log n)   |
| `insert(it, x)`           | Idem, avec hint.                                              | O(1) amorti si le hint est correct |
| `insert(first, last)`     | Insertion en masse d'une plage.                               | O(m log n) |
| `erase(it)`               | Supprime un seul élément.                                     | O(1) amorti |
| `erase(key)`              | **Supprime TOUS les éléments correspondants. Renvoie le nombre d'éléments supprimés.** | O(log n + m) où m = correspondances |
| `find(key)`               | Iterator vers **un** élément correspondant (n'importe lequel). | O(log n)   |
| `count(key)`              | **Nombre de correspondances (peut être > 1).**                 | O(log n + m) |
| `lower_bound(key)`        | Premier ≥ `key`.                                              | O(log n)   |
| `upper_bound(key)`        | Premier > `key`.                                              | O(log n)   |
| `equal_range(key)`        | **Paire d'iterators vers la séquence de clés équivalentes.**  | O(log n)   |
| Autres opérations         | Identiques à `set`.                                           | Identique  |

---

## 5. Invalidation des iterators — identique à set

Garanties identiques : `insert` n'invalide rien ; `erase(it)` invalide uniquement `it`. Vous pouvez conserver des iterators à travers les mutations.

---

## 6. Conseils d'efficacité

### 6.1 Pour supprimer une seule correspondance, utilisez `find` + `erase(iterator)`

```cpp
// FAUX si vous ne voulez en supprimer qu'un seul — supprime toutes les correspondances
m.erase(5);

// CORRECT — n'en supprime qu'un seul
std::multiset<int>::iterator it = m.find(5);
if (it != m.end()) m.erase(it);
```

### 6.2 `equal_range` est préférable à deux appels distincts `lower_bound`/`upper_bound`

Même complexité, mais `equal_range` effectue les deux recherches en une seule descente de l'arbre :

```cpp
std::pair<It, It> r = m.equal_range(key);     // one descent
// vs
It lo = m.lower_bound(key);
It hi = m.upper_bound(key);                    // two descents
```

### 6.3 Si vous cherchez principalement « toutes les valeurs de la clé K », envisagez une structure différente

Un `std::map<Key, std::vector<Value> >` est souvent plus clair et plus rapide qu'un `multimap` (le cousin de `multiset`/`multimap`). Pour du simple comptage, `std::map<Key, int>` (un histogramme) est encore meilleur.

### 6.4 La forme avec hint pour l'insertion en masse triée

Même astuce que pour set : passez un hint correct pour transformer une insertion en masse de O(n log n) en O(n).

---

## 7. Quand NE PAS utiliser multiset

| Si vous avez besoin de…                            | Choisissez…                                         |
|----------------------------------------------------|-----------------------------------------------------|
| Compter les occurrences                            | `std::map<Key, int>` — plus clair et même complexité. |
| Regrouper des valeurs par clé                      | `std::map<Key, std::vector<Value> >`.               |
| Recherche rapide sans ordonnancement               | (C++11 `unordered_multiset` — interdit en C++98)    |
| Suivre l'ordre d'insertion                         | `std::vector` de paires, à trier ensuite.           |

---

## 8. Mises en garde spécifiques à C++98

Identiques à `set` :

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_multiset` | C++11    | Utiliser `std::multiset`.                   |
| `emplace`              | C++11       | `insert` — paye une copie.                  |
| Sémantique de déplacement | C++11    | Passage par `const &`.                      |
| Ctor avec initializer-list | C++11   | Ctor par plage depuis un tableau C temporaire. |

---

## 9. Pièges courants

- **`erase(key)` supprime TOUTES les correspondances.** C'est la source n°1 de bugs avec multiset. Utilisez `erase(find(key))` pour n'en supprimer qu'une seule.
- **`insert` renvoie un `iterator`, pas une `pair<iterator, bool>`.** Le code qui suppose l'existence de `.second` ne compilera pas lors du portage de `set` vers `multiset`.
- **`find(key)` renvoie *une* correspondance, pas une correspondance précise.** Si vous avez besoin d'une position particulière, utilisez `equal_range` et parcourez les éléments.
- **L'ordre entre clés équivalentes est défini par l'implémentation en C++98.** Ne dépendez pas de l'ordre d'insertion — cette garantie n'apparaît qu'en C++11+.
- **Même coût d'allocation sur le heap par nœud que set.** L'overhead mémoire pour de nombreuses petites clés est significatif.
- **Les clés sont immuables à travers les iterators** — tout comme dans set.

---

## 10. Exemple pratique — regroupement d'anagrammes

```cpp
#include <set>
#include <string>
#include <algorithm>
#include <iostream>

int main(void)
{
    // multiset stores all words; sorted by signature (sorted letters).
    // Custom comparator sorts by canonical form so anagrams cluster.
    struct ByCanonical {
        static std::string canonical(const std::string &s) {
            std::string c = s;
            std::sort(c.begin(), c.end());
            return c;
        }
        bool operator()(const std::string &a, const std::string &b) const {
            return canonical(a) < canonical(b);
        }
    };

    std::multiset<std::string, ByCanonical> bag;
    const char *words[] = {"listen", "silent", "enlist", "google", "gogole", "hello"};
    bag.insert(words, words + 6);

    // All anagrams of "listen" land in equal_range:
    std::pair<std::multiset<std::string, ByCanonical>::iterator,
              std::multiset<std::string, ByCanonical>::iterator>
        r = bag.equal_range("listen");

    for (; r.first != r.second; ++r.first)
        std::cout << *r.first << " ";
    std::cout << "\n";        // listen silent enlist  (anagrams of "listen")
    return 0;
}
```

Le comparateur personnalisé trie par « signature de lettres », de sorte que les anagrammes sont considérés comme équivalents et se regroupent dans l'arbre. `equal_range` permet ensuite de parcourir ce groupe.

---

> **Résumé en une ligne.** `std::multiset` est un `set` autorisant les doublons. Deux pièges d'API : `insert` renvoie simplement un iterator, et `erase(key)` supprime **toutes** les correspondances. `count`, `equal_range` et `lower_bound`/`upper_bound` sont les outils du quotidien.