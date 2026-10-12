# `std::list` — La liste doublement chaînée

> **TL;DR.** Un `std::list<T>` est une **liste doublement chaînée de nœuds alloués sur la heap**. Chaque nœud contient une valeur et deux pointeurs (`prev`, `next`). Elle offre une insertion/suppression en O(1) **n'importe où** dès lors que l'on possède déjà un iterator, et les **iterators restent valides** lors de toute insertion ou suppression, sauf pour l'élément supprimé lui-même. Le prix à payer : pas d'accès aléatoire (`l[i]` n'existe pas), et chaque parcours génère un cache miss par nœud.

Voir aussi : [`VECTOR.md`](VECTOR.md) · [`DEQUE.md`](DEQUE.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <list>`

---

## 1. Modèle mental

C'est le `t_list *` de la libft, mais **doublement chaîné** et sous forme de template :

```
   list<int> avec trois éléments : [10, 20, 30]

       ┌─────┐      ┌─────┐      ┌─────┐
   …──→│ 10  │←────→│ 20  │←────→│ 30  │←──…
       │prev │      │prev │      │prev │
       │next │      │next │      │next │
       └─────┘      └─────┘      └─────┘
         ▲                          ▲
         │                          │
       front()                    back()

   Chaque nœud = une allocation heap, dispersée n'importe où en RAM.
```

En C avec les conventions de la libft :

```c
typedef struct s_node {
    int             value;
    struct s_node   *prev;
    struct s_node   *next;
}   t_node;
```

C'est là tout le secret. Chaque `push_back` correspond à un `malloc(sizeof(t_node))` + le réajustement de deux pointeurs. Il n'y a pas de redimensionnement, pas de realloc, pas de copie de la structure entière. C'est pourquoi les iterators restent valides : le nœud pointé par un iterator donné ne bouge jamais.

---

## 2. Construction

```cpp
#include <list>

std::list<int> a;                            // vide
std::list<int> b(5);                         // cinq éléments construits par défaut (0)
std::list<int> c(5, 42);                     // cinq 42
std::list<int> d(c);                         // copie
std::list<int> e(c.begin(), c.end());        // copie par plage (range)
std::list<int> f(arr, arr + 10);             // depuis un tableau C

a = c;
a.assign(5, 7);
a.assign(c.begin(), c.end());
```

Le même ensemble de constructeurs que `vector`. Aucune surprise ici.

---

## 3. API principale

| Méthode                       | Ce qu'elle fait                                               |
|-------------------------------|---------------------------------------------------------------|
| `push_back(x)` / `push_front(x)` | Alloue un nœud, l'insère dans la chaîne. Les deux en O(1).   |
| `pop_back()` / `pop_front()`  | Détache + supprime le nœud terminal. O(1).                    |
| `insert(it, x)`               | Insère avant `it`. O(1) **étant donné** `it`.                 |
| `erase(it)`                   | Détache + supprime. O(1). Renvoie l'iterator vers le suivant. |
| `clear()`                     | Détruit chaque nœud. O(n) — libère réellement la mémoire (pas de buffer). |
| `front()` / `back()`          | Reference vers la première / dernière valeur. O(1). UB si vide. |
| `size()` / `empty()`          | O(1) en C++11+ ; **O(n) dans certaines implémentations C++98** de libstdc++ avant la 4.x. Considérez `empty()` comme l'option sûre. |
| `begin()` / `end()`           | Iterators bidirectionnels (pas de `+`, `-`, pas de `<`).      |
| `rbegin()` / `rend()`         | Reverse iterators.                                            |

**Pas d'`operator[]`. Pas d'`at()`.** L'accès aléatoire est O(n) par définition — le standard refuse de l'exposer.

### Fonctions membres spécifiques à list

Elles existent sur `std::list` car elles exploitent la structure chaînée pour être plus efficaces que l'algorithme générique :

| Méthode             | Ce qu'elle fait                                                   | Coût |
|---------------------|-------------------------------------------------------------------|------|
| `splice(pos, other)` | Déplace l'intégralité de `other` dans `*this` avant `pos`. **Aucune copie, aucune allocation.** | O(1) |
| `splice(pos, other, it)` | Déplace un seul élément. | O(1) |
| `splice(pos, other, first, last)` | Déplace une plage. | O(1) en C++98 (taille non maintenue) |
| `merge(other)`      | Fusionne deux listes **triées** en une seule liste triée. | O(n+m) |
| `sort()`            | Trie sur place. List ne peut pas utiliser `std::sort` (nécessite des iterators à accès aléatoire). | O(n log n) |
| `reverse()`         | Inverse la chaîne en échangeant les pointeurs `prev`/`next`. | O(n) |
| `unique()`          | Supprime les doublons consécutifs (triez d'abord pour une unicité globale). | O(n) |
| `remove(val)`       | Supprime chaque nœud égal à `val`. | O(n) |
| `remove_if(pred)`   | Supprime chaque nœud satisfaisant `pred`. | O(n) |

`std::sort(l.begin(), l.end())` **ne compile pas** pour une liste — les iterators de list sont bidirectionnels, alors que `std::sort` requiert un accès aléatoire. Utilisez la fonction membre : `l.sort();`.

---

## 4. Fiche récapitulative des complexités

| Opération                     | Coût   | Remarque                                      |
|-------------------------------|--------|-----------------------------------------------|
| `push_back` / `push_front`    | O(1)   | Un `malloc`. Aucune réallocation, jamais.      |
| `pop_back` / `pop_front`      | O(1)   |                                                |
| `insert(it, x)`               | O(1)   | Mais trouver `it` a pu coûter O(n).           |
| `erase(it)`                   | O(1)   |                                                |
| `find(begin, end, x)`         | O(n)   | Pas d'arbre, pas de table de hachage — scan complet. |
| `l.sort()`                    | O(n log n) | Fonction membre ; utilise un tri fusion (merge sort). |
| `l.splice(pos, other)`        | O(1)   | La fonctionnalité maîtresse. Déplace toute la liste, aucune copie. |
| `size()`                      | O(1) (C++11+) / O(n) (certains C++98) | Utilisez `empty()` quand vous n'avez besoin que du booléen. |
| `clear()`                     | O(n)   | Libère chaque nœud.                           |
| Accès aléatoire `l[i]`        | **N/A** | N'existe pas. Avancer manuellement un iterator avec `++` est en O(n). |

---

## 5. Invalidation des iterators — le super-pouvoir de list

C'est ici que `list` excelle :

| Opération                | Iterators invalidés ?                                      |
|--------------------------|------------------------------------------------------------|
| `push_back` / `push_front` | **Aucun.** Les iterators existants restent valides.      |
| `insert(it, x)`          | **Aucun.** `it` pointe toujours là où il pointait.        |
| `erase(it)`              | Uniquement `it` lui-même.                                  |
| `splice(...)`            | **Aucun.** Les nœuds déplacés conservent leur identité (et leurs iterators !), même transférés dans une autre liste. |
| `clear()`                | Tous les iterators (chaque nœud est libéré).               |
| `swap(other)`            | Le sens de `end()` est échangé, mais les iterators vers les éléments restent valides. |

C'est l'argument décisif pour choisir `list` plutôt que `vector` : si vous avez besoin d'iterators à longue durée de vie malgré de nombreuses insertions, list est le seul conteneur de séquence qui vous le garantit.

---

## 6. Conseils d'efficacité

### 6.1 Utiliser `splice` pour déplacer des nœuds entre listes gratuitement

```cpp
std::list<int> a; a.push_back(1); a.push_back(2);
std::list<int> b; b.push_back(99);

b.splice(b.begin(), a);      // a est maintenant vide, b vaut [1, 2, 99]
                              // ZERO allocation, ZERO copie — simple réajustement de pointeurs
```

Ceci est impossible avec `vector` (cela requerrait une copie en O(n)). C'est la meilleure raison d'être de `std::list`.

### 6.2 Utiliser les fonctions membres `sort` et `merge`, pas les versions de `std::`

```cpp
l.sort();                    // membre — fonctionne
l.merge(other);              // membre — suppose les deux triées ; fusionne en O(n+m)

std::sort(l.begin(), l.end()); // ERREUR — les iterators bidirectionnels rejettent les opérations d'accès aléatoire
```

Pourquoi list a son propre sort : l'algorithme `std::sort` a besoin de `it + n` (accès aléatoire). List ne peut pas le fournir sans parcourir les nœuds. Le sort membre utilise un tri fusion qui ne nécessite que les opérateurs `++` / `--`.

### 6.3 Ne pas calculer `size()` dans une boucle

En C++98 strict, certaines implémentations de `list` (libstdc++ avant 4.7 avec l'ancien ABI) rendent `size()` en O(n) — elles parcourent toute la liste pour compter. Ainsi :

```cpp
// Potentiellement en O(n²) sur une ancienne libstdc++
for (std::size_t i = 0; i < l.size(); ++i) { … }

// Toujours en O(n)
for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) { … }
```

Les libc++ et libstdc++ modernes stockent la taille dans l'en-tête de la liste (O(1)). Néanmoins, préférez toujours l'itération aux boucles indexées sur une liste.

### 6.4 Éviter `list` pour les collections éphémères — vector l'emporte

`malloc` est coûteux — facilement 100× plus lent que l'incrémentation d'un pointeur. Pour un millier de `push_back`, `vector` (avec un seul `reserve`) effectue une allocation ; `list` en effectue mille. List est destinée aux collections à **longue durée de vie** avec de fréquentes insertions/suppressions au milieu, pas à l'accumulation rapide de données.

### 6.5 `remove`/`remove_if` est l'idiome erase–remove, directement intégré

```cpp
l.remove(0);                 // supprime chaque 0 — O(n)
l.remove_if(IsNegative());    // supprime chaque élément négatif — O(n)
```

Pour vector, vous écrivez `v.erase(std::remove(v.begin(), v.end(), 0), v.end())`. List intègre ce comportement directement dans un appel de fonction membre.

---

## 7. Quand NE PAS utiliser list

| Si vous avez besoin de…                            | Choisissez…                                    |
|----------------------------------------------------|------------------------------------------------|
| Accès aléatoire par indice                        | `vector` ou `deque`                            |
| Parcourir des millions d'éléments rapidement       | `vector` (la localité de cache domine)         |
| Un simple LIFO / FIFO                              | `stack` / `queue`                              |
| Clés uniques triées                                | `set`                                          |
| Recherche par association clé → valeur             | `map`                                          |
**Utilisez `vector` par défaut.** Ne choisissez `list` que lorsque vous avez mesuré que l'insertion/suppression au milieu est votre goulot d'étranglement, ou lorsque vous avez réellement besoin d'iterators à longue durée de vie malgré les mutations.

---

## 8. C++98 caveats

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `emplace` / `emplace_back` | C++11   | `insert` / `push_back` — paie une copie.    |
| `std::forward_list`    | C++11       | Utilisez `std::list` (nodes légèrement plus lourds). |
| Move semantics         | C++11       | Utilisez `splice` (qui déplace les nodes gratuitement). |
| Initializer-list ctor  | C++11       | Range ctor depuis un tableau C temporaire.  |

---

## 9. Gotchas

- **`std::sort(l.begin(), l.end())` ne compile pas.** Utilisez `l.sort()`.
- **`size()` peut être en O(n)** sur certaines implémentations C++98. Utilisez `empty()` lorsque vous devez seulement savoir "y a-t-il quelque chose ?".
- **Pas de `operator[]`, pas de `at`.** Si vous vous surprenez à vouloir un accès aléatoire sur une list, passez à `vector` ou `deque`.
- **Chaque node est une allocation heap distincte** — une list de 1M d'ints représente ~24 Mo d'overhead en plus des 4 Mo de données. Les cache-misses dominent le coût d'itération.
- **`splice` depuis une autre list n'invalide pas les iterators déplacés** — ils continuent de fonctionner à travers la nouvelle list. C'est la garantie d'iterator la plus forte de toute la STL.
- **`erase(it)` retourne le prochain iterator**, tout comme pour vector. Le pattern pour "effacer dans une boucle" est identique : `it = l.erase(it);`.

---

## 10. Worked example — splice for free

```cpp
#include <list>
#include <iostream>

int main(void)
{
    std::list<int> hot;       // recently used
    std::list<int> cold;      // archived

    for (int i = 0; i < 5; ++i)
        hot.push_back(i);     // hot = [0, 1, 2, 3, 4]

    // "Archive" the front element — move one node from hot to cold.
    cold.splice(cold.end(), hot, hot.begin());
    // hot = [1, 2, 3, 4], cold = [0]
    // ZERO allocations: the node was unlinked from hot and linked into cold.

    // Print both
    for (std::list<int>::const_iterator it = hot.begin(); it != hot.end(); ++it)
        std::cout << *it << " ";
    std::cout << "| ";
    for (std::list<int>::const_iterator it = cold.begin(); it != cold.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";        // 1 2 3 4 | 0
    return 0;
}
```

Compilez avec les flags de 42 :

```
c++ -Wall -Wextra -Werror -Wswitch -std=c++98 main.cpp -o demo
valgrind --leak-check=full ./demo
```

Chaque `push_back` est un `malloc` ; le destructor de `std::list` parcourt la chaîne et `free` chaque node. Valgrind rapporte zéro fuite.

---

> **Résumé en une ligne.** `std::list` est la `t_list` de la libft avec des pointers `prev` et une valeur templated. Choisissez-la pour des **iterators à longue durée de vie malgré les insertions/suppressions au milieu**, ou pour **déplacer (splice) des nodes entre des lists en O(1)**. Sinon, vector l'emporte.