# `std::stack` — Adaptateur de Container LIFO

> **TL;DR.** `std::stack<T>` n'est **pas un container** — c'est un **adaptateur** léger par-dessus un autre container (`deque` par défaut ; vous pouvez le remplacer par `vector` ou `list`). Il expose uniquement `push`, `pop`, `top`, `size`, `empty`. Le piège le plus courant : **`pop()` renvoie `void`** — il supprime l'élément supérieur mais ne vous le *fournit pas*. Faites toujours `top()` d'abord, puis `pop()`.

Voir aussi : [`QUEUE.md`](QUEUE.md) · [`DEQUE.md`](DEQUE.md) · [`VECTOR.md`](VECTOR.md) · [`LIST.md`](LIST.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header : `#include <stack>`

---

## 1. Modèle mental

Une stack n'est qu'un wrapper qui masque tout sauf l'arrière (back) d'un container sous-jacent :

```
   std::stack<int>            (default = deque underneath)

      ┌──────────────┐
      │   stack<T>   │
      │              │
      │  push() ──── │──→ underlying.push_back()
      │  pop()  ──── │──→ underlying.pop_back()
      │  top()  ──── │──→ underlying.back()
      └──────────────┘
              │
              v
       std::deque<int>           ← the actual data lives here
       [a, b, c, d, e]
                    ↑
                  top
```

C'est l'intégralité de l'implémentation. La norme C++98 définit littéralement `std::stack` comme suit :

```cpp
template <class T, class Container = std::deque<T> >
class stack {
protected:
    Container c;
public:
    void  push(const T& x) { c.push_back(x); }
    void  pop()             { c.pop_back(); }
    T&    top()             { return c.back(); }
    // size, empty, comparisons …
};
```

Le « patron d'adaptateur » (adapter pattern) — restreindre l'interface d'un container existant pour imposer une discipline (LIFO).

---

## 2. Construction et choix du container sous-jacent

```cpp
#include <stack>
#include <vector>
#include <list>

std::stack<int> s;                          // default — deque underneath
std::stack<int, std::vector<int> > sv;      // backed by vector
std::stack<int, std::list<int> > sl;        // backed by list

std::deque<int> seed;                       // pre-populate from another container
seed.push_back(1); seed.push_back(2);
std::stack<int> s2(seed);                   // s2's top is 2
```

> **Remarquez l'espace dans `std::list<int> >`.** En C++98, `>>` est analysé comme l'opérateur de décalage à droite (right-shift). C++11 a corrigé cela ; en C++98, vous devez conserver un espace entre les chevrons fermants consécutifs d'un template.

**Quel container sous-jacent ?**

| Backing       | Avantages                                | Inconvénients                              |
|---------------|------------------------------------------|--------------------------------------------|
| `deque` (défaut) | push/pop en O(1), ne copie jamais lors de l'agrandissement, pas de grosse réallocation | Surcoût par élément dû aux blocs |
| `vector`      | Meilleure localité de cache ; empreinte mémoire minimale pour les petits N | Réalloue et copie tous les éléments lorsque la capacité est épuisée |
| `list`        | push/pop en véritable O(1), itérateurs stables à travers les mutations (sans intérêt pour une stack) | Un `malloc` par push — lent |

**Règle par défaut :** conservez `deque`. Ne passez à `vector` que si vous avez mesuré que la localité de cache importe davantage que le coût rare de réallocation.

---

## 3. API principale

| Méthode     | Ce qu'elle fait                                    | Coût  |
|-------------|----------------------------------------------------|-------|
| `push(x)`   | Ajoute `x` au sommet.                              | Identique au `push_back` sous-jacent. |
| `pop()`     | Supprime le sommet. **Renvoie `void`.**            | Identique au `pop_back` sous-jacent.  |
| `top()`     | Reference vers l'élément supérieur. UB si vide.    | O(1)  |
| `size()`    | Nombre d'éléments.                                 | O(1)  |
| `empty()`   | `size() == 0`.                                     | O(1)  |
| `==`, `<`, etc. | Comparaison lexicographique via le container sous-jacent. | O(n)  |

**C'est tout.** Pas de `begin`, pas d'`end`, pas d'`operator[]`, pas d'iterator. Par conception.

---

## 4. Le piège de `pop()` renvoyant `void`

Tout le monde se fait avoir exactement une fois :

```cpp
// FAUX — pop renvoie void ; cela ne compilera pas.
int x = s.pop();

// CORRECT — lire avec top, puis supprimer avec pop.
int x = s.top();
s.pop();
```

**Pourquoi `pop` est-il void ?** Parce que renvoyer l'élément dépilé par valeur nécessiterait une copie. Si la copie lève une exception (par exemple, si `T` est un type lourd dont le constructeur de copie alloue et qu'une `bad_alloc` est levée), l'élément serait **détruit** mais **jamais transmis à l'appelant** — la valeur serait perdue. C++98 a choisi la sécurité : séparer la lecture (`top`) de la destruction (`pop`), afin qu'une copie qui lève une exception ne puisse pas entraîner de perte de données.

La séparation `top()` + `pop()` est l'un des choix de conception les plus défendus de la norme, et l'un de ceux qui suscitent le plus de récriminations chez les débutants.

