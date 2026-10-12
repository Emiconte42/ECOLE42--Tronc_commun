# `cpp/containers/` — Une page par container STL

Notes détaillées pour chaque container STL de C++98. Utilisez ce document comme point d'entrée ; pour la vue d'ensemble de haut niveau "à quoi sert la STL", consultez [`../advanced/STL.md`](../advanced/STL.md).

> Chaque page suit le même plan : TL;DR → modèle mental (avec diagramme ASCII) → construction → API clé → tableau de complexité → invalidation des iterators → conseils d'efficacité → quand ne pas l'utiliser → pièges et limites de C++98 → pièges courants (gotchas) → exemple concret.

---

## Les 10 containers, classés par famille

### Sequence containers — *stockage indexé par position*

| Container        | Fichier                  | En une ligne                                                                       | Par défaut ? |
| ---------------- | ------------------------ | ---------------------------------------------------------------------------------- | ------------ |
| `std::vector<T>` | [`VECTOR.md`](VECTOR.md) | Tableau contigu dynamique. Le choix par défaut. Croissance style `realloc`, accès aléatoire en O(1). | ✅           |
| `std::list<T>`   | [`LIST.md`](LIST.md)     | Liste doublement chaînée de nœuds sur le heap. Splice en O(1), iterators stables, pas d'accès aléatoire. |              |
| `std::deque<T>`  | [`DEQUE.md`](DEQUE.md)   | Liste de blocs — O(1) aux deux extrémités *et* accès aléatoire. References stables lors des push. |              |

`std::string` (techniquement un sequence container) se trouve dans [`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md).

### Container adapters — *interface restreinte par-dessus un sequence container*

| Container | Fichier | En une ligne | Container sous-jacent par défaut |
|---|---|---|---|
| `std::stack<T>` | [`STACK.md`](STACK.md) | LIFO. `push` / `top` / `pop`. Seul `pop()` est `void` — lire avec `top()` d'abord. | `deque` |
| `std::queue<T>` | [`QUEUE.md`](QUEUE.md) | FIFO. Même forme que stack mais `front()` + `pop()`. Vector sous-jacent interdit. | `deque` |
| `std::priority_queue<T>` | [`PRIORITY_QUEUE.md`](PRIORITY_QUEUE.md) | Max-heap binaire. Push/pop en O(log n), top en O(1). `std::greater<T>` pour un min-heap. | `vector` |

### Associative containers — *accès basé sur une clé, trié (arbre rouge-noir)*

| Container | Fichier | En une ligne | Doublons autorisés ? |
|---|---|---|---|
| `std::set<K>` | [`SET.md`](SET.md) | Clés uniques triées. Insert/erase/find en O(log n). Iterators stables. | Non |
| `std::multiset<K>` | [`MULTISET.md`](MULTISET.md) | Comme `set` mais doublons autorisés. `equal_range` est l'outil du quotidien. | Oui |
| `std::map<K, V>` | [`MAP.md`](MAP.md) | `K → V` trié. `m[k]` insère en cas d'absence (le piège). Le type d'iterator est `pair<const K, V>`. | Non (clés) |
| `std::multimap<K, V>` | [`MULTIMAP.md`](MULTIMAP.md) | `K → V` trié avec clés en doublon. Pas de `operator[]`. | Oui |

---

## Arbre de décision — "quel container dois-je choisir ?"

```
   Besoin d'une recherche par clé ?
   ├── Oui → itération ordonnée, log n, triée par clé ?
   │         ├── Oui, avec valeur aussi ? → map / multimap
   │         └── Oui, clé uniquement ?   → set / multiset
   └── Non  → pattern d'accès ?
             ├── Push/pop à une seule extrémité ?   → stack (LIFO) ou queue (FIFO)
             ├── Toujours extraire le maximum ?     → priority_queue
             ├── Push/pop aux DEUX bouts + accès aléatoire ? → deque
             ├── Nombreux inserts/erases au milieu ?        → list
             └── Autrement (le choix par défaut)            → vector
```

En cas de doute : **`std::vector`**. Choisissez autre chose uniquement si vous avez une raison mesurée.

---

## Problématiques transversales couvertes dans les fiches

Sujets qui reviennent dans plusieurs fichiers — la page de chaque container couvre la version spécifique à ce container, mais voici ce qu'il faut repérer :

- **Règles d'invalidation des iterators** — chaque container en a ; celles de vector sont les plus strictes, celles de list les plus souples. Voir la section "Iterator invalidation" de chaque fichier.
- **Limites de C++98** — ce qui *manque* (pas de `emplace`, pas d'`auto`, pas de `shrink_to_fit`, pas de `unordered_*`) et le substitut C++98 pour chacun.
- **La règle `pop()` renvoie `void`** — s'applique aux trois adapters (stack, queue, priority_queue). Toujours faire `top()` / `front()` *avant* `pop()`.
- **Passage par `const &`** — chaque container est un type valeur ; les copies sont coûteuses. Toujours passer par reference sauf si vous souhaitez explicitement copier.
- **La forme avec hint de `insert`** — pour les containers ordonnés (set/multiset/map/multimap), passer un hint correct transforme une insertion de masse en O(n log n) en O(n).

---

## Où chaque container apparaît dans le cursus 42

| Container | Mod | Pourquoi |
|---|---|---|
| `vector` | CPP08 ex01 (Span), CPP09 ex01 (RPN) | Random-access, usage intensif de push_back. |
| `list`   | rare              | Apparaît lorsque la stabilité des iterators est le sujet explicite. |
| `deque`  | CPP08 ex02 (MutantStack en dessous) | Container sous-jacent par défaut pour `std::stack`. |
| `stack`  | CPP08 ex02 (MutantStack), CPP09 ex01 (RPN) | Discipline LIFO. |
| `queue`  | rare dans le cursus, courant en algorithmique | BFS, planification (scheduling). |
| `priority_queue` | rare dans le cursus       | Sélection basée sur un heap. |
| `set`    | CPP08 ex01 (Span)              | Tri automatique + déduplication. |
| `map`    | CPP09 ex00 (BitcoinExchange)   | Recherche de séries temporelles via `lower_bound`. |
| `multiset`/`multimap` | rare              | Quand les doublons ont une signification sémantique. |

---

## Ordre de lecture si vous partez de zéro

1. **`VECTOR.md`** — pose le modèle mental ; tout le reste est comparé à lui.
2. **`STL.md` (dans `../advanced/`)** — vue d'ensemble de haut niveau des containers + iterators + algorithmes.
3. **`LIST.md`** — le contraste de la liste chaînée ; enseigne la stabilité des iterators.
4. **`MAP.md`** — le container associatif le plus courant ; introduit `pair<const K, V>` et `lower_bound`.
5. **`STACK.md` / `QUEUE.md`** — le design pattern adapter et la discipline du `pop()` renvoyant `void`.
6. **`SET.md`, `DEQUE.md`, `PRIORITY_QUEUE.md`** — à lire au fur et à mesure de vos besoins.
7. **`MULTISET.md`, `MULTIMAP.md`** — à lire une fois que vous maîtrisez leurs équivalents à clés uniques.