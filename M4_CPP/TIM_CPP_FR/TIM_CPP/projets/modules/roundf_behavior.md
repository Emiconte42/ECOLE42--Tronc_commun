# `roundf` — Comportement exact et implémentation matérielle

## Ce que fait réellement `roundf` (la spécification)

`roundf(float x)` de `<math.h>` arrondit à l'**entier le plus proche, avec les cas médians arrondis en s'éloignant de zéro**.

```
roundf( 0.5f) ==  1.0f      roundf(-0.5f) == -1.0f
roundf( 1.5f) ==  2.0f      roundf(-1.5f) == -2.0f
roundf( 2.5f) ==  3.0f      roundf(-2.5f) == -3.0f
roundf( 2.4f) ==  2.0f      roundf(-2.4f) == -2.0f
```

Selon C99/C11, elle doit :

- Arrondir les demi-entiers en s'éloignant de zéro (et NON au pair le plus proche).
- **Ne pas lever** l'exception en virgule flottante `FE_INEXACT`, même lorsque la valeur a une partie fractionnaire.
- Ignorer le mode d'arrondi en virgule flottante courant — son comportement est fixe quel que soit `fesetround()`.

Le résultat reste un `float`, donc pour `|x| ≥ 2²³`, l'entrée est déjà une valeur entière et est renvoyée inchangée.

---

## Pourquoi c'est un cas à part

Il existe cinq fonctions pour « arrondir à l'entier » dans `<math.h>`, et elles diffèrent par des aspects subtils et importants :

| Fonction         | Règle d'arrondi               | Respecte le mode FPU ? | Peut lever inexact ? |
|------------------|-------------------------------|------------------------|----------------------|
| `roundf`         | plus proche, **médians loin de 0** | non                    | non                  |
| `nearbyintf`     | ce qu'indique le mode FPU     | oui                    | non                  |
| `rintf`          | ce qu'indique le mode FPU     | oui                    | oui                  |
| `truncf`         | vers 0                        | non                    | non                  |
| `floorf`/`ceilf` | vers −∞ / +∞                  | non                    | non                  |

Le point gênant : **la règle de `roundf` ne fait pas partie des quatre modes d'arrondi standard IEEE 754.** Les modes IEEE 754 sont :

1. Arrondi au plus proche, **cas médians au pair** (le mode par défaut — « arrondi bancaire »)
2. Arrondi vers 0 (troncature)
3. Arrondi vers +∞
4. Arrondi vers −∞

L'arrondi des « cas médians en s'éloignant de zéro » ne figure *pas* dans cette liste. La norme IEEE 754-2008 l'a ajouté comme mode optionnel (`roundTiesToAway`), mais la plupart des architectures matérielles ne l'implémentent pas comme un mode configurable.

---

## La réalité matérielle

### x86 / x86-64 (SSE / AVX)

L'instruction `ROUNDSS` (SSE4.1) prend une valeur immédiate de 8 bits qui sélectionne :

- `0` — arrondi au plus proche (cas médians au pair)
- `1` — arrondi vers −∞
- `2` — arrondi vers +∞
- `3` — arrondi vers 0
- `4` — utiliser le mode courant du MXCSR

Il n'y a **aucun encodage pour les « cas médians éloignés de zéro »**. Ainsi, `roundf` ne peut pas être une seule instruction sur x86. Les compilers et la libm l'implémentent à l'aide d'une courte séquence : tests + manipulation de bits + troncature.

### ARMv8 (AArch64)

ARM a été plus généreux. AArch64 a ajouté toute une famille d'instructions `FRINT*`, y compris celle dont nous avons besoin :

- `FRINTA` — **arrondi au plus proche, cas médians éloignés de zéro** ← exactement la sémantique de `roundf`
- `FRINTN` — au plus proche, cas médians au pair
- `FRINTM` — vers −∞
- `FRINTP` — vers +∞
- `FRINTZ` — vers zéro
- `FRINTX` — mode courant, *lève* inexact
- `FRINTI` — mode courant, pas d'inexact

