# `std::deque` — File à double extrémité (la « liste de blocs »)

> **TL;DR.** Un `std::deque<T>` est **une liste de tableaux de taille fixe** (appelés *blocs* ou *chunks*). Il offre un push/pop en O(1) aux **deux extrémités** *et* un accès aléatoire en O(1) — mais le stockage n'est **pas contigu**, donc `&d[0]` n'est pas un pointer utile vers un buffer. Les iterators sont à accès aléatoire mais légèrement plus coûteux que ceux d'un vector car ils peuvent devoir franchir les limites des blocs.

Voir aussi : [`VECTOR.md`](VECTOR.md) · [`LIST.md`](LIST.md) · [`STACK.md`](STACK.md) · [`QUEUE.md`](QUEUE.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <deque>`

---

## 1. Modèle mental

Un deque contient un petit tableau de pointers vers des blocs de taille fixe. Chaque bloc stocke N éléments (typiquement 512 octets / `sizeof(T)`, mais cela dépend de l'implémentation). Le deque suit l'endroit où la zone active commence à l'intérieur du premier bloc et où elle se termine à l'intérieur du dernier bloc.

```
   std::deque<int> d, after a few push_back/push_front

   block_map (small array of pointers)
   ┌───┬───┬───┬───┐
   │ A │ B │ C │ D │     ← four block pointers
   └─┬─┴─┬─┴─┬─┴─┬─┘
     │   │   │   │
     v   v   v   v
   [...][...][...][...]   ← four heap-allocated blocks of N ints each
       └─ live region ─┘

   front_iterator points into block A at some offset
   back_iterator  points into block D at some offset
```

Lorsque vous faites un `push_front` et que le premier bloc se remplit, le deque alloue un nouveau bloc et insère son pointer au début de la block_map. Lorsque la block_map elle-même est pleine, *celle-ci* est réallouée — mais les **blocs de données eux-mêmes ne bougent pas**. C'est pourquoi les references vers les éléments intermédiaires restent (la plupart du temps) valides.

Pour l'accès aléatoire `d[i]` : calculez `(block_index, offset_in_block)` et déréférencez. Deux accès en tableau au lieu d'un seul — un cheveu plus lent que `vector`, mais toujours en O(1).

---

## 2. Construction

Menu identique à `vector` :

```cpp
#include <deque>

std::deque<int> a;
std::deque<int> b(5);                        // five 0s
std::deque<int> c(5, 42);                    // five 42s
std::deque<int> d(c);                        // copy
std::deque<int> e(c.begin(), c.end());       // range
std::deque<int> f(arr, arr + 10);            // from C array

a = c;
a.assign(5, 7);
```

Il n'y a pas de `reserve` ni de `capacity()` — deque ne dispose pas d'un buffer unique extensible dans lequel réserver de l'espace.

---

## 3. API principale

| Méthode                             | Coût          | Note                                   |
|-------------------------------------|---------------|----------------------------------------|
| `push_back(x)` / `push_front(x)`    | O(1) amorti   | Alloue un nouveau bloc si nécessaire.  |
| `pop_back()` / `pop_front()`        | O(1)          | Peut libérer un bloc quand il devient vide. |
| `d[i]`, `d.at(i)`                   | O(1)          | Deux indirections (bloc + offset).     |
| `front()` / `back()`                | O(1)          | UB si vide.                            |
| `insert(it, x)` / `erase(it)`       | O(n) au milieu, O(1) aux extrémités | Décale la moitié la plus courte. |
| `clear()`                           | O(n)          | Libère chaque bloc.                    |
| `size()` / `empty()`                | O(1)          |                                        |
| `resize(n)` / `resize(n, x)`        | O(\|Δ\|)      |                                        |
| `swap(other)`                       | O(1)          | Échange le pointer de la block_map.    |
| `begin()` / `end()` / `rbegin()` / `rend()` | random-access | Légèrement plus lourds que les iterators de vector. |

**Pas de `data()`, pas de `reserve`, pas de `capacity`.** Deque gère activement plusieurs blocs ; « le buffer » n'existe pas en tant qu'allocation unique.

---

## 4. Invalidation des iterators — le piège de deque

C'est ici que la plupart des débutants se font piéger. Les règles d'invalidation de deque sont **plus strictes qu'elles n'en ont l'air** :

| Opération                    | Iterators                          | References / pointers                             |
|------------------------------|------------------------------------|---------------------------------------------------|
| `push_back` / `push_front`   | **Tous les iterators sont invalidés.** | Les references vers les éléments existants **restent valides**. |
| `pop_back` / `pop_front`     | Iterators vers l'extrémité dépilée + `end()` | Les references vers les autres éléments restent valides. |
| `insert(middle)` / `erase(middle)` | Tous les iterators sont invalidés. | Toutes les references sont invalidées.            |
| `clear()`                    | Tous les iterators                 | Toutes les references                             |
| `swap(other)`                | Aucun des vôtres                   | Aucun des vôtres                                  |

Notez l'asymétrie : ajouter un élément à un deque **ne déplace pas les éléments existants** (les blocs de données ne sont pas réalloués), donc les **pointers et les references restent valides** — mais l'implémentation de l'iterator peut avoir besoin de recalculer l'état interne de la block-map, de sorte que les iterators sont prudemment tous invalidés.

En pratique : si vous détenez un `T*` ou une `T&` vers un élément d'un deque, il reste valide à travers `push_back` / `push_front` (propriété unique à deque). Si vous détenez un iterator, il ne l'est pas. **Utilisez des indices** dans les boucles de modification, tout comme avec vector.

---

## 5. Conseils d'efficacité

### 5.1 Choisissez deque lorsque vous effectuez réellement des push aux deux extrémités

```cpp
std::deque<int> d;
d.push_front(1);                  // O(1) — vector would be O(n)
d.push_back(2);                   // O(1) — same as vector
d.pop_front();                    // O(1) — vector would be O(n)
```

Si votre algorithme consiste à « construire une fenêtre glissante » ou une « queue avec accès aléatoire », deque est le bon outil.

### 5.2 Ne choisissez pas deque simplement parce que vector « gaspille de la capacité »

Deque alloue par bloc, donc un deque vide possède déjà un bloc (~512 octets de surcharge). Pour les petites collections, un vector avec `reserve(n)` est plus économe.

### 5.3 L'itération est plus lente qu'avec vector — mesurez si cela a de l'importance

Chaque déréférencement d'un iterator peut nécessiter de vérifier si vous avez traversé une limite de bloc. Pour les boucles numériques serrées sur des millions d'éléments, vector l'emporte. Pour les schémas d'accès mixtes, la différence se noie dans le bruit.

### 5.4 `swap` pour réduire l'espace mémoire — même astuce que vector

```cpp
std::deque<int>(d).swap(d);       // shrink to fit
std::deque<int>().swap(d);        // free all blocks
```

`clear()` libère les blocs de manière paresseuse (certaines implémentations en conservent un) ; l'idiome du swap force une libération complète.

---

## 6. Quand NE PAS utiliser deque

| Si vous avez besoin de…                             | Choisissez…                               |
|-----------------------------------------------------|-------------------------------------------|
| Un buffer à transmettre à une API C (`read`, `write`, `memcpy`) | `vector` — lui seul vous donne un pointer vers un unique bloc contigu. |
| Une vitesse d'itération maximale                    | `vector`                                  |
| Nombreuses insertions au milieu                     | `list`                                    |
| Accès trié par clé                                  | `set` / `map`                             |
| Pur LIFO / FIFO avec une API restreinte             | `stack` / `queue` (qui utilise deque en interne par défaut) |

**Le cas d'usage idéal de deque est restreint :** push/pop aux deux extrémités + accès aléatoire. En dehors de cela, vector est préférable.

---

## 7. Particularités de C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `emplace_*`            | C++11       | `push_*` — paie le coût d'une copie.        |
| `shrink_to_fit()`      | C++11       | L'astuce du swap (§5.4).                    |
| Membre `data()`        | Jamais (deque est non contigu) | N/A — par conception.      |
| Move semantics         | C++11       | Non disponible ; les copies de deque sont en O(n). |

---

## 8. Pièges à éviter

- **`&d[0]` n'est *pas* un pointer vers l'ensemble des données**, seulement vers le premier bloc. Ne le passez pas à `read`/`write`/`memcpy` en espérant lire ou écrire `n * sizeof(T)` octets.
- **`push_front` invalide tous les iterators**, même s'il ne réalloue pas les blocs de données. La block_map peut s'agrandir.
- **Les references sont plus stables que les iterators.** Un `T&` / `T*` vers des éléments existants survit à un `push_back` / `push_front`. Les iterators ne survivent pas.
- **Deque a un overhead par élément plus important que vector** pour un N faible — au moins un bloc (souvent 512 octets) plus la block_map. Pour N < ~32, vector l'emporte presque toujours.
- **Pas de `reserve`.** Vous ne pouvez pas préallouer un nombre d'éléments ; la croissance se fait automatiquement par bloc.

---

## 9. Exemple concret — fenêtre glissante

```cpp
#include <deque>
#include <iostream>

int main(void)
{
    std::deque<int> window;
    int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
    int N = sizeof(data) / sizeof(data[0]);
    int K = 3;

    for (int i = 0; i < N; ++i)
    {
        window.push_back(data[i]);
        if ((int)window.size() > K)
            window.pop_front();             // O(1) — the deque sweet spot

        if ((int)window.size() == K)
        {
            std::cout << "window: ";
            for (std::size_t j = 0; j < window.size(); ++j)
                std::cout << window[j] << " ";
            std::cout << "\n";
        }
    }
    return 0;
}
```
Les deux extrémités se déplacent à chaque étape. Avec un `vector`, chaque `pop_front` serait en O(K). Deque le maintient en O(1).

---

> **Résumé en une ligne.** `std::deque` est une liste de petits tableaux. Utilisez-le lorsque vous faites des push aux **deux** extrémités *et* voulez un random access. Sinon vector.