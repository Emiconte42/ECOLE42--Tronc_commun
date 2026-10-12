# `for` / `while` / `do` / `break` / `continue` — Boucles

> **TL;DR.** Trois formes de boucles : `while` (test d'abord), `do…while` (test après — le corps s'exécute au moins une fois), `for` (init + test + pas sur une seule ligne). `break` quitte la boucle ; `continue` saute à l'itération suivante. Toutes se compilent sous la même forme : un saut conditionnel vers l'arrière.

Voir aussi : [`IF_ELSE.md`](IF_ELSE.md) · [`SWITCH.md`](SWITCH.md) · [`RETURN_GOTO.md`](RETURN_GOTO.md)

---

## 1. Les trois formes

```cpp
while (cond) { body; }            // 0..n fois

do { body; } while (cond);        // 1..n fois — notez le ;

for (init; cond; step) { body; }  // l'idiome de comptage
```

```
   while:            do…while:          for:
   ┌─► cond ─false─►exit   ┌─► body        init
   │     │true             │     │           │
   │   body                │   cond ─false─►exit ◄── cond ─false─┐
   └─────┘                 └─true┘           │true               │
                                           body → step ──────────┘
```

**Comment choisir :** compter sur un intervalle connu → `for`. Attendre une condition → `while`. « Exécuter une fois, puis peut-être encore » (menus, invites pour réessayer) → `do…while`.

## 2. Le `for` canonique en C++98

```cpp
for (int i = 0; i < n; ++i)                 // boucle avec index
for (std::string::size_type i = 0; i < s.size(); ++i)  // string-safe
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)  // STL
```

- La variable de boucle déclarée dans `init` **meurt avec la boucle** — son scope est la boucle.
- Les iterators se comparent avec `!=`, pas `<` — tous les iterators ne supportent pas `<`.
- Pas de range-`for` (`for (x : v)`) — c'est du C++11. Repérez-le, signalez-le, réécrivez-le.

## 3. `break` et `continue`

```cpp
for (int i = 0; i < n; ++i) {
    if (skip(i))  continue;   // → saut vers ++i
    if (found(i)) break;      // → saut après la boucle
    process(i);
}
```

- Tous deux agissent uniquement sur la boucle englobante la plus **interne**. Pas de break étiqueté (labeled break) en C++ — quitter proprement des boucles imbriquées est le rôle d'une fonction + `return`, ou d'un flag.
- Un `break` à l'intérieur d'un `switch` placé dans une boucle quitte le **switch**, pas la boucle. Piège classique.

## 4. Pièges

**Boucle infinie par faute de frappe :**

```cpp
for (unsigned i = n; i >= 0; --i)   // 💥 unsigned est TOUJOURS >= 0
```

**Erreur d'un élément (off-by-one) :** `<` vs `<=`. Idiome : intervalles semi-ouverts `[0, n)` — commencez à 0, testez avec `<`, et la taille, le compte et l'index concordent tous.

**Modifier un container pendant son parcours** — cela invalide les iterators. Consultez les règles d'invalidation dans [`VECTOR.md`](../notions/containers/VECTOR.md).

**`while (cin >> x)`** est la boucle de lecture correcte — elle teste l'*état du stream* après la lecture, pas avant :

```cpp
int x;
while (std::cin >> x)   // s'arrête sur EOF ou entrée invalide
    total += x;
```

## 5. Conseils

- Préférez `++i` à `i++` dans les boucles — pour les iterators, `i++` effectue une copie. Une habitude gratuite, un gain occasionnel.
- Sortez les invariants de la boucle (hoisting) : n'appelez pas d'équivalents à `v.size()` coûteux à l'intérieur de la condition.
- Un corps de boucle plus long qu'un écran demande à être extrait dans une fonction.