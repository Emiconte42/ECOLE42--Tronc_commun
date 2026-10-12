# `std::priority_queue` — L'adapter basé sur un heap

> **TL;DR.** `std::priority_queue<T>` n'est **pas une queue** — c'est un **binary heap** déguisé en queue. Le prochain élément extrait est toujours le **plus grand** (max-heap par défaut). Adossé à `vector` + les fonctions de heap de `<algorithm>` (`std::make_heap`, `std::push_heap`, `std::pop_heap`). Push et pop sont en O(log n), top est en O(1). Pour un min-heap, passez `std::greater<T>` comme comparateur.

Articles liés : [`STACK.md`](STACK.md) · [`QUEUE.md`](QUEUE.md) · [`VECTOR.md`](VECTOR.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <queue>` *(oui — le même header que `std::queue`)*

---

## 1. Modèle mental

Un binary heap stocké sous forme de vector. Le vector sert de stockage ; les fonctions de heap de `<algorithm>` appliquent l'invariant "parent ≥ enfants".

```
   priority_queue<int> avec les éléments insérés dans l'ordre : 3, 1, 4, 1, 5, 9, 2, 6

   le heap sous forme d'arbre :                  le heap sous forme de vector :

              9                                  index :   0  1  2  3  4  5  6  7
            /   \                                valeurs : 9  6  4  3  5  1  2  1
           6     4
          / \   / \                              parent de i :   (i-1) / 2
         3   5 1   2                             enfants de i :  2*i+1, 2*i+2
        /
       1                                         top() = vector[0] = 9
```

Push : ajout à la fin du vector, puis "sift up" (échange avec le parent jusqu'à ce que le parent soit plus grand). Pop : échange de la racine avec le dernier élément, réduction de la taille du vector de un, puis "sift down" de la nouvelle racine. Les deux sont en O(log n).

Le standard C++98 la définit approximativement comme suit :

```cpp
template <class T,
          class Container = std::vector<T>,
          class Compare = std::less<T> >
class priority_queue {
protected:
    Container c;
    Compare   comp;
public:
    void  push(const T& x) {
        c.push_back(x);
        std::push_heap(c.begin(), c.end(), comp);
    }
    void  pop() {
        std::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
    const T& top() const   { return c.front(); }
    // size, empty, no comparisons
};
```

Notez le comparateur : `std::less<T>` crée un **max-heap** car les fonctions de heap placent le "plus grand" élément en tête lorsque le comparateur est "less".

---

## 2. Construction et choix du comparateur

```cpp
#include <queue>
#include <vector>
#include <functional>          // pour std::greater

std::priority_queue<int> max_pq;                 // par défaut — max-heap

// Min-heap : changer le comparateur pour greater
std::priority_queue<int, std::vector<int>, std::greater<int> > min_pq;

// Pré-remplissage depuis une plage — le ctor appelle make_heap une seule fois
int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
std::priority_queue<int> pq(std::less<int>(), std::vector<int>(data, data + 8));
```

**Conteneur sous-jacent :** doit supporter `front`, `push_back`, `pop_back`, **et** des itérateurs à accès aléatoire (random-access iterators dont les algorithmes de heap ont besoin). C'est `vector` (par défaut) ou `deque`. `list` est rejeté.

> **La signature du ctor en C++98 est peu pratique** — elle prend d'abord le comparateur, puis le conteneur. C++11 a ajouté des constructeurs plus propres. En pratique, on utilise principalement le ctor par défaut et on y insère des éléments avec `push`.

---

## 3. API principale

| Méthode     | Ce qu'elle fait                                          | Coût      |
|-------------|----------------------------------------------------------|-----------|
| `push(x)`   | Insère et réorganise le heap.                            | O(log n)  |
| `pop()`     | Retire le sommet. **Renvoie `void`.**                    | O(log n)  |
| `top()`     | Référence vers le plus grand élément. UB si vide.        | O(1)      |
| `size()`    | Nombre d'éléments.                                       | O(1)      |
| `empty()`   | `size() == 0`.                                           | O(1)      |

**Pas d'itération, pas d'accès aléatoire, pas de `begin`/`end`.** Et fait unique parmi les adapters : **pas d'opérateurs de comparaison** (`==`, `<`). La seule chose possible est push, peek, pop.

---

## 4. Max-heap par défaut — et comment l'inverser

Ce point piège tous les débutants. `std::less<T>` (le comparateur par défaut) signifie "strictement inférieur à" — mais le heap stocke l'élément tel **qu'aucun autre élément ne lui soit supérieur selon la comparaison** en tête. Ainsi, le *front* est le *plus grand*. C'est un max-heap.

```cpp
std::priority_queue<int> pq;
pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
std::cout << pq.top() << "\n";                  // 5  (le maximum)
```

Pour un **min-heap** (le plus petit sort en premier), utilisez `std::greater<int>` :

```cpp
#include <functional>
std::priority_queue<int, std::vector<int>, std::greater<int> > pq;
pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
std::cout << pq.top() << "\n";                  // 1  (le minimum)
```

Les comparateurs personnalisés fonctionnent de la même manière — fournissez un foncteur dont l'`operator()(const T &a, const T &b) const` renvoie true lorsque `a` doit venir **après** `b` dans l'ordre de priorité.

```cpp
struct ByLength {
    bool operator()(const std::string &a, const std::string &b) const {
        return a.size() < b.size();             // la chaîne la plus longue est la "plus grande"
    }
};
std::priority_queue<std::string, std::vector<std::string>, ByLength> pq;
```

---

## 5. Même règle : `pop()` renvoie `void`

Identique à stack et queue :

```cpp
// INCORRECT
int x = pq.pop();

// CORRECT
int x = pq.top();
pq.pop();
```

Même raisonnement : séparer la *lecture* de la *destruction* évite de perdre l'élément si une copie lève une exception.

---

## 6. Conseils d'efficacité

### 6.1 La construction globale l'emporte sur N pushes

```cpp
// LENT — N × O(log N) = O(N log N)
std::priority_queue<int> pq;
for (int i = 0; i < N; ++i) pq.push(data[i]);

// RAPIDE — un seul appel à make_heap : O(N)
std::vector<int> v(data, data + N);
std::priority_queue<int> pq(std::less<int>(), v);   // le ctor appelle make_heap une seule fois
```

La construction par plage d'itérateurs est asymptotiquement plus rapide (`make_heap` est en O(N), pas O(N log N)).

### 6.2 Le conteneur sous-jacent par défaut (vector) est presque toujours le bon

`vector` offre aux fonctions de heap une mémoire contiguë à accès aléatoire — exactement ce dont elles ont besoin. `deque` fonctionne mais ajoute un surcoût de franchissement de blocs à chaque accès. Il n'y a aucune bonne raison de le préférer pour des charges de travail de type heap.

### 6.3 Réserver indirectement

Vous ne pouvez pas appeler `reserve` sur une priority_queue (aucun membre de ce type). Solution de contournement : construire d'abord un vector, y faire le reserve, puis construire la queue à partir de celui-ci.

### 6.4 Dépiler dans une boucle = trier

```cpp
while (!pq.empty()) {
    std::cout << pq.top() << " ";
    pq.pop();
}
```

Il s'agit d'un heapsort. O(n log n), par ordre décroissant (max-heap) ou croissant (min-heap). Pratique lorsque vous souhaitez obtenir les éléments dans l'ordre de priorité un par un sans tout trier à l'avance.

### 6.5 Ne l'utilisez pas comme un "vector trié" — `std::set` ou vector trié + `lower_bound` sont généralement préférables

Si vous avez besoin d'une itération triée, de requêtes par plage, ou de supprimer des éléments arbitraires, le heap n'a pas la structure adaptée — il sait seulement trouver/supprimer le maximum. Choisir le bon outil : priority_queue pour "toujours extraire le meilleur élément suivant" ; set pour "accès trié par clé".

---

## 7. Quand NE PAS utiliser priority_queue

- Vous devez supprimer des éléments arbitraires → utilisez `std::set` (ou implémentez une "suppression paresseuse" en ignorant les sommets périmés au moment du pop).
- Vous devez mettre à jour la priorité d'un élément → pas directement supporté. Solution courante : insérer un doublon avec la nouvelle priorité et ignorer les éléments périmés lors du pop.
- Vous avez besoin d'un ordre stable pour les éléments de même priorité → priority_queue ne donne aucune garantie en cas d'égalité. Intégrez un numéro d'ordre d'insertion dans le comparateur si vous en avez besoin.
- Vous devez itérer sur les éléments → mauvais adapter. Utilisez un vector et `std::sort`/`std::partial_sort`.

---

## 8. Mises en garde spécifiques à C++98

| Fonctionnalité       | Version C++ | Alternative C++98                                |
|----------------------|-------------|--------------------------------------------------|
| `emplace`            | C++11       | `push` — subit une copie.                        |
| Sémantique de déplacement | C++11  | Passage par `const &`.                           |
| Comparateurs lambda  | C++11       | Définir une struct foncteur avec `operator()`.   |
| Ctor plus propre (juste des itérateurs) | C++11 | Ctor C++98 peu pratique prenant `(comp, container)`. |

---

## 9. Pièges courants

- **Par défaut, c'est un *max*-heap, pas un min.** Passez `std::greater<T>` pour un min-heap.
- **`pop()` renvoie `void`.** Appelez `top()` d'abord.
- **Aucun moyen de supprimer des éléments arbitraires.** La structure de données ne le supporte pas ; utilisez `std::set` si nécessaire.
- **Pas d'itérateurs, pas de `begin`/`end`.** Si vous en avez besoin, priority_queue n'est pas l'outil adéquat.
- **Le comparateur applique un *strict weak ordering*.** Renvoyer `true` à la fois pour `comp(a,b)` et `comp(b,a)` constitue un UB.
- **La signature du ctor C++98 est `(Compare, Container)`** — l'ordre compte et il est facile de les inverser par erreur.
- **Pas d'opérateurs de comparaison entre les priority_queues elles-mêmes** — `pq1 == pq2` ne compile pas. Comparez les conteneurs sous-jacents si nécessaire.

---

## 10. Exemple complet — les K plus grands éléments

```cpp
#include <queue>
#include <vector>
#include <functional>
#include <iostream>

int main(void)
{
    int data[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    int N = sizeof(data) / sizeof(data[0]);
    int K = 3;

    // Min-heap de taille K — conserve les K PLUS GRANDS éléments.
    // Astuce : quand il est plein et qu'un nouvel élément est plus grand que le plus petit conservé, on les échange.
    std::priority_queue<int, std::vector<int>, std::greater<int> > heap;

    for (int i = 0; i < N; ++i)
    {
        if ((int)heap.size() < K)
            heap.push(data[i]);
        else if (data[i] > heap.top())
        {
            heap.pop();
            heap.push(data[i]);
        }
    }

    // heap contient désormais les K plus grands éléments (dans l'ordre du min-heap)
    while (!heap.empty())
    {
        std::cout << heap.top() << " ";        // 5 6 9
        heap.pop();
    }
    std::cout << "\n";
    return 0;
}
```
Temps : O(N log K) au lieu du O(N log N) que coûterait un tri complet. Le min-heap de taille K est l'astuce canonique pour le « top K » — et la priority_queue est l'outil idéal car elle expose toujours le pire des meilleurs pour une comparaison peu coûteuse.

---

> **En résumé.** `std::priority_queue` est un max-heap sur un vector avec `push` / `top` / `pop`. Passez en min-heap avec `std::greater<T>`. `push` en O(log n), `top` en O(1). Pas d'itération, pas de suppression arbitraire — elle fait une seule tâche (extraire l'élément extrême) extrêmement bien.