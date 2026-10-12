# `explicit` — "Ne convertis pas dans mon dos"

> **TL;DR.** Un constructeur à un argument agit comme un **opérateur de conversion implicite** à moins de le marquer `explicit`. Presque tous les constructeurs à argument unique devraient être `explicit`.

Lié : [`CASTS.md`](CASTS.md) · [`OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. Le problème

```cpp
class Fixed {
    int _raw;
public:
    Fixed(int v) : _raw(v << 8) {}     // non marqué explicit
};

void process(Fixed f);

process(42);                            // ← quoi ?
```

Ce qui s'est passé : le compiler a vu que `process` attend un `Fixed`, vous lui avez passé un `int`, et il existe un constructeur à un argument `Fixed(int)`. Le compiler a discrètement exécuté `Fixed(42)` à votre place — une **conversion implicite**.

Parfois, c'est ce que vous voulez. Souvent, c'est un bug :

```cpp
std::string s(5);                       // s = ??? — en fait une string de 5 caractères NUL
//                                        parce que std::string(size_t) existe
```

---

## 2. La solution

```cpp
class Fixed {
    int _raw;
public:
    explicit Fixed(int v) : _raw(v << 8) {}
};

process(42);          // ERREUR — pas de conversion implicite
process(Fixed(42));   // OK — conversion explicite
```

`explicit` signifie : *"pour fabriquer un `Fixed` à partir d'un `int`, l'appelant doit l'écrire explicitement."*

---

## 3. Quand la conversion implicite se déclenche-t-elle ?

Partout où le compiler peut appeler votre constructeur à argument unique pour satisfaire un type :

```cpp
class S {
public:
    S(const char *);    // non explicit
};

void f(S s);

f("hi");                 // implicite : f(S("hi"))

S a = "hi";              // implicite (copy-init) — appelle S(const char*)
S b("hi");               // syntaxe explicite (direct-init)
S c = S("hi");           // également explicite (direct-init via un temporaire)
```

Si le constructeur était `explicit` :

```cpp
class S {
public:
    explicit S(const char *);
};

S a = "hi";              // ERREUR — copy-init interdite
S b("hi");               // OK     — direct-init toujours autorisée
S c = S("hi");           // OK     — direct-init via un temporaire
f("hi");                 // ERREUR
f(S("hi"));              // OK
```

Moyen mnémotechnique : **`explicit` bloque la forme avec `=`, autorise la forme avec `()`.**

---

## 4. La chaîne de conversion

Le compiler est autorisé à insérer **au maximum une** conversion définie par l'utilisateur dans une même séquence de conversion :

```cpp
class A { public: A(int); };
class B { public: B(A); };

B b = 42;        // 42 → A → B  ?
                 // NON — cela fait deux conversions définies par l'utilisateur. Rejeté.

B b = A(42);     // OK — une seule définie par l'utilisateur : A → B
```

Marquer `A(int)` comme `explicit` empêche le premier maillon de la chaîne. Marquer `B(A)` comme `explicit` empêche le second.

---

## 5. La règle empirique

> **En C++98, les constructeurs à un seul argument doivent être `explicit` par défaut.** Ne retirez `explicit` que lorsque la conversion implicite constitue l'*interface voulue*.

Cas où l'implicite *est* voulu :
- `std::string(const char *)` — chaque base de code compte sur le fait que `"hello"` devienne une `std::string`.
- Les types de wrapper / smart-pointer dont le but entier est de se comporter comme l'élément encapsulé.
- Les types numériques qui imitent un type natif (par exemple, un `Fixed` qui doit se comporter comme un `float`).

Pour votre classe moyenne de module 42 ? **Toujours `explicit`.**

---

## 6. Vision matérielle et compiler

`explicit` est purement une **indication pour le système de types**. Aucun coût à l'exécution, aucune métadonnée, aucune différence de code machine entre `explicit` et non-explicit. Le compiler modifie simplement ses règles d'overload resolution :

```
   site d'appel                 overload resolution
   ────────────                 ───────────────────
   process(42);                 candidat : process(Fixed)
                                  nécessite : int → Fixed
                                  source :    Fixed(int) ctor
                                  est-il explicit ?
                                       ┌───── oui ──→ rejeter ce candidat
                                       └───── non ──→ accepter (coût = 1 conv utilisateur)
```

Si aucun candidat ne survit → erreur de compilation. L'optimiseur ne voit jamais `explicit` ; il a disparu avant la génération de code.

---

## 7. Note C++11+ (pour le contexte, 42 utilise C++98)

C++11 a étendu `explicit` aux **opérateurs de conversion** :

```cpp
class Smart {
public:
    explicit operator bool() const { return _ptr != 0; }
};

Smart s;
if (s) {}              // OK — conversion en bool dans un "contexte booléen"
bool b = s;            // ERREUR
bool b = bool(s);      // OK
```

En C++98, les opérateurs de conversion (`operator T()`) ne peuvent pas être marqués explicit. C'est pourquoi la norme à 42 est de les éviter ou d'utiliser l'idiome safe-bool.

---

## 8. Constructeurs multi-arguments et sans argument

```cpp
class P {
public:
    P();                     // default ctor — explicit est autorisé mais inutile en C++98
    P(int);                  // argument unique — doit être explicit
    P(int, int);             // deux arguments en C++98 — ne convertit jamais implicitement
                             // (mais en C++11 avec la brace-init il LE PEUT : P p = {1, 2};
                             //  → le marquer explicit bloque la copy-list-init)
};
```

En C++98, seuls les constructeurs à un seul argument effectuent une conversion implicite, donc seuls ceux-ci ont besoin d'`explicit`. Un constructeur avec deux paramètres **obligatoires** ne peut pas se déclencher à partir d'une seule valeur source.

Un constructeur avec un paramètre obligatoire et les autres par défaut **compte comme un constructeur à un seul argument** :

```cpp
class Q {
public:
    Q(int x, int y = 0);    // peut convertir depuis un int — doit être explicit
};
```

---

## 9. Astuces et conseils

### 9.1 Ajoutez `explicit` d'abord ; ne le retirez qu'avec une raison valable

Lorsque vous écrivez un nouveau ctor à un argument, collez-lui `explicit`. Si un appelant se heurte à "pas de conversion implicite", vérifiez s'il ne devrait pas de toute façon écrire explicitement `T(x)` — en général, oui.

### 9.2 Lisez le code de la bibliothèque sous cet angle

```cpp
std::vector<int> v(10);          // 10 int construits par défaut — ctor explicit
std::vector<int> v = 10;         // ERREUR — vector<T>(size_t) est explicit
```

La bibliothèque standard utilise abondamment `explicit`. Observez où, et pourquoi.

### 9.3 Combinez avec un paramètre `const` reference

```cpp
class Buffer {
public:
    explicit Buffer(const std::string& filename);
};

void load(const Buffer& b);

load("data.txt");                // ERREUR — nécessiterait une conversion implicite string→Buffer
load(Buffer("data.txt"));        // OK
```

### 9.4 Constructeur de copie explicit — presque jamais utile

```cpp
class X {
public:
    explicit X(const X&);    // légal mais inhabituel
};

X a;
X b = a;                     // ERREUR — copy-init interdite
X c(a);                      // OK
```

Cela casse le passage par valeur et de nombreux patterns idiomatiques. Ne le faites pas sans une excellente raison.

### 9.5 Méfiez-vous de `BankAccount(double)`

```cpp
class BankAccount {
public:
    BankAccount(double initialBalance);
};

void transfer(BankAccount from, BankAccount to);

transfer(100.0, account);    // crée implicitement un NOUVEAU compte avec 100 $ — mauvais compte !
```

L'ajout d'`explicit` aurait permis de détecter cette erreur.

---

## 10. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `conversion from 'int' to non-scalar type 'X' requested` | Vous avez utilisé la forme `=` avec un ctor `explicit` | Utilisez la forme direct-init : `X x(42);` |
| Surcharge surprenante sélectionnée | Conversion implicite via un ctor à argument unique non-explicit | Marquez le ctor `explicit` |
| `more than one conversion required` | Tentative d'enchaîner deux conversions définies par l'utilisateur | Insérez une conversion explicite au milieu |

---

## 11. Résumé visuel

```
              ┌───────────────────────────────────────┐
              │   class T { T(U); }     non explicit  │
              │       ▲                               │
              │       │                               │
              │   valeur U    ───── le compiler peut ─►
              │                       construire T    │
              │                       discrètement    │
              └───────────────────────────────────────┘
                              │
                              │  ajouter 'explicit' →
                              ▼
              ┌───────────────────────────────────────┐
              │   class T { explicit T(U); }          │
              │       ▲                               │
              │       │                               │
              │   valeur U ─────  le compiler refuse  │
              │                   à moins que         │
              │                   l'appelant écrive   │
              │                   T(valeur)           │
              └───────────────────────────────────────┘
```

---

## 12. Pratique

1. Pourquoi `std::string s = "hello";` fonctionne-t-il alors que `std::vector<int> v = 5;` échoue ? *(`std::string(const char*)` est implicite ; `std::vector<T>(size_t)` est `explicit`.)*
2. `explicit` est-il vérifié au moment de la compilation ou à l'exécution ? *(À la compilation uniquement ; `explicit` ne produit aucun code.)*
3. Pourquoi la règle empirique est-elle de "rendre chaque ctor à argument unique explicit" ? *(La plupart des conversions accidentelles sont des bugs ; le coût d'écrire `T(x)` sur le site d'appel est minime ; le coût de débogage d'une conversion implicite est élevé.)*