# `std::queue` — FIFO Container Adapter

> **TL;DR.** `std::queue<T>` est le pendant FIFO de [`std::stack`](STACK.md) : un **adapter** léger par-dessus un container sous-jacent. Le conteneur sous-jacent par défaut est `deque` (puisque la queue a besoin à la fois de `push_back` et `pop_front` en O(1)). Vous **ne pouvez pas utiliser `vector`** comme conteneur sous-jacent — `vector` n'a pas de `pop_front`. Même piège du `pop()` qui retourne `void` que pour stack.

En lien : [`STACK.md`](STACK.md) · [`PRIORITY_QUEUE.md`](PRIORITY_QUEUE.md) · [`DEQUE.md`](DEQUE.md) · [`LIST.md`](LIST.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <queue>`

---

## 1. Modèle mental

```
   std::queue<int>            (default = deque underneath)

   front ────►  ┌───┬───┬───┬───┬───┐  ◄──── back
                │ 1 │ 2 │ 3 │ 4 │ 5 │
                └───┴───┴───┴───┴───┘
                  ▲                 ▲
                  │                 │
              pop here           push here

      push() ──→ underlying.push_back()
      pop()  ──→ underlying.pop_front()
      front()──→ underlying.front()
      back() ──→ underlying.back()
```

La norme C++98 définit `std::queue` essentiellement comme :

```cpp
template <class T, class Container = std::deque<T> >
class queue {
protected:
    Container c;
public:
    void  push(const T& x) { c.push_back(x); }
    void  pop()             { c.pop_front(); }
    T&    front()           { return c.front(); }
    T&    back()            { return c.back(); }
    // size, empty, comparisons …
};
```

Ce `pop_front` est la raison pour laquelle **vector ne peut pas servir de base à une queue**. Le `erase(begin())` de vector est en O(n), et il n'y a aucun membre `pop_front`. La norme exige que le backing container fournisse un véritable `pop_front` — seuls `deque` et `list` sont qualifiés.

---

## 2. Construction et choix du container sous-jacent

```cpp
#include <queue>
#include <list>

std::queue<int> q;                          // default — deque
std::queue<int, std::list<int> > ql;        // backed by list (mind the space before >)

std::deque<int> seed;
seed.push_back(1); seed.push_back(2);
std::queue<int> q2(seed);                   // q2.front() == 1, q2.back() == 2
```

| Backing  | Avantages                               | Inconvénients                     |
|----------|-----------------------------------------|-----------------------------------|
| `deque`  | O(1) aux deux extrémités ; cache-friendly au sein des blocs | Overhead d'au moins un bloc       |
| `list`   | Vrai O(1) push/pop ; stabilité des iterators inutile pour queue | Un `malloc` par push — lent       |
| `vector` | **Interdit** — pas de `pop_front`       | —                                 |

**Règle par défaut :** gardez `deque`. Ne changez pas sans raison mesurée.

---

## 3. API principale

| Méthode     | Ce qu'elle fait                                 | Coût  |
|-------------|-------------------------------------------------|-------|
| `push(x)`   | Ajoute `x` à l'arrière.                         | Identique au `push_back` sous-jacent. |
| `pop()`     | Retire l'élément avant. **Retourne `void`.**     | Identique au `pop_front` sous-jacent. |
| `front()`   | Reference vers l'élément le plus ancien. UB si vide. | O(1)  |
| `back()`    | Reference vers l'élément le plus récent. UB si vide. | O(1)  |
| `size()`    | Nombre d'éléments.                              | O(1)  |
| `empty()`   | `size() == 0`.                                  | O(1)  |
| `==`, `<`, etc. | Comparaison lexicographique via le sous-jacent. | O(n)  |

**Pas d'iterators, pas d'indexation.** Même interface restreinte que stack — par conception.

---

## 4. Le même piège du `pop()` qui retourne `void`

Raisonnement identique au [`STACK.md` §4](STACK.md#4.%20The%20pop%28%29-returns-void%20gotcha) : C++98 sépare la *lecture* de la *destruction* pour qu'une copie qui lève une exception ne perde pas de données.

```cpp
// WRONG
int x = q.pop();

// RIGHT
int x = q.front();
q.pop();
```

---

## 5. Conseils d'efficacité

### 5.1 Choisissez `deque` par défaut. `list` en vaut rarement la peine.

Le coût amorti de deque est de O(1) par push et O(1) par pop, avec une localité de cache à l'intérieur de chaque bloc. List ajoute un `malloc`/`free` par push/pop — facilement 10× plus lent dans les charges de travail intenses.

### 5.2 Passez par `const &`

Une copie de queue copie chaque élément. Ne faites pas cela par accident.

### 5.3 Si vous devez jeter un coup d'œil au milieu, vous avez dépassé le cadre de l'adapter

Utilisez `std::deque<T>` directement. Le but même de l'adapter est d'*empêcher* l'accès aux éléments intermédiaires ; si vous en avez besoin, l'abstraction ne convient pas.

### 5.4 Besoin d'une priorité plutôt que de l'ordre d'insertion ? Voir `PRIORITY_QUEUE.md`

`std::queue` est strictement FIFO. Si le "prochain élément à sortir" dépend d'une priorité basée sur la valeur plutôt que sur l'ordre d'arrivée, utilisez plutôt [`std::priority_queue`](PRIORITY_QUEUE.md).

---

## 6. Quand NE PAS utiliser queue

- Vous devez inspecter ou modifier des éléments autres que `front` / `back` → utilisez `deque`.
- Vous devez réordonner les éléments → utilisez `vector` et triez, ou `priority_queue`.
- Vous avez plusieurs producers/consumers à travers des threads → `std::queue` n'est **pas thread-safe**. C++98 n'a pas de queue concurrente intégrée ; vous devriez encapsuler avec un mutex (et `<thread>` est en C++11+, interdit dans le cursus 42 jusqu'à plus tard).

Utilisez queue quand :
- L'algorithme consiste à "traiter dans l'ordre d'arrivée" — BFS, planificateur de tâches, boucle d'événements, pool de threads.
- Vous voulez que la discipline FIFO soit appliquée par le type.

---

## 7. Spécificités C++98

| Fonctionnalité | Version C++ | Alternative C++98                |
|----------------|-------------|----------------------------------|
| `emplace`      | C++11       | `push` — subit une copie.        |
| Move semantics | C++11       | Passage par `const &`.           |
| Surcharges de `std::queue` depuis une initializer-list | C++11 | Construire le deque d'abord, puis construire à partir de lui. |

---

## 8. Pièges

- **`pop()` retourne `void`** — `front()` d'abord, puis `pop()`.
- **Impossible d'utiliser `vector`** comme backing container. L'erreur du compiler est cryptique ("no member named `pop_front`"). La norme autorise uniquement `deque` et `list`.
- **`front()` / `back()` sur une queue vide est un UB.** Toujours vérifier `if (!q.empty())` d'abord.
- **Pas d'iterators** — même restriction que stack.
- **Non thread-safe.** Deux threads effectuant un push simultanément mène à un UB.
- **`std::list<int> >` a besoin d'un espace** entre les chevrons fermants en C++98.

---

## 9. Exemple concret — Squelette de BFS (breadth-first search)

```cpp
#include <queue>
#include <vector>
#include <iostream>

void    bfs(int start, const std::vector<std::vector<int> > &graph)
{
    std::vector<bool> seen(graph.size(), false);
    std::queue<int>   frontier;

    frontier.push(start);
    seen[start] = true;

    while (!frontier.empty())
    {
        int node = frontier.front();
        frontier.pop();                     // §4 — front first, then pop
        std::cout << node << " ";

        for (std::size_t i = 0; i < graph[node].size(); ++i)
        {
            int next = graph[node][i];
            if (!seen[next])
            {
                seen[next] = true;
                frontier.push(next);
            }
        }
    }
    std::cout << "\n";
}
```

La BFS est l'utilisation canonique d'une queue : traiter les nœuds dans l'ordre où ils ont été découverts. Une stack ici vous donnerait une DFS (depth-first) à la place — même squelette, container différent.

---

> **Résumé en une ligne.** `std::queue` est le jumeau FIFO de `std::stack`. Le conteneur sous-jacent par défaut est `deque` (vector est interdit), `pop()` est `void`, pas d'iterators. À utiliser pour un traitement dans l'ordre d'arrivée.