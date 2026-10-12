# `bool` / `true` / `false` — Un vrai type booléen, pas seulement un int

> **TL;DR.** `bool` est un type C++ intégré comportant deux valeurs : `true` et `false`. Conceptuellement un bit ; en pratique **un byte** de stockage. La conversion implicite vers/depuis les entiers est autorisée (tout non-zéro → `true`, zéro → `false`), et c'est la source de la plupart des surprises.

Associé : [`CONST.md`](CONST.md) · [`OPERATOR.md`](OPERATOR.md)

---

## 1. Les bases

```cpp
bool b1 = true;
bool b2 = false;
bool b3 = (x > 0);           // any expression yielding true/false works

if (b1) { ... }              // direct test — clean
```

Le C n'avait pas de `bool`. Il utilisait `int` et une convention (`0` = faux, tout le reste = vrai). Le C++ a ajouté un vrai type booléen ainsi que les littéraux `true` et `false`.

---

## 2. Disposition en mémoire — pourquoi un byte, pas un bit ?

Un `bool` pourrait en principe être stocké sous forme d'un unique bit. En pratique, chaque implémentation C++ fait en sorte que `sizeof(bool) == 1` parce que :

- Les bytes sont adressables individuellement — les bits ne le sont pas. `&myBool` a besoin d'une vraie adresse.
- Aligner plusieurs variables booléennes sur des frontières d'octets permet au CPU de lire chacune d'elles en une seule instruction.
- Des structs composites nécessiteraient une logique coûteuse d'extraction de bits si les booléens faisaient moins d'un octet.

```
   bool b;
   ┌──────┐
   │ 0x01 │     stocké comme un byte complet (ou 0x00 pour false)
   └──────┘     1 byte = 8 bits, mais seul le LSB est conventionnellement utilisé
```

Le motif binaire pour `true` est défini par l'implémentation, mais **conventionnel** — généralement 0x01. `false` est toujours 0x00.

`std::vector<bool>` est une **exception** célèbre : il compacte les booléens en bits, ce qui casse l'interface habituelle de `std::vector<T>` (vous ne pouvez pas faire `&v[0]` pour obtenir un véritable `bool*`). Ne comptez pas sur lui pour se comporter comme un conteneur normal.

```cpp
struct Flags {
    bool a;        // 1 byte
    bool b;        // 1 byte
    bool c;        // 1 byte
    bool d;        // 1 byte
};                  // sizeof = 4 (compacté sans padding car aligné sur char)

struct Mixed {
    int  i;        // 4
    bool b;        // 1
};                  // sizeof = 8 (4 + 1 + 3 tail padding)
```

---

## 3. Conversions implicites — le piège

```cpp
int n = 42;
bool b = n;                  // OK — true (tout int non nul → true)
bool c = 0;                  // false (seul zéro → false)

if (n) { ... }               // légal — n != 0 signifie "true"

bool a = true;
int  i = a;                  // OK — i == 1
int  j = false;              // OK — j == 0
```

C'est à la fois pratique *et* source d'erreurs :

```cpp
bool ok = false;
ok = process();              // process renvoie int — l'appelant ne sait jamais que la valeur de retour a perdu en précision
```

Ou encore :

```cpp
void f(bool flag);
void g(int  n);

f(42);                       // int implicite → bool → true. surprenant ?
g(true);                     // bool implicite → int → 1.
```

La plupart des guides de style recommandent de tester explicitement :

```cpp
if (n != 0) { ... }          // explicite
if (p != 0) { ... }          // explicite (test de pointer NULL)
```

Mais le C++ idiomatique accepte aussi :

```cpp
if (n) { ... }
if (p) { ... }
```

Choisissez un style et restez cohérent.

---

## 4. Opérateurs booléens

```cpp
&&    AND logique, avec court-circuit
||    OR logique,  avec court-circuit
!     NOT logique
```

Le court-circuit signifie :

```cpp
if (p != 0 && p->isValid()) { ... }     // sûr — p->isValid() n'est pas appelé si p est nul
```

L'opérande droit n'est évalué que si nécessaire. Ne cassez pas cela avec des expressions à effets de bord sur le côté droit :

```cpp
if (cheap() || expensive()) { ... }     // expensive() est ignoré quand cheap() est vrai
```

### `&&` / `||` ne sont PAS des opérateurs bitwise

Facile à confondre avec `&` et `|` :

```cpp
bool a, b;
a && b;          // logique : court-circuit, renvoie true/false
a &  b;          // bitwise : évalue les deux, renvoie true/false (même résultat pour les bools mais sans court-circuit)
```

Pour `bool`, le résultat a la même valeur, mais la sémantique diffère. **Utilisez `&&`/`||` pour la logique** et réservez `&`/`|` pour les opérations bitwise entières.

---

## 5. Arithmétique booléenne — oui, c'est légal

```cpp
bool a = true;
bool b = false;

int sum = a + b + a;        // 1 + 0 + 1 = 2 — légal mais étrange
```

Additionner des booléens comme des ints est techniquement valide en raison de la conversion implicite. N'écrivez pas de code ainsi — exprimez clairement votre intention avec des casts explicites ou des compteurs.

C'est pourtant un idiome utile :

```cpp
int trueCount = 0;
trueCount += (x > 0);       // ajoute 1 si vrai, 0 si faux
trueCount += (y > 0);
trueCount += (z > 0);
```

Une méthode compacte pour compter les conditions satisfaites.

---

## 6. Affichage des booléens

```cpp
std::cout << true;            // 1
std::cout << false;           // 0

std::cout << std::boolalpha;  // passage en mode "true"/"false"
std::cout << true;            // true
std::cout << std::noboolalpha;
std::cout << true;            // à nouveau 1
```

