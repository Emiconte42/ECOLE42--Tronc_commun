# Module C++ 02 — Guide d'apprentissage

**Thème du module :** *Polymorphisme ad-hoc, surcharge d'opérateurs et
la Forme Canonique Orthodoxe (OCF).*

Ce module est l'endroit où les classes cessent d'être de simples structs C avec des méthodes et
deviennent de véritables **valeurs** — des objets que vous pouvez additionner, comparer, injecter dans un flux et
copier comme des nombres natifs. Le support utilisé est une classe de nombres à
virgule fixe (`Fixed`). Les mathématiques ne sont qu'un prétexte ; le véritable apprentissage repose sur l'OCF +
la surcharge d'opérateurs + la const-correctness.

Voir aussi : le fichier `TOPICS.md` par module dans [`LEVEL4/C++1/Mod02/`](../../../../LEVEL4/C++1/Mod02/TOPICS.md)
pour des analyses approfondies exercice par exercice, et [`FLOATING.md`](../../../../LEVEL4/C++1/Mod02/FLOATING.md)
pour les notions de base sur la virgule flottante / virgule fixe.

---

## Forme Canonique Orthodoxe — les quatre membres que vous écrirez 100 fois

Chaque class du Mod 02+ doit fournir :

1. **Default constructor** — `Fixed()`
2. **Copy constructor** — `Fixed(Fixed const& src)`
3. **Copy assignment operator** — `Fixed& operator=(Fixed const& other)`
4. **Destructor** — `~Fixed()`

Pourquoi ces quatre membres alors que le compiler les générerait par défaut pour vous ? Parce que
la moulinette évalue les **affichages de trace** que vous placez dans chacun d'eux. Les
fonctions générées par défaut sont silencieuses, vous ne pouvez donc pas les réutiliser.