---

## 5. Conseils d'efficacité

### 5.1 Conservez `deque` par défaut. Ne passez à `vector` qu'avec une bonne raison.

Pour la plupart des usages, `deque` convient parfaitement. La raison pour laquelle vector peut être tentant :

```cpp
std::stack<int, std::vector<int> > s;       // contiguous storage
```

La vitesse d'itération (si vous contournez l'adaptateur — voir §6) est meilleure avec vector. Mais pour des charges de travail purement composées de push/pop, le coût amorti de deque est généralement identique.

### 5.2 Préallouer lors de l'utilisation d'un vector sous-jacent — mais vous ne pouvez pas accéder à `reserve`

L'adaptateur masque `reserve`. Pour préallouer :

```cpp
std::vector<int> backing;
backing.reserve(1000);
std::stack<int, std::vector<int> > s(backing);   // copies — but capacity carries over
```

Ou bien étendez `std::stack` (le membre protégé `c` est *intentionnellement* protégé pour permettre cela). C'est l'exercice classique `MutantStack` du CPP08.

### 5.3 L'astuce `MutantStack` — exposer les itérateurs sous-jacents

Stack masque les itérateurs. Pour les exposer, dérivez de `std::stack` et accédez au membre protégé :

```cpp
template <typename T>
class MutantStack : public std::stack<T> {
public:
    typedef typename std::stack<T>::container_type::iterator iterator;
    iterator begin() { return this->c.begin(); }   // c is protected, accessible from derived
    iterator end()   { return this->c.end();   }
};
```

Désormais, vous pouvez itérer sur une stack — l'adaptateur standard ne le permet pas, mais le container sous-jacent oui. C'est tout le propos de CPP08 ex02.

### 5.4 Passer par `const &`

Même règle que pour tous les autres containers — copier une stack copie chaque élément sous-jacent.

---

## 6. Quand NE PAS utiliser stack

Si vous avez besoin de *quoi que ce soit* au-delà de push / pop / top — itération, indexation, recherche — utilisez directement le container sous-jacent. L'adaptateur empêche délibérément l'accès à ces opérations.

Utilisez stack quand :
- Vous souhaitez que la discipline LIFO soit imposée au niveau du type (afin que le compiler vous rappelle que « regarder l'élément `i` » n'a pas de sens pour cette collection).
- Vous implémentez un parseur, un évaluateur d'expressions, un parcours en profondeur (depth-first), un tampon d'annulation (undo), ou toute autre logique où le LIFO est l'invariant algorithmique.

Évitez stack quand :
- Vous aurez besoin d'inspecter le contenu plus tard → utilisez un vector.
- Vous aurez besoin d'effectuer un splice entre des stacks → utilisez une list.
- Vous utilisez « plus ou moins » du LIFO mais avez aussi besoin d'autres opérations → utilisez le container sous-jacent.

---

## 7. Mises en garde C++98

| Fonctionnalité | Version C++ | Alternative C++98                |
|----------------|-------------|----------------------------------|
| `emplace`      | C++11       | `push` — paie une copie.         |
| Move semantics | C++11       | Passage par `const &`.           |

---

## 8. Pièges courants

- **`pop()` renvoie `void`.** Vous devez appeler `top()` d'abord.
- **`top()` sur une stack vide est un UB**, pas une exception. Faites toujours `if (!s.empty())` au préalable.
- **Pas d'itérateurs par défaut.** `MutantStack` est la solution de contournement si vous en avez besoin.
- **`std::list<int> >` nécessite un espace** en C++98 — `>>` est analysé comme un right-shift.
- **Comparer deux stacks effectue une comparaison élément par élément** via l'`operator==` du container sous-jacent — assurez-vous donc que `T` le définit.
- **L'assignation d'une stack effectue une copie profonde (deep copy).** Copier une stack de 1M d'éléments copie l'intégralité des 1M d'éléments.

---

## 9. Exemple concret — parenthèses équilibrées

```cpp
#include <stack>
#include <string>
#include <iostream>

bool    is_balanced(const std::string &s)
{
    std::stack<char> st;
    for (std::size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{')
            st.push(c);
        else if (c == ')' || c == ']' || c == '}')
        {
            if (st.empty())
                return false;
            char open = st.top();
            st.pop();                       // §4 — top first, then pop
            if ((c == ')' && open != '(')
             || (c == ']' && open != '[')
             || (c == '}' && open != '{'))
                return false;
        }
    }
    return st.empty();
}

int main(void)
{
    std::cout << is_balanced("([{}])") << "\n";    // 1
    std::cout << is_balanced("([)]")   << "\n";    // 0
    return 0;
}
```

Discipline LIFO classique — chaque ouvrant effectue un push, chaque fermant effectue un pop + vérification. L'adaptateur stack vous interdit de lire tout élément autre que `top`, ce qui correspond exactement à la contrainte requise par l'algorithme.

---

> **Résumé en une phrase.** `std::stack` est une discipline, pas un container. Push/top/pop, jamais de `pop()`-en-tant-qu'expression, le backing par défaut convient très bien, et tournez-vous vers `MutantStack` si jamais vous avez besoin d'itérer.