`std::boolalpha` provenant de `<ios>` (inclus par `<iostream>`) active l'affichage des booléens sous forme lisible. Persistant — s'applique au flux jusqu'à réinitialisation.

---

## 7. Les booléens dans les conditions — contextes de conversion

Le standard C++ parle de "conversion contextuelle en bool". Partout où une condition est attendue (`if`, `while`, `for`, ternaire `?:`, `&&`, `||`, `!`), l'opérande est converti en `bool`. Ainsi :

```cpp
int *p = ...;
if (p) { ... }          // p se convertit en bool : pointer non nul → true

std::string s = ...;
if (s.empty()) { ... }  // s.empty() renvoie bool — test direct
```

Dans vos propres classes, vous pouvez activer cela avec `operator bool()` (C++98) — mais c'est un terrain glissant :

```cpp
class Smart {
public:
    operator bool() const { return _ptr != 0; }
};

Smart s;
if (s) { ... }          // OK
int n = s + 1;          // ÉGALEMENT OK en C++98 — bool implicite→int→arithmétique
```

Cette chaîne implicite provoque des bugs subtils. L'`explicit operator bool()` de C++11 corrige cela ; C++98 n'a pas d'opérateurs de conversion explicites. La solution de contournement à la norme 42 est d'exposer une méthode nommée comme `isValid()` à la place.

---

## 8. Trucs & astuces

### 8.1 Les paramètres `bool` réduisent la lisibilité

```cpp
sendEmail(addr, true, false);        // que signifient ces flags ?
```

Remplacez-les par des enums nommés :

```cpp
enum Format { TEXT, HTML };
enum Priority { NORMAL, URGENT };

sendEmail(addr, HTML, NORMAL);       // auto-documenté
```

### 8.2 Ne comparez pas à true/false explicitement

```cpp
if (b == true)  { ... }     // verbeux
if (b == false) { ... }     // verbeux

if (b)  { ... }             // idiomatique
if (!b) { ... }             // idiomatique
```

### 8.3 Évitez `bool` dans les boucles numériques serrées

```cpp
bool flags[100];           // 100 bytes
unsigned char flags[100];  // également 100 bytes, mais fonctionne avec les ops bitwise
```

Pour un stockage compact de bits, utilisez `std::bitset<N>` (taille à la compilation) ou `std::vector<bool>` (taille à l'exécution — mais attention à l'interface proxy).

### 8.4 Renvoyer bool depuis `operator==` etc.

```cpp
bool operator==(const C&) const;
bool operator< (const C&) const;
bool operator!=(const C&) const;
```

Signatures standard. Toujours `bool`.

### 8.5 Le court-circuit de `||` / `&&` est une fonctionnalité

```cpp
// parcourir une liste en toute sécurité
while (node && node->isValid()) {
    process(node);
    node = node->next;
}
```

C'est l'un des patterns les plus idiomatiques en C et C++. Utilisez-le.

### 8.6 Ne surchargez pas `&&` / `||` / `,`

Les versions surchargées (overload) ne court-circuitent pas (les deux opérandes sont évalués). Surprenant ; presque toujours une mauvaise idée. Voir [`OPERATOR.md`](OPERATOR.md).

---

## 9. Erreurs fréquentes

| Erreur | Cause | Solution |
|---|---|---|
| L'argument `bool` se convertit silencieusement depuis un int | Conversion implicite int→bool | Tester explicitement (`if (n != 0)`) ou utiliser des enums nommés |
| La sortie est `1`/`0` au lieu de `true`/`false` | Absence de `std::boolalpha` | `std::cout << std::boolalpha << b;` |
| `vector<bool>` ne peut pas renvoyer un vrai `bool*` | Spécialisé pour compacter les bits | Utiliser `std::vector<char>` si vous avez besoin d'un "vector de bools" avec l'interface habituelle |
| L'overload d'opérateur ne court-circuite pas | `operator&&` / `operator||` personnalisé | Ne pas les surcharger |

---

## 10. Résumé visuel

```
              ┌──────────────────────────────────────────────┐
              │   mot-clé : bool                             │
              │       valeurs : true, false                  │
              │       taille :  typiquement 1 byte           │
              └────────────────────┬─────────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────────┐
              ▼                    ▼                         ▼
         opérations           conv implicite         cas d'usage
        ────────────          ─────────────          ───────────
        && || logique         bool ↔ int             flags, conditions,
        avec court-circuit    bool ↔ pointer         types de retour de
        !     négation        (tout non-nul est      prédicats,
        true/false            vrai)                  operator ==/!=/<
        littéraux
                              PEUT être subtil —
                              utiliser des tests
                              !=0 explicites en
                              cas de doute.

       mémoire : 1 byte (8 bits)       std::vector<bool> compacte en bits
       affichage : 1/0 par défaut      std::boolalpha pour true/false
       éviter les paramètres bool      préférer les enums nommés
```
---

## 11. Pratique

1. Pourquoi `sizeof(bool) == 1` et non `sizeof(bool) == 0` (un seul bit) ? *(Les octets sont la plus petite unité adressable ; les bool ont besoin d'adresses pour `&b`, le passage de paramètres, etc.)*
2. Qu'affiche `std::cout << true;` ? *(`1` par défaut ; `true` si vous avez configuré `std::boolalpha` sur le stream.)*
3. Pourquoi `if (n)` fonctionne-t-il lorsque `n` est un `int` ? *(Conversion contextuelle en bool : zéro → false, toute autre valeur → true.)*
4. Pourquoi l'overload de `operator&&` est-il rarement une bonne idée ? *(Les overloads perdent la sémantique de court-circuit — les deux opérandes sont évalués, ce qui surprend tous les lecteurs.)*