Explication complète : [`oop/ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md).

L'idiome canonique dans `operator=` :

```cpp
Fixed& Fixed::operator=(Fixed const& other) {
    if (this != &other)       // protection contre l'auto-affectation
        m_raw = other.m_raw;
    return *this;             // permet le chaînage : a = b = c;
}
```

Pour une class qui ne possède que des types primitifs (la class `Fixed` du Mod 02), cette
protection est superflue. Pour une class qui possède de la mémoire sur la heap (le `Dog` du Mod 04), elle
évite un double-free. Prenez cette habitude dès maintenant.

---

## Exercice 00 — Ma première classe en OCF

### Ce que vous apprenez
**Comment écrire une class minimale en OCF complet**, avec des affichages de trace,
en respectant la séparation header/implémentation du projet.

### Structure de la classe
- Private : `int m_raw`, `static const int m_fract_bits = 8`.
- Public : les quatre membres OCF + `int getRawBits() const` et
  `void setRawBits(int const raw)`.

### Conseils
- Initialisez `m_raw` dans la **liste d'initialisation** du ctor dès que possible :
  `Fixed() : m_raw(0) { ... }`. La liste d'initialisation avant le corps est
  l'idiome standard en C++.
- `static const int m_fract_bits = 8;` est une *déclaration +
  initialisation* sur une seule ligne dans le header — une tolérance spéciale
  pour les entiers `const` en c++98. Aucune définition hors ligne n'est requise.
- `getRawBits()` est `const` — la méthode garantit qu'elle ne modifiera pas
  `*this`. Tout accesseur qui se contente de lire doit être `const`.

### Piège à éviter
Le sujet exige que chaque membre de l'OCF et `getRawBits` affiche un message de trace.
`setRawBits` n'affiche généralement rien. Relisez bien l'exemple de sortie.

---

## Exercice 01 — Une classe de virgule fixe utile

### Ce que vous apprenez
**Convertir entre `int` / `float` et la représentation de `Fixed`,
et surcharger `operator<<`** sous forme de fonction non-membre.

### Les quatre conversions, regroupées

```cpp
raw_from_int(int n)       →  n << 8
raw_from_float(float f)   →  roundf(f * 256)
to_int(int raw)           →  raw >> 8
to_float(int raw)         →  raw / 256.0f
```

`<< 8` correspond à un décalage vers la gauche de 8 = multiplier par 256. `>> 8` est un
décalage arithmétique vers la droite sur un int signé (avec extension de signe).
`roundf` provient de `<cmath>`.

### `operator<<` sous forme de fonction libre

```cpp
std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

- **Libre (non-membre)**, car l'opérande de gauche est un
  `std::ostream&` — vous ne pouvez pas ajouter une fonction membre à `ostream`.
- **Retourne le flux** pour que le chaînage fonctionne : `std::cout << a << b`.
- **Aucun `friend` requis** si l'accès se fait via des accesseurs publics
  (`toFloat()`).

### Conseils
- **Piège sur la priorité des opérations dans le ctor de float.** Appliquez `roundf`
  *avant* le cast en `int`, et non après. La forme correcte est
  `static_cast<int>(roundf(f * 256))`. Écrire
  `roundf(static_cast<int>(f * 256))` arrondit la valeur déjà
  tronquée — ce qui ne sert à rien.
- **`std::cout` affiche par défaut les floats avec 6 chiffres significatifs.**
  `Fixed(3.14f)` stocké sous la valeur brute `804` s'affiche sous la forme `3.14062` — c'est
  la précision de la représentation Q8, pas un bug.
- **Ambiguïté :** `Fixed a(42.0)` (un littéral `double`) est ambigu
  face à `Fixed(int)` + `Fixed(float)`. Les tests du sujet utilisent
  uniquement `int` et `float`, vous n'avez donc pas besoin d'une overload pour `double`.

### Piège à éviter
Incluez `<cmath>` pour `std::roundf`. Pas `<math.h>` — convention C++.

---

## Exercice 02 — « Maintenant, on peut parler »

### Ce que vous apprenez
L'**ensemble complet d'opérateurs** sur une class de type numérique : comparaisons,
arithmétique, incrémentation/décrémentation, et **`min` / `max` statiques** avec
des surcharges const et non-const.

Voir [`oop/OPERATOR_OVERLOADING.md`](../../notions/oop/OPERATOR_OVERLOADING.md).

### Comparaisons — six opérateurs, six one-liners

```cpp
bool Fixed::operator<(Fixed const& rhs) const {
    return m_raw < rhs.m_raw;
}
```

Les deux opérandes partagent la même échelle, donc comparer les entiers bruts donne le
même résultat que comparer les valeurs réelles. Les six opérateurs (`<`, `>`, `<=`,
`>=`, `==`, `!=`) sont des variantes de cette même ligne.

### Arithmétique — opérations sur les valeurs brutes, avec un décalage pour `*` et `/`

| Opérateur | Formule brute        | Pourquoi le décalage |
|-----------|----------------------|----------------------|
| `a + b`   | `a_raw + b_raw`      | les échelles correspondent |
| `a - b`   | `a_raw - b_raw`      | les échelles correspondent |
| `a * b`   | `(a_raw * b_raw) >> 8` | le produit brut est mis à l'échelle deux fois, décalage à droite pour compenser |
| `a / b`   | `(a_raw << 8) / b_raw` | le quotient brut perd la mise à l'échelle, décalage à gauche pour la rétablir |

`*` et `/` sont les cas intéressants. La preuve tient en une ligne de
calcul arithmétique — voir [`TOPICS.md` pour Mod02 §ex02](../../../../LEVEL4/C++1/Mod02/TOPICS.md).

### Incrémentation / décrémentation — préfixée vs postfixée

```cpp
Fixed& Fixed::operator++() {          // préfixé : ++a
    ++m_raw;
    return *this;
}
Fixed Fixed::operator++(int) {        // postfixé : a++
    Fixed tmp(*this);
    ++m_raw;
    return tmp;
}
```

Le paramètre `int` de la version postfixée est une **étiquette (tag)** que le compiler
utilise pour choisir l'overload — elle n'est jamais lue. La forme préfixée retourne une
reference ; la forme postfixée retourne une copie de l'état précédent.

Le pas d'incrémentation correspond au **plus petit ε représentable** = brut ±1 = `1/256`
en valeur réelle.

### `min` / `max` statiques — deux surcharges chacun

```cpp
static Fixed&       min(Fixed& a, Fixed& b);         // non-const
static Fixed const& min(Fixed const& a, Fixed const& b);  // const
```

Les deux versions sont nécessaires — la version non-const permet à l'appelant de modifier la
reference retournée (`Fixed::min(x, y) = z;`), la version const fonctionne
avec des arguments const.

### Conseils
- `operator+` retourne par **valeur** (un temporaire nouvellement créé) ; `operator=`
  retourne par **reference** (`*this`). Ce n'est pas un hasard —
  lisez [`oop/OPERATOR_OVERLOADING.md §5`](../../notions/oop/OPERATOR_OVERLOADING.md).
- Déclarez tous les opérateurs de comparaison `const` — ils ne modifient pas
  leurs opérandes.
- N'utilisez pas `std::min` ou `std::max` (pas de STL). Implémentez les versions
  de la class.

### Piège à éviter
`Fixed(256) * Fixed(256)` donne un produit brut de `65536 * 65536 = 2^32`
— ce qui provoque un overflow sur un signed 32-bit. Les tests du sujet restent bien dans la
plage sécurisée, mais connaissez cette limite.

---

## Exercice 03 — BSP (bonus)

### Ce que vous apprenez
**Utiliser `Fixed` pour la géométrie** : déterminer si un point se trouve
*strictement* à l'intérieur d'un triangle.

### Structure de la classe
`Point` avec `Fixed const x` et `Fixed const y` privés —
le `const` sur les membres constitue la particularité de cet exercice.

### Le piège des membres `const`
`Fixed const x;` implique que `x` ne peut plus être assigné après la construction.
Par conséquent, `operator=` ne peut pas accomplir sa tâche habituelle :

```cpp
Point& Point::operator=(Point const&) {
    // x = other.x;  // ← erreur de compilation
    return *this;
}
```

**Solution acceptée :** un corps d'`operator=` vide retournant `*this`.
La class est en pratique immuable après sa construction ; c'est
l'intention de conception.

### Point dans un triangle — signe des produits vectoriels (cross products)

Étant donné le triangle `abc` et le point `p` :

```
d1 = cross(a, b, p)
d2 = cross(b, c, p)
d3 = cross(c, a, p)

strictement à l'intérieur ⟺ les trois signes sont identiques ET aucun n'est nul
```

`cross(a, b, p) = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x)`.

Positif = `p` est à gauche du segment orienté `a→b`. Négatif = à droite.
Zéro = sur la droite contenant le segment.

### Pourquoi utiliser `Fixed` (et non `float`) ici
Deux raisons :
1. **Déterminisme** sur l'ensemble des machines et des compilers — l'arithmétique
   entière ne dérive pas.
2. **Test d'alignement fiable.** `d == 0` en virgule fixe est exact ; avec
   des floats, les erreurs d'arrondi rendent le résultat instable.

### Conseils
- `bsp` est une **fonction libre**, pas un membre. Elle réside dans `bsp.cpp`
  et est déclarée dans un header inclus par `main`.
- Le sens d'enroulement (l'ordre des points `abc`, horaire ou trigonométrique) n'a
  pas d'importance — la condition « les trois signes sont identiques » gère les deux cas.
- Triangle dégénéré (`a, b, c` colinéaires) → les trois produits vectoriels sont
  nuls → renvoie `false`. Ce qui est le comportement attendu.

### Piège à éviter
« Strictement à l'intérieur » signifie que **les sommets et les arêtes renvoient `false`**.
Ajoutez les vérifications `d_i != 0`, et pas seulement la cohérence des signes.

---

## Ce que ce module vous apporte

- Vous savez écrire du code en OCF sans hésiter.
- Vous savez quels opérateurs doivent être membres et lesquels doivent être des fonctions libres.
- La const-correctness devient un réflexe naturel et non une corvée.
- Vous comprenez que la virgule fixe repose sur un principe simple (un entier avec
  un décalage décimal implicite) et représente un outil puissant (géométrie
  déterministe, égalités exactes).
Tout à partir du Mod 03 **part du principe** que vous avez assimilé l'OCF et l'operator overloading. Si vous n'êtes pas encore tout à fait à l'aise avec ces notions, repassez par ex00/01 avant de commencer le Mod 03.





































