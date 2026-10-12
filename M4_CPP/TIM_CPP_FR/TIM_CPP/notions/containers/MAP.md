# `std::map` — Recherche Triée Clé → Valeur

> **TL;DR.** `std::map<K, V>` est un **conteneur associatif trié** associant des `K` uniques à des `V`. Même arbre rouge-noir que `std::set`, mais chaque nœud stocke un `std::pair<const K, V>` au lieu d'une simple clé. Toutes les opérations sont en O(log n). Killer feature : requêtes par plage via `lower_bound` (le moteur derrière le BitcoinExchange du CPP09). Piège mortel : `m[k]` **insère silencieusement une valeur construite par défaut** lorsque `k` est manquant — presque toujours un bug si vous vouliez seulement effectuer une recherche.

Connexe : [`SET.md`](SET.md) · [`MULTIMAP.md`](MULTIMAP.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <map>`

---

## 1. Modèle mental

Un arbre rouge-noir de nœuds `pair<const K, V>`, ordonnés par `K` :

```
   map<std::string, int>  after inserting ("apple", 3), ("banana", 7), ("cherry", 2)

                  ("banana", 7)
                  /            \
       ("apple", 3)         ("cherry", 2)

   In-order traversal yields: ("apple", 3) ("banana", 7) ("cherry", 2)
   — sorted by key, regardless of insertion order.
```

Le type d'élément est **`std::pair<const K, V>`** — notez le `const` sur la clé. Même logique que pour `set` : modifier la clé placerait le nœud à une mauvaise position dans l'arbre. La valeur `V` est librement modifiable.

```cpp
std::map<std::string, int>::iterator it = m.find("apple");
// it->first  est `const std::string &`  — lecture seule
// it->second est `int &`                 — modifiable
it->second = 99;        // OK
// it->first = "x";     // ERREUR
```

---

## 2. Construction

```cpp
#include <map>

std::map<std::string, int> a;                              // vide

// Depuis une plage de paires (une allocation par element)
std::pair<%% std %%::string, int> seed[] = {
    std::make_pair("apple", 3),
    std::make_pair("banana", 7)
};
std::map<std::string, int> b(seed, seed + 2);

std::map<std::string, int> c(b);                           // copie

// Comparateur personnalise — doit ordonner par CLE uniquement
std::map<std::string, int, std::greater<std::string> > d;  // ordre decroissant des cles
```

**Pas de constructeur par initializer-list en C++98.** Construisez un tableau temporaire et utilisez le constructeur par plage.

---

## 3. Les quatre façons d'insérer

| Forme                                         | Retourne                               | Quand l'utiliser                       |
|-----------------------------------------------|----------------------------------------|----------------------------------------|
| `m.insert(std::make_pair(k, v))`              | `pair<iterator, bool>` — le booléen indique si l'élément était nouveau | Vous voulez savoir « ai-je écrasé ou ajouté un nouveau ? » |
| `m.insert(it, std::make_pair(k, v))`          | `iterator` — utilise un hint           | Insertion en masse de paires presque triées. |
| `m[k] = v`                                    | `void` (en réalité, une référence à l'emplacement assigné) | Assignation rapide, **insère un `V` par défaut si `k` était absent.** |
| `m[k]`                                        | `V &`                                  | « Obtenir ou construire par défaut. » Même piège d'insertion en cas d'absence. |

La première forme est la plus sûre. La forme avec crochets est pratique mais traître — voir §5.

---

## 4. API principale

| Méthode                   | Retour / comportement                                         | Coût       |
|---------------------------|---------------------------------------------------------------|------------|
| `m[k]`                    | Référence vers la valeur pour `k`. **Insère par défaut si absent.** | O(log n)   |
| `m.find(k)`               | `iterator` vers la paire, ou `end()` si manquante. Recherche pure. | O(log n)   |
| `m.count(k)`              | 0 ou 1 (les clés de map sont uniques).                         | O(log n)   |
| `m.insert(pair)`          | `pair<iterator, bool>`. `bool == false` signifie clé déjà présente (pas d'écrasement). | O(log n) |
| `m.erase(it)`             | Supprime. Retourne void en C++98.                             | O(1) amorti |
| `m.erase(k)`              | Supprime si présent. Retourne le nombre d'éléments supprimés (0 ou 1). | O(log n)   |
| `m.lower_bound(k)`        | Premier élément dont la clé ≥ `k`.                            | O(log n)   |
| `m.upper_bound(k)`        | Premier élément dont la clé > `k`.                            | O(log n)   |
| `m.equal_range(k)`        | Paire `(lower_bound, upper_bound)`. La plage est de 0 ou 1 élément. | O(log n)   |
| `m.size()` / `m.empty()`  |                                                                | O(1)       |
| `m.clear()`               | Libère chaque nœud.                                           | O(n)       |
| `m.begin()` / `end()` / `rbegin()` / `rend()` | Iterators bidirectionnels ordonnés selon les clés. | O(1)       |

> **`at(k)` est en C++11.** En C++98, l'équivalent le plus proche pour une recherche pure est `find(k)` — et vous devez comparer avec `end()`.

---

## 5. Le piège de l'insertion en cas d'absence de `m[k]`

```cpp
std::map<std::string, int> counts;
counts["apple"] += 1;             // counts = {"apple" → 1}
counts["apple"] += 1;             // counts = {"apple" → 2}
                                   // parfait pour les histogrammes.

// Mais :
if (counts["banana"] == 0)         // ← CECI A INSERE ("banana", 0)
    std::cout << "no bananas\n";
std::cout << counts.size() << "\n";  // 2 — banana est maintenant dans la map
```

**Chaque accès `m[k]` crée l'entrée si elle n'existe pas.** C'est délibéré — c'est ce qui permet à `counts[word]++` de fonctionner pour un histogramme. Mais si vous vouliez seulement *vérifier*, vous venez de polluer la map.

**Le pattern de recherche pure :**

```cpp
std::map<std::string, int>::const_iterator it = counts.find("banana");
if (it == counts.end())
    std::cout << "no bananas\n";   // n'insere PAS
else
    std::cout << it->second << "\n";
```

**Règle empirique :** utilisez `m[k]` pour *écrire* ; utilisez `m.find(k)` pour *lire*. Si vous inversez ces usages, vous insérerez des éléments fantômes.

### Que fait réellement `m[k]` ?

```cpp
V& operator[](const K &k) {
    iterator it = lower_bound(k);
    if (it == end() || comp(k, it->first))
        it = insert(it, std::make_pair(k, V()));   // ctor par defaut de V
    return it->second;
}
```

Deux points à retenir :
- `V` **doit être default-constructible** pour utiliser `m[k]`. Si `V` n'a pas de constructeur par défaut, la forme avec crochets ne compilera pas.
- Chaque recherche `m[k]` coûte une descente d'arbre — identique à `find`. Ne réutilisez pas les crochets deux fois de suite sur la même clé si le coût vous préoccupe.

---

## 6. Invalidation des iterators — identique à set

| Opération             | Iterators / references invalidés ?               |
|-----------------------|--------------------------------------------------|
| `insert`              | Aucun.                                           |
| `erase(it)`           | Seulement `it`.                                  |
| `erase(k)`            | Seulement l'iterator pointant vers `k` (s'il était présent). |
| `clear()`             | Tous.                                            |
| `swap(other)`         | Aucun.                                           |

Les iterators sont stables à travers les insertions. Même arbre rouge-noir que set.

---

## 7. Conseils d'efficacité

### 7.1 Utiliser `lower_bound` pour la « clé précédente la plus proche » — le pattern BitcoinExchange

Le sujet du CPP09 ex00 demande : étant donné une date, trouver la date la plus proche *antérieure ou égale* à celle-ci dans une base de données de prix. C'est exactement ce que `lower_bound` + un ajustement d'un pas en arrière vous offre :

```cpp
std::map<std::string, double>::const_iterator it = prices.lower_bound(query_date);
if (it == prices.end() || it->first != query_date) {
    if (it == prices.begin())
        return /* pas de date anterieure */;
    --it;                           // recule vers la date strictement anterieure
}
return it->second;
```

Trié par clé + dichotomie en log-n = idéal pour la recherche dans des séries temporelles.

### 7.2 Utiliser `insert` (pas `[]`) lorsque vous voulez « ajouter uniquement si absent »

```cpp
// SUR : laisse la valeur existante intacte si "apple" en a deja une.
m.insert(std::make_pair("apple", 5));

// NON SUR : ecrase silencieusement
m["apple"] = 5;
```

### 7.3 Itérer dans l'ordre des clés

L'ordre d'itération par défaut **est** trié par clé. Si c'est ce que vous souhaitez, aucune étape de tri supplémentaire n'est requise. Si vous voulez l'ordre d'insertion, il vous faut un `vector<pair<K,V>>`.

### 7.4 Construire une fois + itérer souvent → envisagez un `vector<pair<K,V>>` trié

Même compromis que set vs vecteur trié. Si la map est construite une fois au démarrage puis interrogée de nombreuses fois, un vector trié est plus rapide (une seule allocation, mémoire contiguë, `lower_bound` fonctionne également dessus).

### 7.5 Passer par `const &`

Les copies de map sont en O(n) et allouent un nœud par élément. Passez toujours par reference.

---

## 8. Quand NE PAS utiliser map

| Si vous avez besoin de…                            | Choisissez…                                    |
|----------------------------------------------------|------------------------------------------------|
| Clés dupliquées                                    | `multimap`                                     |
| Recherche moyenne en O(1), aucun ordre nécessaire  | `unordered_map` — **C++11, interdit dans les modules C++98 de 42** |
| Suivi de l'ordre d'insertion                       | `vector<pair<K,V>>`                            |
| Comptage (histogramme)                             | `map<K, int>` convient parfaitement — c'est l'usage canonique |
| Regroupement (group-by)                            | `map<K, vector<V> >` est généralement plus clair que `multimap` |
---

## 9. Pièges et limites en C++98

| Feature                | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_map`   | C++11       | Utiliser `std::map` (O(log n) au lieu de O(1) en moyenne). |
| `at(k)`                | C++11       | `find(k)` + vérification `!= end()`.        |
| `emplace`              | C++11       | `insert(make_pair(k, v))` — coûte une copie. |
| Initializer-list ctor `{{a, 1}, {b, 2}}` | C++11 | Construire un tableau de pairs, utiliser le range ctor. |
| Move semantics         | C++11       | Passer par `const &`.                       |

---

## 10. Pièges courants

- **`m[k]` insère en cas d'absence.** Utilisez `find(k)` pour une simple recherche.
- **Le type d'élément est `pair<const K, V>`.** Vous ne pouvez pas réassigner une clé via un iterator.
- **`V` doit être default-constructible** pour pouvoir utiliser `operator[]`.
- **`erase(it)` renvoie `void` en C++98** (C++11 renvoie l'iterator suivant). L'idiome pour effacer pendant l'itération nécessite l'astuce de la post-incrémentation :
  ```cpp
  for (std::map<K, V>::iterator it = m.begin(); it != m.end(); ) {
      if (should_erase(it->first)) {
          std::map<K, V>::iterator dead = it++;
          m.erase(dead);
      } else {
          ++it;
      }
  }
  ```
- **`insert(make_pair(k, v))` n'écrase PAS** une entrée existante. Le booléen dans la pair retournée vous indique si l'insertion a eu lieu.
- **Chaque entrée est une allocation sur la heap.** Le coût mémoire est significatif pour un grand nombre de petites entrées.
- **Les comparateurs personnalisés doivent définir un strict weak ordering sur la clé uniquement.** Ils n'ont jamais accès à la valeur.

---

## 11. Exemple complet — histogramme de mots + requête par plage

```cpp
#include <map>
#include <string>
#include <iostream>

int main(void)
{
    std::map<std::string, int> counts;
    const char *words[] = {"the", "cat", "sat", "the", "mat", "the", "cat"};

    // Histogramme via le pattern crochet-insertion (c'est le point fort de [])
    for (int i = 0; i < 7; ++i)
        counts[words[i]] += 1;

    // Itération dans l'ordre trié par clé
    for (std::map<std::string, int>::const_iterator it = counts.begin();
         it != counts.end(); ++it)
        std::cout << it->first << ": " << it->second << "\n";
    // cat: 2
    // mat: 1
    // sat: 1
    // the: 3

    // Recherche pure — NE PAS utiliser []
    std::map<std::string, int>::const_iterator q = counts.find("dog");
    std::cout << (q == counts.end() ? "no dog\n" : "found\n");
    std::cout << "size still: " << counts.size() << "\n";   // toujours 4

    // "Clé la plus proche non supérieure à" — le pattern BitcoinExchange
    std::map<std::string, int>::const_iterator lo = counts.lower_bound("mau");
    if (lo != counts.begin()) {
        if (lo == counts.end() || lo->first != "mau")
            --lo;
        std::cout << "nearest <= mau: " << lo->first << "\n";    // mat
    }
    return 0;
}
```

L'histogramme met en valeur la force de la notation entre crochets. La « clé précédente la plus proche » met en avant celle de `lower_bound`. Les deux opérations s'exécutent en O(log n).

---

> **Résumé en une ligne.** `std::map` est une structure `K → V` triée avec des opérations en O(log n). N'utilisez `m[k]` que pour l'*écriture* (il insère si la clé est absente) ; utilisez `find(k)` pour la *lecture*. `lower_bound` est le moteur des recherches dans les séries temporelles triées. Le type d'élément est `pair<const K, V>` — les clés sont immuables.