# `std::vector` — Le conteneur de séquence par défaut

> **TL;DR.** Un `vector<T>` est un **tableau contigu redimensionnable dynamiquement** de `T`. Il stocke trois pointers — `begin`, `end`, `end_of_storage` — et grandit en se réallouant dans un bloc plus grand (typiquement ×2) lorsqu'il manque d'espace. C'est le bon choix **par défaut** ; ne choisissez `list`, `deque`, `set` ou `map` que si vous avez une raison mesurable de le faire.

Articles connexes : [`STL.md`](../advanced/STL.md) · [`TEMPLATES.md`](../advanced/TEMPLATES.md) · [`MEMORY.md`](../fundamentals/MEMORY.md) · [`NEW.md`](../../lexique/NEW.md) · [`INDEX.md`](INDEX.md)

Header : `#include <vector>`

---

## Table des matières

1. [Modèle mental — ce qu'est réellement un vector](#1-modèle-mental--ce-quest-réellement-un-vector)
2. [Construction](#2-construction)
3. [Capacité — `size` vs `capacity`](#3-capacité--size-vs-capacity)
4. [Accès aux éléments](#4-accès-aux-éléments)
5. [Modificateurs](#5-modificateurs)
6. [Itérateurs](#6-itérateurs)
7. [Aide-mémoire de complexité](#7-aide-mémoire-de-complexité)
8. [Invalidation d'itérateurs — le tueur silencieux](#8-invalidation-ditérateurs--le-tueur-silencieux)
9. [Efficacité : dix astuces](#9-efficacité--dix-astuces)
10. [Algorithmes qui s'associent bien avec vector](#10-algorithmes-qui-sassocient-bien-avec-vector)
11. [`vector<bool>` — l'exception maudite](#11-vectorbool--lexception-maudite)
12. [Quand NE PAS utiliser vector](#12-quand-ne-pas-utiliser-vector)
13. [Pièges spécifiques à C++98](#13-pièges-spécifiques-à-c98)
14. [Pièges courants](#14-pièges-courants)

---

## 1. Modèle mental — ce qu'est réellement un vector

Un `vector<T>` est essentiellement une toute petite struct contenant trois pointers (ou un pointer + deux tailles — selon les implémentations) :

```
   struct vector_internals {
       T *begin_;         // first element
       T *end_;            // one past the last element
       T *end_of_storage_; // one past the allocated buffer
   };
```

Visuellement :

```
   vector<int> v with size = 4, capacity = 8

      begin_              end_                end_of_storage_
       │                   │                   │
       v                   v                   v
     ┌───┬───┬───┬───┬───┬───┬───┬───┐
     │ 3 │ 1 │ 4 │ 1 │ . │ . │ . │ . │   ← heap-allocated, contiguous
     └───┴───┴───┴───┴───┴───┴───┴───┘
       0   1   2   3
       └── size = 4 ──┘
       └────── capacity = 8 ──────┘
```

L'intérêt principal : **les éléments résident dans un seul bloc contigu sur la heap**, exactement comme le tableau C `new T[N]` que vous utilisiez dans CPP01 — mais avec un agrandissement automatique. Grâce à cette contiguïté, vous obtenez :

- Un accès aléatoire en O(1) (`v[i]` = `*(begin_ + i)`).
- Une efficacité maximale avec le cache CPU (le prefetcher adore la mémoire séquentielle).
- Un raw pointer vers les données via `&v[0]` — interopérable avec n'importe quelle API C attendant un `T *`.

Lorsque vous dépassez la `capacity`, le vector :
1. Alloue un **nouveau buffer plus grand** (typiquement 2× l'ancienne capacité sur libc++ / libstdc++).
2. **Copie** (ou copy-construct) chaque élément dans le nouveau buffer.
3. Détruit les originaux et libère l'ancien buffer.
4. Met à jour les trois pointers.

Cette réallocation explique pourquoi `push_back` est en O(1) *amorti*, et non strictement O(1). 99 % des pushs ne sont que des décalages de pointer ; 1 % correspond à une copie complète. La stratégie de doublement garantit que le coût moyen par push reste constant.

### En venant du C — c'est un tableau géré par `realloc`, **pas** une liste chaînée

L'intuition voulant que « vector = un mélange de tableau C et de liste chaînée » est tentante car tous deux grandissent à la demande. C'est trompeur. Un `std::vector<T>` est essentiellement un tableau C redimensionné via `realloc` et enveloppé dans une class — il n'y a pas de nœuds, pas de pointers `next`, rien qui ressemble à une liste.

Si vous en écriviez un en C selon les conventions de la libft, il ressemblerait presque exactement à ceci :

```c
typedef struct s_vec {
    int     *data;       // begin_     — heap buffer
    size_t  size;        // logical count
    size_t  capacity;    // physical count
}   t_vec;

void    vec_push_back(t_vec *v, int x)
{
    if (v->size == v->capacity)
    {
        v->capacity = v->capacity ? v->capacity * 2 : 1;
        v->data = realloc(v->data, v->capacity * sizeof(int));
        // ↑ realloc may MOVE the buffer to a new address.
        //   Any pointer you held into the old buffer now dangles.
    }
    v->data[v->size++] = x;
}
```

Voilà `std::vector<int>` en douze lignes de C. Chaque `push_back` est un `realloc + assign`. Chaque `&v[0]` est `v->data`. Chaque `v[i]` est `v->data[i]`.

Le *mécanisme* d'agrandissement est à l'opposé d'une liste chaînée : une liste grandit en allouant **un nouveau nœud** et en pointant vers lui ; un vector grandit en allouant **un tout nouveau buffer plus grand** et en copiant l'intégralité du contenu à l'intérieur.

| Propriété                                 | Tableau C          | libft `t_list *`           | `std::vector`                          |
| ----------------------------------------- | ------------------ | -------------------------- | -------------------------------------- |
| Organisation mémoire                      | Contiguë           | Nœuds dispersés            | **Contiguë** ← comme un tableau        |
| Accès aléatoire `v[i]`                    | O(1)               | O(n)                       | **O(1)** ← comme un tableau            |
| Efficacité du cache                       | Excellente         | Mauvaise (chaque nœud = miss)| **Excellente** ← comme un tableau      |
| Peut grandir à la demande ?               | Non                | Oui                        | **Oui** ← comme une liste              |
| Insérer à la **fin**                      | N/A                | O(1)                       | O(1) amorti                            |
| Insérer au **milieu**                     | N/A                | O(1) (rechaîner)           | **O(n) — doit décaler** ← comme tableau|
| Pointer vers élément toujours valide ?    | Oui (jusqu'à `free`)| Oui (jusqu'à free du nœud) | **Non — invalidé à l'agrandissement**  |

Le vector se comporte comme un tableau sur chaque colonne, sauf pour la capacité à « grandir ». Cette unique propriété rappelant les listes chaînées est elle-même implémentée à la manière d'un tableau (via `realloc`), et non à la manière d'une liste (via un chaînage de nœuds).

Si vous cherchez réellement l'équivalent de la liste chaînée, c'est [`std::list`](LIST.md) — la même `t_list *` que vous avez écrite dans la libft, simplement transformée en template avec des pointers `prev` en plus. Le véritable compromis idéal est **`std::deque`** : une liste de tableaux de taille fixe. Accès aléatoire en O(1), push/pop aux deux extrémités en O(1), mais pas contigu globalement :

```
   std::deque<int> internals (conceptually)
   ┌──────────────┐
   │ block_ptrs[] │ → [block A] [block B] [block C] [block D]
   └──────────────┘     (4 ints)  (4 ints)  (4 ints)  (4 ints)
```

> **Modèle mental :** `std::vector<T>` est un tableau C sur la heap qui connaît sa `size` et sa `capacity` et peut s'auto-`realloc`. L'agencement contigu le rend rapide. L'agrandissement automatique est un confort apporté par-dessus. Tous les pièges de la section 8 découlent de ce simple fait.

---

## 2. Construction

C++98 fournit ces constructeurs (et l'opérateur d'assignation par copie) :

```cpp
#include <vector>

std::vector<int> a;                            // empty,   size=0, capacity=0
std::vector<int> b(5);                         // 5 elems, value-initialized → 0
std::vector<int> c(5, 42);                     // 5 elems all == 42
std::vector<int> d(c);                         // copy of c
std::vector<int> e(c.begin(), c.end());        // range copy
std::vector<int> f(arr, arr + 10);             // from a C array

a = c;                                          // copy-assign — clears a, copies c's elements
a.assign(5, 7);                                 // replace contents with 5 sevens
a.assign(c.begin(), c.end());                   // replace contents with c's range
```

> **C++11+ uniquement (interdit dans le Mod08) :** `std::vector<int> v = {1, 2, 3};` — ctor par liste d'initialisation entre accolades.
> En C++98, vous construisez d'abord un petit tableau et utilisez le ctor par plage : `int tmp[] = {1,2,3}; std::vector<int> v(tmp, tmp + 3);`.

---

## 3. Capacité — `size` vs `capacity`

C'est **le** concept qui échappe à la plupart des débutants.

| Fonction       | Signification                                                           |
|----------------|-------------------------------------------------------------------------|
| `size()`       | Combien d'éléments vous avez logiquement.                               |
| `capacity()`   | Combien d'éléments peuvent loger avant que le buffer ne doive grandir.  |
| `empty()`      | `size() == 0`. Utilisez ceci — plus clair et jamais erroné.             |
| `max_size()`   | Le maximum théorique — généralement `2^31` ou `2^63`. Inutile en pratique. |
| `reserve(n)`   | Si `capacity < n`, réalloue à au moins `n`. **Ne modifie pas size.**   |
| `resize(n)`    | Force `size == n`. Default-construct les nouveaux éléments, détruit l'excédent. |
| `resize(n, x)` | Pareil, mais initialise les nouveaux éléments avec `x`.                 |

```cpp
std::vector<int> v;
std::cout << v.size() << " " << v.capacity() << "\n";  // 0 0
v.reserve(100);
std::cout << v.size() << " " << v.capacity() << "\n";  // 0 100 (size unchanged!)
v.push_back(42);
std::cout << v.size() << " " << v.capacity() << "\n";  // 1 100 (no realloc — capacity was enough)
v.resize(5, 0);
std::cout << v.size() << " " << v.capacity() << "\n";  // 5 100 (added four zeros)
```
**Règle :** `reserve` sert à la performance — il évite les réallocations. `resize` sert à la *sémantique* — il crée / détruit réellement des éléments.

---

## 4. Element access

| Forme        | Bounds check ?        | Retourne                             |
|--------------|-----------------------|--------------------------------------|
| `v[i]`       | **Non.** UB si `i ≥ size`. | `T&` (ou `const T&` sur un vector const) |
| `v.at(i)`    | Oui — lève `std::out_of_range`. | Identique |
| `v.front()`  | Non — UB si vide.     | Premier élément.                      |
| `v.back()`   | Non — UB si vide.     | Dernier élément.                      |
| `&v[0]`      | Non — UB si vide.     | `T *` vers le buffer sous-jacent. La porte de sortie vers les C-array. |

```cpp
std::vector<int> v(3, 0);
v[0] = 10;
v.at(1) = 20;       // sûr mais plus lent (un branchement par accès)
int x = v.front();  // 10
int y = v.back();   // 0
int *raw = &v[0];   // passage à une API C : read(fd, raw, sizeof(int) * v.size())
```

> **C++11+ :** `v.data()` retourne le raw pointer même lorsque le vector est vide (retourne quelque chose de convertible en null). En C++98, `&v[0]` sur un vector vide est un **UB** — vérifiez toujours `!v.empty()` au préalable.

---

## 5. Modifiers

### 5.1 Add / remove at the back — chemin rapide

```cpp
v.push_back(x);          // O(1) amorti — le cheval de trait
v.pop_back();            // O(1) — détruit le dernier élément, décrémente size
v.clear();               // O(n) — détruit tous les éléments, size = 0, capacity INCHANGÉE
```

**`clear()` ne libère pas la mémoire.** Il détruit seulement les éléments. Si vous devez libérer le buffer, consultez le [swap trick](#9.%20Efficiency%3A%20ten%20tricks).

### 5.2 Insert / erase in the middle — chemin lent

```cpp
std::vector<int>::iterator it = v.begin() + 2;
v.insert(it, 99);                       // insère 99 à l'index 2, décale le reste à droite — O(n)
v.insert(it, 3, 99);                    // insère trois 99 — O(n)
v.insert(it, src.begin(), src.end());   // insère un range — O(n + m)

it = v.begin() + 2;
it = v.erase(it);                       // efface un élément, décale le reste à gauche — O(n)
                                         // retourne l'iterator vers l'élément APRÈS celui effacé
v.erase(v.begin() + 1, v.begin() + 4);  // efface un range — O(n)
```

Chaque insertion / suppression au milieu **décale tous les éléments situés après la modification**. Cela représente des déplacements de mémoire en O(n). Si vous faites cela dans une boucle, vous obtenez du O(n²).

### 5.3 Swap

```cpp
std::vector<int> a, b;
a.swap(b);              // O(1) — échange uniquement les trois pointeurs internes
std::swap(a, b);        // identique, spécialisé par le standard
```

Swap s'exécute en temps constant *et* ne throw jamais. C'est le fondement de plusieurs idiomes d'efficacité.

### 5.4 Assign

```cpp
v.assign(5, 0);                // remplace le contenu par cinq 0
v.assign(src.begin(), src.end());
```

Équivalent à `clear()` suivi de `insert(end(), …)`, mais l'implémentation peut réutiliser le stockage existant.

---

## 6. Iterators

Chaque vector expose quatre types d'iterator :

```cpp
std::vector<int>::iterator               it;
std::vector<int>::const_iterator         cit;
std::vector<int>::reverse_iterator       rit;
std::vector<int>::const_reverse_iterator crit;
```

Obtenus via :

| Méthode           | Retourne                                           |
|-------------------|----------------------------------------------------|
| `begin() / end()` | iterator (ou const_iterator sur un vector const)   |
| `rbegin() / rend()` | reverse_iterator (parcourt `back()` → `front()`) |

Les iterators de vector sont à **accès aléatoire** (random-access) : vous pouvez faire `it + 5`, `it2 - it1`, `it[3]`, `it < it2`. C'est pourquoi `std::sort` (qui nécessite un accès aléatoire) fonctionne sur les vectors mais pas sur `std::list`.

```cpp
for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
}
```

**C++98 ne dispose pas de range-`for`** (`for (int x : v)`). Vous devez écrire la boucle explicite ou utiliser `std::for_each`.

---

## 7. Complexity cheat sheet

| Opération                       | Coût                | Note                                       |
|---------------------------------|---------------------|--------------------------------------------|
| `v[i]`, `v.at(i)`               | O(1)                | `at` est légèrement plus lent (branchement). |
| `v.front()`, `v.back()`         | O(1)                |                                            |
| `v.push_back(x)`                | **Amorti** O(1)     | O(n) occasionnel lors d'une réallocation.  |
| `v.pop_back()`                  | O(1)                |                                            |
| `v.insert(end(), x)`            | O(1) amorti         | Identique à `push_back`.                   |
| `v.insert(middle, x)`           | O(n)                | Décalages.                                 |
| `v.erase(middle)`               | O(n)                | Décalages.                                 |
| `v.clear()`                     | O(n)                | Les destructors s'exécutent ; capacity inchangée. |
| `v.size()`, `v.capacity()`, `v.empty()` | O(1)        |                                            |
| `v.reserve(n)`                  | O(n) quand il grandit, O(1) quand ce n'est pas le cas | Coût ponctuel. |
| `v.resize(n)`                   | O(n)                |                                            |
| `v.swap(w)`                     | O(1)                | Échange simplement trois pointeurs.        |
| Trouver une valeur (`std::find`)| O(n)                | Balayage linéaire.                         |
| `std::sort(v.begin(), v.end())` | O(n log n)          | Introsort sous le capot.                   |
| `std::lower_bound` sur v trié   | O(log n)            | Vector surpasse `std::set` ici pour les petits N. |

---

## 8. Invalidation d'iterator — le tueur silencieux

Lorsque vous mutez un vector, **les iterators, pointeurs et references pointant vers lui peuvent devenir dangling**. Les déréférencer est un comportement indéfini (UB) — valgrind l'intercepte parfois, ASan l'intercepte de manière fiable, votre soutenance pourrait ne pas le faire.

| Opération                                           | Qu'est-ce qui meurt ?                                         |
|-----------------------------------------------------|---------------------------------------------------------------|
| `push_back` / `insert` **sans** réallocation        | Les iterators **au niveau ou après** le point d'insertion.    |
| `push_back` / `insert` **avec** réallocation         | **Tous** les iterators, pointeurs, references.                |
| `pop_back`                                          | `end()` et iterator/ref vers le dernier élément supprimé.     |
| `erase(it)`                                         | `it` et tout ce qui se trouve après.                          |
| `clear()`                                           | Tous les iterators, refs (mais `end()` reste valide en tant que nouvelle fin). |
| `reserve(n)` avec `n > capacity`                    | Tous les iterators, pointeurs, references.                    |
| `resize(n)` qui dépasse la capacity                 | Tous les iterators, pointeurs, references.                    |
| `swap(other)`                                       | Aucun des vôtres — mais `end()` change de signification.      |

**Le bug classique — un pointer vers un élément conservé au-delà d'un `push_back` :**

```cpp
std::vector<int> v;
v.push_back(10);
int *p = &v[0];          // pointe dans le buffer de v
v.push_back(20);         // si capacity valait 1, le buffer se déplace — p est maintenant dangling
*p = 99;                 // UB
```

La solution : utilisez un index, pas un pointer. Ou appliquez `reserve` suffisamment au préalable afin qu'aucune réallocation ne se produise pendant la durée de vie de `p`.

**L'autre classique — erase dans une boucle :**

```cpp
// INCORRECT — erase retourne le prochain iterator valide ; vous devez l'utiliser.
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (*it == 0)
        v.erase(it);     // it est maintenant invalide ; ++it est un UB
}

// CORRECT — laissez erase vous faire avancer.
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ) {
    if (*it == 0)
        it = v.erase(it);
    else
        ++it;
}

// OPTIMAL — idiome erase–remove (voir §10).
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

---

## 9. Efficiency: ten tricks

### 9.1 `reserve()` avant un ajout massif

```cpp
std::vector<int> v;
v.reserve(1000);              // une seule allocation
for (int i = 0; i < 1000; ++i)
    v.push_back(i);
```

Sans `reserve`, le vector se réalloue ~10 fois pour 1000 éléments, copiant chaque élément à chaque fois. Avec `reserve`, vous n'avez qu'une seule allocation. Sur des types non triviaux, cela peut apporter une accélération de 5 à 10×.

### 9.2 Passer par **`const &`** (ou `&` si vous mutez)

```cpp
void print(const std::vector<int> &v);    // BIEN — pas de copie
void print(std::vector<int> v);            // MAUVAIS — copie le buffer entier
```

`vector` est un type valeur (value-type). Le passage par valeur copie chaque élément.

### 9.3 Le **swap trick** — libérer la capacité excédentaire (substitut C++98 à `shrink_to_fit`)

```cpp
std::vector<int> v;
v.reserve(10000);
v.push_back(42);
// v.size() == 1, v.capacity() == 10000 — gaspillage

std::vector<int>(v).swap(v);
// construit un *nouveau* vector avec capacity == size, l'échange avec v,
// l'ancien buffer (surdimensionné) est détruit à la fin de la full-expression
```
Pour descendre jusqu'à une capacity nulle :

```cpp
std::vector<int>().swap(v);   // v is now empty AND has freed its buffer
```

### 9.4 Préférer `push_back` et `pop_back` aux insertions / suppressions au milieu

Si vous pouvez reformuler votre algorithme pour ne manipuler que l'arrière du vector, vous transformez du O(n) par opération en O(1) par opération. Parfois, un tri suivi d'un parcours linéaire s'avère une meilleure approche que « recherche + erase au milieu ».

### 9.5 L'**idiome erase–remove** — supprimer toutes les correspondances en O(n)

```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());
```

Ce qui se passe :
- `std::remove` n'efface rien concrètement — il déplace les éléments « conservés » vers l'avant et renvoie un iterator vers la nouvelle fin logique.
- `v.erase(new_end, v.end())` tronque ensuite la fin en O(1) par élément restant.

Total : O(n) au lieu de O(n²) pour un « erase dans une boucle ».

Même principe avec `std::remove_if` et un foncteur :

```cpp
struct IsNegative {
    bool operator()(int x) const { return x < 0; }
};
v.erase(std::remove_if(v.begin(), v.end(), IsNegative()), v.end());
```

### 9.6 Trier une fois, puis `lower_bound` — bat `std::set` pour de petites valeurs de N

Si vous construisez une collection puis l'interrogez de nombreuses fois sans insertions supplémentaires :

```cpp
std::sort(v.begin(), v.end());
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), key);
bool found = (it != v.end() && *it == key);
```

Cela offre une recherche en O(log n) avec la localité de cache du vector. `std::set` est aussi en O(log n), mais chaque nœud implique une allocation heap séparée — vector gagne pour `n < ~1000` dans les benchmarks réels.

### 9.7 Construire dans une variable locale, puis faire un `swap` vers la destination

```cpp
void rebuild(std::vector<int> &target) {
    std::vector<int> tmp;
    tmp.reserve(target.size());
    // … fill tmp …
    target.swap(tmp);                     // O(1); old target's elements die with tmp
}
```

Garantie forte d'exception gratuite — si la construction de `tmp` lance une exception, `target` reste intact.

### 9.8 Utiliser des indices, pas des iterators, lorsqu'une réallocation peut survenir

```cpp
for (std::size_t i = 0; i < v.size(); ++i) {
    if (some_condition(v[i]))
        v.push_back(derived(v[i]));       // safe: i still indexes correctly after realloc
}
```

Une version de cette boucle basée sur des iterators deviendrait pendante (dangling) dès la première réallocation.

### 9.9 `clear()` ≠ libération. Utiliser `swap` pour réellement restituer la mémoire.

```cpp
v.clear();                                 // size = 0, capacity unchanged — buffer still held
std::vector<int>().swap(v);                // size = 0, capacity = 0 — buffer freed
```

### 9.10 Construire directement sur place avec le ctor (count, value) ou par plage

```cpp
std::vector<int> v(1000, 0);               // one allocation, 1000 zeroes
// vs
std::vector<int> v;
for (int i = 0; i < 1000; ++i) v.push_back(0);   // multiple reallocs, slower
```

Les constructeurs de la bibliothèque standard connaissent la taille finale à l'avance — ils peuvent pré-allouer exactement une fois.

---

## 10. Algorithmes qui se marient bien avec vector

Tous ces algorithmes résident dans `<algorithm>`. Ils opèrent sur des iterators et sont donc indépendants du conteneur, mais les iterators à accès aléatoire contigus du vector sont les plus efficaces possibles — chaque algorithme s'exécute à sa vitesse maximale.

| Algorithme                     | Utilisation                                                  |
|--------------------------------|--------------------------------------------------------------|
| `std::sort`                    | Introsort en O(n log n). Random-access iterators requis.     |
| `std::stable_sort`             | Comme `sort` mais préserve l'ordre des éléments équivalents. |
| `std::find` / `std::find_if`   | Recherche linéaire. Renvoie `end()` si non trouvé.          |
| `std::count` / `std::count_if` | Compte le nombre de correspondances.                         |
| `std::remove` / `std::remove_if` | À associer avec `erase` — voir §9.5.                        |
| `std::unique`                  | Supprime les doublons **adjacents**. Triez d'abord pour une unicité globale. |
| `std::reverse`                 | Inversion sur place.                                         |
| `std::lower_bound` / `upper_bound` | Recherche binaire sur un vector trié.                   |
| `std::binary_search`           | Renvoie simplement un bool.                                  |
| `std::accumulate` (dans `<numeric>`) | Somme / réduction (fold).                              |
| `std::copy` / `std::copy_backward` | Copie des plages entre iterators.                         |
| `std::fill` / `std::fill_n`    | Écrit une valeur sur toute une plage.                       |
| `std::min_element` / `std::max_element` | Iterators vers le plus petit / plus grand.          |
| `std::nth_element`             | Tri partiel — garantit seulement que le n-ième élément est à sa place. O(n). |

Exemple idiomatique — tri avec élimination des doublons :

```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
```

---

## 11. `vector<bool>` — l'exception maudite

`std::vector<bool>` n'est **pas** un véritable vector de `bool`. Il s'agit d'une spécialisation compactée au niveau du bit — chaque élément occupe 1 bit, et non `sizeof(bool)` octets. Par conséquent :

- `&v[0]` n'est **pas** un `bool *` — vous ne pouvez pas le passer à une API C.
- `v[i]` renvoie une **référence proxy**, pas un `bool &`. `bool &b = v[i];` ne compilera pas.
- Les iterators ont des types surprenants ; certains codes génériques ne s'instancient pas.

**Solutions de contournement :**

- `std::vector<char>` si vous voulez un tableau booléen normal avec un octet par élément.
- `std::vector<int>` si vous le voulez encore plus grand et avec des valeurs stables.
- `std::deque<bool>` si vous désirez réellement un vrai conteneur de `bool`.

Ceci est largement considéré comme la plus grande erreur de la STL. Évitez `vector<bool>` à moins d'avoir spécifiquement besoin d'économiser de l'espace et d'avoir audité chaque utilisation.

---

## 12. Quand NE PAS utiliser vector

Vector est le choix par défaut. Choisissez autre chose uniquement si vous avez une raison *spécifique* :

| Si vous avez besoin de…                            | Choisissez…                                    |
|----------------------------------------------------|------------------------------------------------|
| Beaucoup d'insertions/suppressions au **milieu**   | `std::list` (splice / insertion en O(1) sur un iterator) |
| Push/pop rapide aux **deux extrémités** + random access | `std::deque`                             |
| Stabilité des iterators/references lors des insertions | `std::list`, `std::deque` (front/back uniquement), ou `std::map` |
| Recherche clé → valeur en O(log n)                 | `std::map`                                     |
| Clés uniques, ordonnées                            | `std::set`                                     |
| LIFO / FIFO avec interface restreinte              | `std::stack`, `std::queue`                     |
| Un tampon de taille fixe, qui ne grandit jamais    | C array ou membre `T[N]`                       |

Pour les exercices du CPP09 (`MutantStack`, `BitcoinExchange`, `RPN`) : vector l'emporte généralement pour RPN (push/pop à l'arrière uniquement), map l'emporte pour BitcoinExchange (recherche de date triée avec `lower_bound`).

---

## 13. Mises en garde C++98

Ce que vous **n'avez pas**, et que le code plus moderne utilise :

| Fonctionnalité         | Version C++ | Alternative C++98                                |
|------------------------|-------------|--------------------------------------------------|
| `auto`                 | C++11       | Écrire le type d'iterator en toutes lettres.     |
| Range-`for`            | C++11       | Boucle avec index ou boucle explicite avec iterator. |
| `emplace_back`         | C++11       | `push_back` — effectue une copie mais fonctionne.|
| `shrink_to_fit()`      | C++11       | L'**astuce du swap** (§9.3).                     |
| Membre `data()`        | C++11       | `&v[0]` (UB si vide — protéger avec `!empty()`). |
| Ctor avec initializer-list `{1,2,3}` | C++11 | Construire un C array d'abord, puis initialiser par plage. |
| Sémantique de déplacement (`vector<X>` de types move-only) | C++11 | Stocker des `X *` et gérer la durée de vie manuellement. |
| `cbegin` / `cend`      | C++11       | `const_iterator` à partir d'une `const vector&`. |
| `std::vector` sur une `initializer_list` de paires | C++11 | Construire avec `std::make_pair` et `push_back`. |

Les flags de 42 `-std=c++98 -Werror` rejetteront catégoriquement toutes les formes de C++11. C'est une bonne chose — cela vous force à comprendre ce que chacune d'elles accomplit réellement sous le capot.

---

## 14. Pièges

- **`v.size() - 1` subit un underflow lorsque `v` est vide.** `size()` renvoie un `size_t` (non signé). `0u - 1u == SIZE_MAX`. Vérifiez toujours `!v.empty()` avant de calculer `size() - 1`.
- **Incompatibilité signé/non signé dans les boucles.** `for (int i = 0; i < v.size(); ++i)` déclenche un avertissement avec `-Wall` car `i` est un `int` et `v.size()` est un `size_t`. Utilisez `std::size_t` ou effectuez un cast délibéré.
- **`v[i]` ne vérifie pas les limites.** En debug, libc++/libstdc++ le font parfois, en release elles ne le font pas. Utilisez `at` pendant le développement, `[]` dans les boucles critiques.
- **Un vector d'objets n'est *pas* un vector de pointers.** `std::vector<MyClass> v; v.push_back(MyClass());` appelle le constructeur de copie. Si `MyClass` est coûteuse à copier, stockez plutôt des `MyClass *` — et n'oubliez pas d'appeler `delete` manuellement sur chaque pointer (pas de smart pointers en C++98 à l'exception de `std::auto_ptr`, qui est inutilisable dans des conteneurs).
- **`v == w` s'applique élément par élément** et utilise l'`operator==` sur `T`. Assurez-vous que votre class en possède un si vous comparez des vectors.
- **Le facteur d'accroissement de la capacity est défini par l'implémentation.** N'écrivez pas de code dépendant du fait qu'il soit exactement de ×2 (libc++ utilise 2, libstdc++ utilise 2, MSVC utilise 1.5).
- **`.front()` / `.back()` sur un vector vide est un comportement indéfini (UB)**, pas une exception. Ils ne vérifient pas les limites. Faites toujours `if (!v.empty())` au préalable.
- **L'auto-insertion est un UB.** `v.insert(v.end(), v.begin(), v.end())` peut corrompre la mémoire car l'insertion peut réallouer, invalidant les iterators sources en pleine copie. Créez d'abord une copie.
---

## Exemple concret — les idiomes standards de 42 en un seul programme

```cpp
#include <vector>
#include <algorithm>
#include <iostream>

int main(void)
{
    std::vector<int> v;
    v.reserve(10);                                  // §9.1

    for (int i = 0; i < 10; ++i)
        v.push_back(i % 4);

    std::sort(v.begin(), v.end());                  // §10

    v.erase(std::unique(v.begin(), v.end()),        // §10 — unique-sort
            v.end());

    v.erase(std::remove(v.begin(), v.end(), 2),     // §9.5 — erase–remove
            v.end());

    for (std::vector<int>::const_iterator it = v.begin();
         it != v.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    std::vector<int>().swap(v);                     // §9.3 — libérer la mémoire
    return 0;
}
```

Compilez avec les flags de 42 :

```
c++ -Wall -Wextra -Werror -Wswitch -std=c++98 main.cpp -o demo
valgrind --leak-check=full ./demo
```

Si vous utilisez reserve, push_back, sort et erase–remove correctement, valgrind ne signale aucun leak et aucun invalid read. C'est l'objectif.

---

> **Résumé en une ligne.** `std::vector` est un `new T[]` qui sait grandir. Utilisez reserve en amont, swap pour réduire, ne conservez jamais un pointer à travers un `push_back`, et utilisez erase–remove au lieu d'effacer dans une boucle. Tout le reste n'est qu'une note de bas de page.