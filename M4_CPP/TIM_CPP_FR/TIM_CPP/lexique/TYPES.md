# `char` / `int` / `float` / `double` / `short` / `long` / `signed` / `unsigned` / `wchar_t` — Fundamental Types

> **TL;DR.** Les types valeur intégrés hérités du C. `int` pour les entiers, `double` pour la virgule flottante, `char` pour les octets/caractères, `bool` pour la vérité — cela couvre 95 % du code de 42. `short`, `long`, `signed`, `unsigned` sont des **modificateurs** qui échangent la plage de valeurs contre la taille. Les tailles sont des *minimums*, pas des garanties.

Associé : [`BOOL.md`](BOOL.md) · [`VOID.md`](VOID.md) · [`SIZEOF.md`](SIZEOF.md) · [`ENUM.md`](ENUM.md) · arithmétique en virgule fixe : [`CPP02.md`](../projets/modules/CPP02.md)

---

## 1. La gamme (macOS/Linux 64-bit typique)

| Type | Taille | Plage (typique) | Utilisation |
|---|---|---|---|
| `bool` | 1 B | `true` / `false` | valeurs de vérité |
| `char` | 1 B | −128…127 | un octet, un caractère ASCII |
| `unsigned char` | 1 B | 0…255 | octets bruts |
| `short` | 2 B | ±32 767 | rarement — stockage compact |
| `int` | 4 B | ±2.1 × 10⁹ | **entier par défaut** |
| `unsigned int` | 4 B | 0…4.2 × 10⁹ | masques de bits, jamais de "compteurs" |
| `long` | 8 B | ±9.2 × 10¹⁸ | quand `int` risque d'overflow |
| `float` | 4 B | ~7 chiffres | **uniquement si le sujet le demande** (CPP02) |
| `double` | 8 B | ~15 chiffres | **virgule flottante par défaut** |
| `wchar_t` | 4 B | wide char | chaînes larges — jamais à 42 |

Le standard promet uniquement `char ≤ short ≤ int ≤ long`. `sizeof(char)` vaut **toujours 1** ; tout le reste dépend de la plateforme — c'est à cela que sert [`sizeof`](SIZEOF.md).

## 2. Les littéraux choisissent un type

```cpp
42      // int
42u     // unsigned
42l     // long
3.14    // double  ← note : PAS float
3.14f   // float
'a'     // char
"a"     // const char[2] — tableau, pas char !
```

C'est pourquoi le CPP02 vous fait écrire `roundf(3.14f)` — mélanger `float` et `double` promeut silencieusement le type.

## 3. Conversions — là où les ennuis commencent

```cpp
int    i = 3.99;        // 3     — troncature, pas d'arrondi
char   c = 300;         // 💥 implementation-defined — ne rentre pas
double d = 1 / 2;       // 0.0   — la division ENTIÈRE a eu lieu en premier
double e = 1.0 / 2;     // 0.5   — un double contamine l'expression
```

La **comparaison signed/unsigned** est le piège classique :

```cpp
std::string s = "hi";
for (int i = 0; i < s.size(); ++i)   // ⚠️ warning int vs size_t
```
`s.size()` renvoie un type unsigned ; si `i` était négatif, la comparaison deviendrait folle (−1 devient immense). Utilisez `std::string::size_type` ou `size_t` pour les indices.

```
   bouclage unsigned :       0
                          ▲  │
                 4294967295◄─┘ −1 "underflow" vers le haut
```

**Overflow :** l'unsigned boucle (défini). L'overflow signed est un **UB** — le compiler peut supposer qu'il ne se produit jamais.

## 4. Les promotions en une image

```
   char, short ──► int ──► unsigned ──► long ──► float ──► double
                  (tout ce qui est petit est promu en int avant le calcul)
```

Deux conséquences : `char + char` donne un `int` ; et afficher le résultat arithmétique d'un `char` peut nécessiter un cast inverse : `static_cast<char>(c + 1)`.

## 5. Conseils

- Paire par défaut : `int` + `double`. N'utilisez les autres que pour une raison valable (le sujet, un buffer d'octets, un masque de bits).
- `unsigned` pour "ne peut pas être négatif" est un piège — la soustraction produit un underflow. Utilisez-le pour les **bits**, pas pour les **compteurs**.
- Comparer des floats avec `==` échoue après des calculs arithmétiques ; comparez `fabs(a-b) < epsilon`. C'est tout l'intérêt du CPP02.