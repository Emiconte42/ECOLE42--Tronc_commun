# `std::multimap` — Clé triée → Valeur avec clés dupliquées

> **TL;DR.** `std::multimap<K, V>` est un `std::map<K, V>` dont la contrainte d'unicité a été retirée. Même arbre rouge-noir de `pair<const K, V>`, mêmes opérations en O(log n) — mais **la même clé peut apparaître plusieurs fois**, chacune pointant vers une valeur `V` (potentiellement différente). Deux conséquences : il n'y a **pas d'`operator[]`** (quelle clé retournerait-il ?), et `equal_range(k)` devient l'outil du quotidien pour « donne-moi toutes les valeurs associées à `k` ».

Voir aussi : [`MAP.md`](MAP.md) · [`MULTISET.md`](MULTISET.md) · [`SET.md`](SET.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <map>` *(oui — le même header que `std::map`)*

---

## 1. Modèle mental

Même arbre que `map`, mais plusieurs nœuds peuvent partager une clé :

```
   multimap<std::string, int> after inserting:
     ("cat", 1), ("dog", 4), ("cat", 2), ("cat", 5), ("ant", 9)

                   ("cat", 1)
                   /          \
            ("ant", 9)    ("cat", 2)
                                \
                            ("cat", 5)
                                  \
                              ("dog", 4)

   In-order: ("ant", 9) ("cat", 1) ("cat", 2) ("cat", 5) ("dog", 4)
```

Toutes les entrées `("cat", *)` sont regroupées ensemble dans le parcours infixe (in-order). C'est ce que retourne `equal_range("cat")`.

---

## 2. Les différences par rapport à `map`

### 2.1 Pas d'`operator[]`

```cpp
std::map<std::string, int>      m;  m["cat"] = 5;     // OK
std::multimap<std::string, int> mm; mm["cat"] = 5;    // ERROR — no operator[]
```

Il n'y a aucun moyen de définir `mm[k]` de manière cohérente lorsque plusieurs entrées `("cat", *)` existent. Utilisez `insert` à la place.

### 2.2 `insert` retourne seulement un `iterator`

```cpp
std::map<K,V>::iterator       r1; std::pair<r1, bool> = m.insert(...);
std::multimap<K,V>::iterator  r2 =                      mm.insert(...);
```

Identique à `set` → `multiset` : l'insertion réussit toujours, pas de booléen à retourner.

### 2.3 `erase(key)` supprime TOUTES les entrées correspondantes

```cpp
mm.insert(std::make_pair("cat", 1));
mm.insert(std::make_pair("cat", 2));
mm.insert(std::make_pair("cat", 5));
std::cout << mm.erase("cat") << "\n";    // 3
```

Tout comme `multiset`. Pour supprimer une seule entrée spécifique, utilisez `erase(iterator)` :

```cpp
std::multimap<std::string, int>::iterator it = mm.find("cat");
if (it != mm.end()) mm.erase(it);        // removes only one
```

`find(k)` retourne un iterator vers **l'une** des entrées correspondantes — généralement la première dans l'ordre de l'arbre, mais le standard ne garantit pas laquelle lorsqu'il en existe plusieurs.

---

## 3. `equal_range` est l'outil du quotidien

La raison même d'utiliser un multimap : « donne-moi chaque valeur associée à la clé K. »

```cpp
std::pair<std::multimap<std::string, int>::iterator,
          std::multimap<std::string, int>::iterator>
    r = mm.equal_range("cat");

for (std::multimap<std::string, int>::iterator it = r.first;
     it != r.second; ++it)
    std::cout << it->first << " → " << it->second << "\n";
// cat → 1
// cat → 2
// cat → 5
```

`equal_range(k)` correspond à `(lower_bound(k), upper_bound(k))` en une seule descente de l'arbre. Coût : O(log n) plus O(m) pour parcourir les m correspondances.

---

## 4. API principale (les différences par rapport à map en **gras**)

| Méthode                   | Retour / comportement                                         | Coût       |
|---------------------------|---------------------------------------------------------------|------------|
| `insert(pair)`            | **`iterator` (pas de bool — réussit toujours).**              | O(log n)   |
| `insert(it, pair)`        | Identique avec hint.                                          | O(1) amorti si le hint est correct |
| `insert(first, last)`     | Insertion en masse d'une plage.                               | O(m log n) |
| `erase(it)`               | Supprime une entrée.                                          | O(1) amorti |
| `erase(key)`              | **Supprime TOUTES les correspondances. Retourne le nombre d'éléments supprimés.** | O(log n + m) |
| `find(key)`               | Iterator vers **une** entrée correspondante.                  | O(log n)   |
| `count(key)`              | **Nombre d'entrées avec cette clé (peut être > 1).**          | O(log n + m) |
| `equal_range(key)`        | **Paire d'iterators délimitant la séquence de clés égales — l'outil du quotidien.** | O(log n)   |
| `lower_bound(key)`        | Première entrée avec une clé ≥ à celle donnée.                | O(log n)   |
| `upper_bound(key)`        | Première entrée avec une clé > à celle donnée.                | O(log n)   |
| **`operator[]`**          | **N'existe pas.**                                             | —          |
| **`at(k)`**               | **N'existe pas** (et `at` est du C++11 de toute façon).       | —          |
| Autres opérations         | Identique à `map`.                                            | Identique  |

---

## 5. Invalidation des iterators — identique à map / set

`insert` n'invalide rien ; `erase(it)` invalide uniquement `it`. Les iterators pointant vers les autres entrées restent stables après les modifications. Même arbre rouge-noir.

---

## 6. Conseils d'efficacité

### 6.1 Utilisez `equal_range` pour itérer sur le groupe d'une clé

Il n'effectue qu'une seule descente de l'arbre pour les deux bornes — légèrement moins coûteux que d'appeler séparément `lower_bound(k)` + `upper_bound(k)`.

### 6.2 Choisir entre `multimap<K, V>` et `map<K, vector<V> >`

Ce sont deux représentations différentes pour la même idée. Compromis :

| Forme                          | Avantages                                       | Inconvénients                              |
|--------------------------------|-------------------------------------------------|--------------------------------------------|
| `multimap<K, V>`               | Les insertions sont en O(log n) ; chaque entrée est stable. | Parcourir toutes les valeurs nécessite `equal_range`. Mémoire : un nœud d'arbre par paire. |
| `map<K, std::vector<V> >`      | Toutes les valeurs d'une clé sont dans un seul vector contigu — optimal pour le cache lors du parcours. Intention plus claire. | Chaque nouvelle valeur peut déclencher une réallocation du vector. |

Pour « beaucoup de clés, peu de valeurs par clé » → `multimap`. Pour « peu de clés, beaucoup de valeurs par clé » → `map<K, vector<V> >`. Les deux approches fonctionnent ; la seconde est généralement plus claire lors d'une revue de code.

### 6.3 Pour supprimer une paire (clé, valeur) spécifique, parcourez `equal_range`

```cpp
std::pair<It, It> r = mm.equal_range("cat");
for (It it = r.first; it != r.second; ) {
    if (it->second == 5) {
        mm.erase(it++);                 // post-increment trick — see §10
    } else {
        ++it;
    }
}
```

`erase(key)` effacerait tous les `("cat", *)`. Parcourir equal_range vous permet de cibler une valeur spécifique.

### 6.4 Passer par `const &`

Identique à tous les autres conteneurs associatifs.

---

## 7. Quand NE PAS utiliser multimap

| Si vous avez besoin de…                            | Choisissez…                                    |
|----------------------------------------------------|------------------------------------------------|
| Clés uniques                                       | `map`                                          |
| Syntaxe `m[k]`                                     | `map`                                          |
| Groupement où chaque groupe est grand              | `map<K, vector<V> >`                           |
| Comptage (clé → occurrence)                        | `map<K, int>`                                  |
| Beaucoup de clés, peu ou une seule valeur par clé  | `map`                                          |

Si vous pouvez exprimer votre problème sous la forme `map<K, vector<V> >`, c'est presque toujours plus propre. Réservez `multimap` aux cas où la stabilité dans l'arbre de chaque nœud importe (par exemple, si vous distribuez des iterators à longue durée de vie vers des entrées individuelles).

---

## 8. Pièges et spécificités de C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_multimap` | C++11    | Utiliser `std::multimap`.                   |
| `emplace`              | C++11       | `insert(make_pair(k, v))` — paie une copie. |
| Ctor avec initializer-list | C++11   | Construire un tableau de paires, utiliser le ctor par plage (range). |
| Sémantique de déplacement (move) | C++11 | Passer par `const &`.                   |

---

## 9. Pièges à éviter

- **Pas d'`operator[]`.** `mm[k]` est une erreur de compilation. Utilisez `insert(make_pair(k, v))`.
- **`erase(key)` supprime TOUTES les entrées correspondantes.** Utilisez `erase(iterator)` après un `find` pour n'en supprimer qu'une seule — et même dans ce cas, `find` retourne *une* correspondance quelconque, pas une correspondance précise.
- **`insert` retourne uniquement un `iterator`.** Le code attendant un `pair<iterator, bool>` (style set/map) ne compilera pas.
- **`find(k)` ne précise pas quelle correspondance il a retournée.** Si vous avez besoin d'une paire (clé, valeur) spécifique, utilisez `equal_range` et effectuez une recherche.
- **L'ordre entre les doublons est défini par l'implémentation en C++98** (l'ordre d'insertion est la garantie apportée par C++11).
- **Chaque entrée représente une allocation heap distincte.** Pour « beaucoup de valeurs par clé », `map<K, vector<V> >` est généralement plus économe en mémoire.
- **`erase(it)` retourne `void` en C++98** (C++11 retourne le prochain iterator). Utilisez l'idiome de post-incrémentation :
  ```cpp
  for (it = mm.begin(); it != mm.end(); ) {
      if (should_erase(it))
          mm.erase(it++);
      else
          ++it;
  }
  ```
---

## 10. Exemple pratique — étudiant → cours

```cpp
#include <map>
#include <string>
#include <iostream>

int main(void)
{
    std::multimap<std::string, std::string> enrolled;
    enrolled.insert(std::make_pair("alice", "algebra"));
    enrolled.insert(std::make_pair("alice", "history"));
    enrolled.insert(std::make_pair("alice", "biology"));
    enrolled.insert(std::make_pair("bob",   "algebra"));
    enrolled.insert(std::make_pair("bob",   "physics"));

    // Tous les cours suivis par Alice
    std::pair<std::multimap<std::string, std::string>::iterator,
              std::multimap<std::string, std::string>::iterator>
        r = enrolled.equal_range("alice");

    std::cout << "Alice: ";
    for (std::multimap<std::string, std::string>::iterator it = r.first;
         it != r.second; ++it)
        std::cout << it->second << " ";
    std::cout << "\n";        // algebra biology history  (trié par key, puis dans l'ordre de l'arbre)

    // Combien de cours suit Bob ?
    std::cout << "Bob count: " << enrolled.count("bob") << "\n";    // 2

    // Retirer Alice de biology
    for (std::multimap<std::string, std::string>::iterator it = enrolled.lower_bound("alice");
         it != enrolled.upper_bound("alice"); ) {
        if (it->second == "biology")
            enrolled.erase(it++);          // idiome de post-increment
        else
            ++it;
    }

    std::cout << "After drop, Alice count: " << enrolled.count("alice") << "\n";  // 2
    return 0;
}
```

`equal_range` parcourt les cours d'un étudiant ; `count` répond à « combien ». Le pattern ciblé d'effacement pendant l'itération montre la forme sûre en C++98.

---

> **Résumé en une ligne.** `std::multimap` est une `map` autorisant les clés dupliquées. Pas d'`operator[]`, `insert` retourne simplement un iterator, `erase(key)` supprime toutes les correspondances, et `equal_range` est l'outil du quotidien. Souvent, `map<K, vector<V> >` constitue une alternative plus claire.