Ainsi, sur AArch64, `roundf(x)` se compile en une seule instruction :

```asm
frinta   s0, s0
ret
```

Tandis que le même appel sur une machine x86 antérieure à SSE4.1 nécessiterait environ 10 à 15 instructions de manipulation de bits et un branchement.

---

## Ce que fait réellement l'implémentation libm (x86 / portable)

L'astuce portable consiste à utiliser de l'**arithmétique entière sur le bit pattern IEEE 754**, et non sur le float lui-même. Un `float` est structuré ainsi :

```
[ sign : 1 ][ exponent : 8 ][ mantissa : 23 ]
```

L'exposant réel non biaisé `e = exp_bits - 127` donne la position de la virgule binaire.

L'algorithme (c'est grosso modo ce que font la glibc/musl) :

1. **Extraire les bits** avec une `union` ou un `memcpy` vers un `uint32_t`.
2. **Extraire l'exposant non biaisé** `e`.
3. **Trois cas :**
   - `e < 0` → `|x| < 1`. Le résultat est `±0` si `|x| < 0.5`, sinon `±1`. Simple.
   - `e ≥ 23` → aucun bit fractionnaire n'existe ; `x` est déjà un entier (ou NaN/Inf). Renvoyer tel quel.
   - `0 ≤ e < 23` → il y a une partie fractionnaire à traiter :
     - Calculer un masque pour les bits fractionnaires sous la position `e`.
     - **Les mettre à zéro** pour obtenir une troncature vers zéro.
     - Ensuite, **ajouter conditionnellement 1 ulp à la position entière** si le bit de poids fort éliminé valait 1 (le cas « 0.5 ou plus ») — cela produit naturellement la règle des « cas médians éloignés de zéro », car la magnitude est ajustée avant que le signe ne soit réappliqué.
4. Réécrire les bits dans un float.

Pourquoi une manipulation de bits plutôt que `(int)(x + copysign(0.5f, x))` ? Parce que cette approche « évidente » est **fausse pour les valeurs proches de `0.5 - ulp`** — l'addition elle-même peut arrondir la valeur au supérieur, produisant un résultat erroné sur le dernier bit. La manipulation de bits évite entièrement tout arrondi intermédiaire.

Une version naïve (et subtilement incorrecte) que certains écrivent :

```c
// Ne faites pas cela. Incorrect pour certaines entrées proches des demi-entiers en raison de l'arrondi FP.
float my_roundf(float x) {
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
```

La vraie version de la glibc comporte environ 20 lignes de manipulation de bits, sans aucune arithmétique FP.

---

## Résumé des performances

- **AArch64 :** 1 instruction (`frinta`), latence d'environ 3 cycles, entièrement pipelined.
- **x86 avec SSE4.1+ :** généralement inlined sous forme d'une courte séquence (comparaison, masque, addition prudente de 0.5, `ROUNDSS` vers zéro, etc.), environ 10 à 15 cycles.
- **x86 sans SSE4.1 :** appel de fonction vers la libm, qui exécute la routine de manipulation de bits — plusieurs dizaines de cycles, lourd en branchements.

C'est pourquoi sur les hot paths, on a souvent tendance à :

- Utiliser `(int)x` (troncature) si c'est réellement une troncature qui est souhaitée.
- Utiliser `nearbyintf` si l'arrondi bancaire convient — cela peut se compiler en une seule instruction `ROUNDSS` sur x86.
- Utiliser `lroundf` si l'on souhaite de toute façon un résultat entier (évite l'aller-retour par un `float`).

---

## TL;DR

**La sémantique de `roundf` est mathématiquement naturelle mais architecturalement maladroite** — elle demande un mode d'arrondi que le matériel IEEE 754 n'a historiquement pas pris en charge, ce qui la rend presque toujours plus coûteuse que ses alternatives.