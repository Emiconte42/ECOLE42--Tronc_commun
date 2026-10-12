# `<cmath>` — la bibliothèque mathématique C++

`<cmath>` est le wrapper C++ autour du header C `<math.h>`. Chaque fonction
réside dans le namespace `std::` (et, parce que les noms C originaux sont
également réexportés globalement dans la plupart des implémentations, généralement
aussi au scope global — ne comptez pas là-dessus, préférez `std::sqrt`).

En **C++98**, la plupart de ces fonctions existent en trois variantes, une par
type flottant :
- `double f(double)` — sans suffixe
- `float  ff(float)` — suffixe `f` (ex. `sqrtf`)
- `long double fl(long double)` — suffixe `l` (ex. `sqrtl`)

Le sujet du Mod 02 de 42 autorise `roundf` de `<cmath>` pour les exercices sur les
nombres à virgule fixe. C'est la version C99 avec suffixe `float`.

---

## Valeur absolue & signe

| Fonction | Signification |
|---|---|
| `abs(int)` | valeur absolue entière (provenant de `<cstdlib>`, mais également utilisable ici) |
| `fabs(double)` | valeur absolue en virgule flottante — utilisez ceci pour `float`/`double` |
| `fmod(x, y)` | reste en virgule flottante de `x / y`, de même signe que `x` |

Piège : `abs` sur un `double` convertit silencieusement en `int` en C89 ; utilisez
toujours `fabs` avec des flottants.

---

## Puissances & racines

| Fonction | Signification |
|---|---|
| `sqrt(x)` | racine carrée ; non définie pour `x < 0` (retourne NaN) |
| `pow(base, exp)` | `base^exp` ; fonctionne pour les exposants non entiers |
| `exp(x)` | `e^x` |

`pow(x, 0.5)` est équivalent à `sqrt(x)` mais plus lent — préférez `sqrt`.

---

## Logarithmes

| Fonction | Signification |
|---|---|
| `log(x)` | logarithme naturel (népérien), base `e`. Non défini pour `x <= 0` |
| `log10(x)` | logarithme en base 10 |

Pour une base arbitraire : `log(x) / log(base)`.

---

## Trigonométrie (radians)

| Fonction | Signification |
|---|---|
| `sin(x)` / `cos(x)` / `tan(x)` | les trois fonctions usuelles |
| `asin(x)` / `acos(x)` | sinus/cosinus inverse ; l'entrée doit être dans `[-1, 1]` |
| `atan(x)` | tangente inverse, renvoie une valeur dans `(-π/2, π/2)` |
| `atan2(y, x)` | tangente inverse de `y/x`, renvoie une valeur dans `(-π, π]` — prend en compte le quadrant, préférez-la donc à `atan(y/x)` pour les vecteurs |

Degrés ↔ radians : `rad = deg * π / 180`. Il n'y a pas de π intégré,
déclarez donc `const double PI = 3.14159265358979323846;` ou utilisez `acos(-1.0)`.

---

## Fonctions hyperboliques

| Fonction | Signification |
|---|---|
| `sinh(x)` | `(e^x - e^-x) / 2` |
| `cosh(x)` | `(e^x + e^-x) / 2` |
| `tanh(x)` | `sinh(x) / cosh(x)` |

`asinh`/`acosh`/`atanh` n'existent qu'à partir de **C++11** — non disponibles
en mode c++98.

---

## Arrondis

| Fonction | Signification |
|---|---|
| `ceil(x)` | plus petit entier ≥ `x`, retourné sous forme de `double` |
| `floor(x)` | plus grand entier ≤ `x`, retourné sous forme de `double` |
| `roundf(x)` | **C99** — entier le plus proche, arrondit à distance de zéro en cas d'égalité (variante `float`). `round` et `trunc` apparaissent officiellement en C++11, mais `roundf` est couramment disponible dans les toolchains C++98 — c'est pourquoi le Mod 02 l'autorise. |

Le cast `(int)x` tronque vers zéro, ce qui n'est **pas** la même chose que
`floor(x)` pour les nombres négatifs (`(int)-0.7 == 0`, `floor(-0.7) == -1`).

Utilisé dans le Mod 02 ex01 pour convertir un `float` en une valeur brute à virgule fixe :
`raw = roundf(f * (1 << bits));`.

---

## Manipulation des nombres à virgule flottante

| Fonction | Signification |
|---|---|
| `frexp(x, &exp)` | décompose `x` en une mantisse normalisée dans `[0.5, 1)` et un exposant puissance de deux stocké via `exp` |
| `ldexp(x, exp)` | inverse de `frexp` : retourne `x * 2^exp` |
| `modf(x, &intpart)` | décompose `x` en sa partie entière (écrite via le pointer) et sa partie fractionnaire (retournée) |

Ceux-ci sont rarement nécessaires dans les exercices mais utiles lorsque vous souhaitez inspecter
la disposition IEEE-754 sans faire de reinterpret-cast.

---

## Valeurs spéciales & classification

`<cmath>` en C++98 ne fournit **pas** `isnan`/`isinf` en tant que
fonctions standard — celles-ci font partie de C++11. En c++98, vous pouvez détecter NaN
grâce au fait que `x != x` est vrai si et seulement si `x` est NaN, et `HUGE_VAL` est la
constante d'infinité pour `double`.

```cpp
bool is_nan(double x) { return x != x; }
bool is_inf(double x) { return x == HUGE_VAL || x == -HUGE_VAL; }
```

---

## Ce qui n'est **pas** dans le `<cmath>` de C++98

Ne cherchez pas à utiliser ceci dans les modules 42 (la norme c++98 est imposée) :

- `round`, `trunc`, `nearbyint` — C++11
- `log2`, `exp2`, `cbrt`, `hypot` — C++11
- `asinh`, `acosh`, `atanh` — C++11
- `tgamma`, `lgamma`, `erf`, `erfc` — C++11
- `isnan`, `isinf`, `isfinite`, `isnormal` en tant que fonctions — C++11
  (les macros C99 de `<math.h>` peuvent fonctionner selon le compiler)

Si un exercice a besoin de l'une de ces fonctionnalités, le sujet l'autorisera
explicitement (comme le fait le Mod 02 pour `roundf`) ou s'attendra à ce que vous
la réimplémentiez à la main.

---

## Un exemple minimal fonctionnel

```cpp
#include <cmath>
#include <iostream>

int main() {
    double x = 2.0;
    std::cout << "sqrt(2)    = " << std::sqrt(x)       << '\n';
    std::cout << "pow(2, 10) = " << std::pow(x, 10)    << '\n';
    std::cout << "sin(π/2)   = " << std::sin(std::acos(-1.0) / 2) << '\n';
    std::cout << "ceil(-0.5) = " << std::ceil(-0.5)    << '\n';
    std::cout << "floor(-0.5)= " << std::floor(-0.5)   << '\n';
    std::cout << "roundf(-0.5)=" << std::roundf(-0.5f) << '\n'; // -1
    return 0;
}
```

Compilez avec `c++ -Wall -Wextra -Werror -std=c++98 main.cpp` — aucun `-lm`
n'est requis sous Linux pour C++ (la bibliothèque mathématique est linkée par `c++`).