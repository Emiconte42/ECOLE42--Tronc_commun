# Surcharge d'opérateurs — Guide complet

> **Zoom sur les mots-clés :** [`OPERATOR`](../../lexique/OPERATOR.md) · [`FRIEND`](../../lexique/FRIEND.md) · [`EXPLICIT`](../../lexique/EXPLICIT.md)

La surcharge d'opérateurs vous permet de donner un sens aux opérateurs natifs (`+`, `==`, `<<`, `[]`, …) lorsqu'ils sont appliqués à votre class.

```cpp
Fixed a(3);
Fixed b(4);
Fixed c = a + b;             // a.operator+(b) — parce que vous l'avez défini
std::cout << c;              // operator<<(std::cout, c) — parce que vous l'avez défini
```

Sous le capot, `a + b` n'est *qu'un* simple appel de fonction : `operator+(a, b)` ou `a.operator+(b)`. Tout le reste en découle.

---

## Table des matières

1. [Pourquoi surcharger les opérateurs ?](#1-pourquoi-surcharger-les-opérateurs-)
2. [La liste des opérateurs surchargeables](#2-la-liste-des-opérateurs-surchargeables)
3. [Membre vs non-membre](#3-membre-vs-non-membre)
4. [Signatures canoniques](#4-signatures-canoniques)
5. [Opérateurs arithmétiques](#5-opérateurs-arithmétiques)
6. [Assignation composée (`+=`, `-=`, …)](#6-assignation-composée--)
7. [Opérateurs de comparaison](#7-opérateurs-de-comparaison)
8. [Incrémentation et décrémentation (préfixe vs postfixe)](#8-incrémentation-et-décrémentation-préfixe-vs-postfixe)
9. [Insertion et extraction de flux (`<<`, `>>`)](#9-insertion-et-extraction-de-flux--)
10. [Indirection `[]`, appel de fonction `()`](#10-indirection--appel-de-fonction-)
11. [Opérateur d'assignation `=`](#11-opérateur-dassignation-)
12. [Opérateurs de conversion](#12-opérateurs-de-conversion)
13. [Dans CPP02 — la class Fixed](#13-dans-cpp02--la-class-fixed)
14. [Règles empiriques](#14-règles-empiriques)
15. [Pièges à éviter](#15-pièges-à-éviter)

---

## 1. Pourquoi surcharger les opérateurs ?

Parce que les types utilisateur de type arithmétique, comparables ou affichables **se lisent plus naturellement** avec des opérateurs qu'avec des méthodes nommées :

```cpp
// Sans surcharge
result = sum(mul(a, b), c);

// Avec surcharge
result = a * b + c;
```

Le compiler traduit le second en premier. Même code machine, clarté différente.

**Ne surchargez pas pour le simple plaisir de le faire.** Uniquement lorsque l'opérateur a une signification évidente et universellement acceptée pour votre type. `+` sur `Fixed` est une bonne chose. `+` sur `Car` pour signifier « ajouter une option » rendra tout le monde malheureux.

---

## 2. La liste des opérateurs surchargeables

Vous pouvez surcharger presque tous les opérateurs :

```
Arithmétique :   +   -   *   /   %
Comparaison :    ==  !=  <   >   <=  >=
Logique :        !   &&  ||
Bit-à-bit :      &   |   ^   ~   <<  >>
Composé :        +=  -=  *=  /=  %=  &=  |=  ^=  <<= >>=
Incrémentation : ++  --
Indirection :    []
Appel :          ()
Membre :         ->  ->*
Assignation :    =
Adresse :        &                (rarement une bonne idée d'y toucher)
Flux :           <<  >>           (décalage, réutilisé pour les E/S)
Allocation :     new  delete  new[]  delete[]
Virgule :        ,                 (rarement une bonne idée non plus)
```

**Ne peuvent pas** être surchargés :
```
::   .   .*   ?:   sizeof   typeid
```
Et vous ne pouvez pas inventer de nouveaux opérateurs.

---

## 3. Membre vs non-membre

La plupart des opérateurs binaires peuvent être écrits de l'une ou l'autre manière. Le type de **l'opérande de gauche** en décide.

### Fonction membre

L'opérande de gauche est `*this` :

```cpp
class Fixed {
public:
    Fixed operator+(const Fixed& rhs) const;   // a + b  →  a.operator+(b)
};
```

### Fonction non-membre (libre)

Les deux opérandes sont des paramètres explicites :

```cpp
Fixed operator+(const Fixed& lhs, const Fixed& rhs);   // a + b  →  operator+(a, b)
```

### Quand vous **devez** utiliser une fonction non-membre

Lorsque l'opérande de gauche n'est pas votre class :

```cpp
std::cout << f;    // l'opérande de gauche est std::ostream — PAS votre class
                   // → doit être un non-membre
```

Ainsi, `operator<<` et `operator>>` sont toujours des fonctions libres.

### Accès aux membres private

Un `operator+` non-membre ne peut pas voir directement les champs private. Deux solutions :
- Le construire par-dessus des méthodes publiques (`toRaw()`, etc.)
- Le déclarer `friend` :
  ```cpp
  class Fixed {
      int _raw;
      friend Fixed operator+(const Fixed& a, const Fixed& b);
  };
  ```

---

## 4. Signatures canoniques

Mémorisez-les une bonne fois pour toutes ; vous les écrirez des centaines de fois.

```cpp
// Arithmétique binaire — retourne une nouvelle valeur, les deux côtés const
T   operator+(const T& lhs, const T& rhs);

// Assignation composée — modifie *this, retourne *this par reference
T&  T::operator+=(const T& rhs);

// Comparaison — toujours bool, les deux côtés const
bool operator==(const T& lhs, const T& rhs);

// Insertion de flux — non-membre, retourne le stream par reference
std::ostream& operator<<(std::ostream& os, const T& v);

// Extraction de flux — lit dans un T non-const
std::istream& operator>>(std::istream& is, T& v);

// Assignation — retourne *this par reference, prend const T&
T& T::operator=(const T& rhs);

// Préfixe ++/--
T& T::operator++();                     // ++a
// Postfixe ++/--
T  T::operator++(int);                  // a++ (le int fictif lève l'ambiguïté)

// Indirection — généralement deux surcharges : const et non-const
T& T::operator[](size_t i);
const T& T::operator[](size_t i) const;
```

Ces types de retour sont importants — ils permettent le fonctionnement des expressions chaînées : `a = b = c`, `std::cout << a << b`.

---

## 5. Opérateurs arithmétiques

Pattern canonique : **définir `+=` en tant que membre, puis implémenter `+` en s'appuyant dessus sous forme de non-membre.** Aucune logique dupliquée.

```cpp
class Fixed {
    int _raw;
public:
    Fixed& operator+=(const Fixed& rhs) { _raw += rhs._raw; return *this; }
    int    toRaw() const { return _raw; }
};

Fixed operator+(Fixed lhs, const Fixed& rhs) {   // lhs par valeur — copie que nous allons modifier
    lhs += rhs;
    return lhs;
}
```

### Pourquoi `+` retourne par valeur, et non par reference

`a + b` crée un *nouvel* objet. Retourner une reference renverrait une reference vers une variable locale → dangling pointer.

### Pourquoi `+=` retourne `*this` par reference

Pour que vous puissiez chaîner : `(a += b) += c;`. C'est également la convention standard en C++ — elle reproduit le comportement des types natifs.

---

## 6. Assignation composée (`+=`, `-=`, …)

Ce sont les membres naturels. Implémentez-les en premier :

```cpp
Fixed& Fixed::operator+=(const Fixed& rhs) { _raw += rhs._raw;  return *this; }
Fixed& Fixed::operator-=(const Fixed& rhs) { _raw -= rhs._raw;  return *this; }
Fixed& Fixed::operator*=(const Fixed& rhs) { _raw = (_raw * rhs._raw) >> SCALE; return *this; }
Fixed& Fixed::operator/=(const Fixed& rhs) { _raw = (_raw << SCALE) / rhs._raw; return *this; }
```

Dérivez ensuite les formes binaires :

```cpp
Fixed operator+(Fixed lhs, const Fixed& rhs) { return lhs += rhs; }
Fixed operator-(Fixed lhs, const Fixed& rhs) { return lhs -= rhs; }
// etc.
```

---

## 7. Opérateurs de comparaison

Ils retournent tous `bool`, ils prennent tous `const T&` des deux côtés. Pour des raisons de cohérence, définissez-les comme un **ensemble cohérent**. Généralement, vous écrivez `==` et `<` de zéro, puis dérivez le reste :

```cpp
bool operator==(const Fixed& a, const Fixed& b) { return a.toRaw() == b.toRaw(); }
bool operator< (const Fixed& a, const Fixed& b) { return a.toRaw() <  b.toRaw(); }

bool operator!=(const Fixed& a, const Fixed& b) { return !(a == b); }
bool operator> (const Fixed& a, const Fixed& b) { return   b <  a;  }
bool operator<=(const Fixed& a, const Fixed& b) { return !(b <  a); }
bool operator>=(const Fixed& a, const Fixed& b) { return !(a <  b); }
```

C'est l'idiome C++98. (Le C++20 ajoute l'`operator<=>` pour synthétiser tout cela, mais vous ne l'avez pas.)

---

## 8. Incrémentation et décrémentation (préfixe vs postfixe)

Le préfixe (`++a`) et le postfixe (`a++`) nécessitent des signatures distinctes. Le paramètre `int` fictif permet de lever l'ambiguïté :

```cpp
class Fixed {
public:
    Fixed& operator++()    { ++_raw; return *this; }        // préfixe
    Fixed  operator++(int) { Fixed tmp(*this); ++_raw; return tmp; }  // postfixe
};
```

- Le **préfixe** retourne l'objet par reference, après modification.
- Le **postfixe** sauvegarde l'ancienne valeur, modifie l'objet, puis retourne l'ancienne valeur par **valeur**.

**Conseil de performance :** le préfixe est plus rapide — il retourne une reference, le postfixe effectue une copie. Préférez `++it` à `it++` dans les boucles.

---

## 9. Insertion et extraction de flux (`<<`, `>>`)

Toujours non-membre. Retournent toujours le stream par reference (pour que le chaînage fonctionne).

```cpp
std::ostream& operator<<(std::ostream& os, const Fixed& f) {
    os << f.toFloat();
    return os;
}

std::istream& operator>>(std::istream& is, Fixed& f) {
    float x;
    is >> x;
    f = Fixed(x);
    return is;
}

std::cout << a << b << '\n';   // fonctionne car chaque appel retourne std::cout&
```

Si l'opérateur a besoin d'accéder aux membres private, déclarez-le `friend`.

---

## 10. Indirection `[]`, appel de fonction `()`

### Indirection

Généralement une paire — l'une `const`, l'autre non :

```cpp
class Array {
    int* _data;
public:
    int&       operator[](size_t i)       { return _data[i]; }
    const int& operator[](size_t i) const { return _data[i]; }
};

Array       a(10);   a[0] = 42;
const Array b(10);   int v = b[0];   // appelle la surcharge const
```

### Appel de fonction — foncteurs

Surcharger l'`operator()` rend un objet **appelable** (callable) :

```cpp
struct Adder {
    int base;
    Adder(int n) : base(n) {}
    int operator()(int x) const { return base + x; }
};

Adder add5(5);
add5(10);    // 15 — appelle Adder::operator()
```
C'est ainsi que la STL prend des « fonctions » avant l'existence des lambdas : `std::sort(..., MyComparator());`.

---

## 11. Assignment operator `=`

Fait partie de l'OCF. Voir [ORTHODOX_CANONICAL_FORM.md](ORTHODOX_CANONICAL_FORM.md). Le squelette :

```cpp
Fixed& Fixed::operator=(const Fixed& rhs) {
    if (this != &rhs) {        // self-assignment check
        _raw = rhs._raw;
    }
    return *this;
}
```

Points clés :
- **Prend `const T&`**, car vous ne modifiez pas la source.
- **Retourne `T&` sur `*this`**, pour que `a = b = c` fonctionne.
- **Le self-assignment check** (`this != &rhs`) empêche les bugs lors de la copie de ressources allouées sur le heap.

---

## 12. Conversion operators

Permettent à votre class de se convertir *vers* un autre type :

```cpp
class Fixed {
public:
    operator float() const { return _raw / (float)(1 << SCALE); }
    operator int()   const { return _raw >> SCALE; }
};

Fixed f(3);
float x = f;             // implicit: calls operator float()
int   i = static_cast<int>(f);
```

Remarque : les conversion operators n'ont **pas de type de retour** syntaxiquement — le nom *est* le type cible.

**Attention :** les conversion operators implicites sont le miroir des constructeurs implicites. Si vous ne voulez pas de conversions silencieuses, à 42 vous devez parfois les marquer (C++11 ajoute `explicit` pour les conversion operators ; C++98 ne dispose pas d'une telle fonctionnalité — c'est pourquoi ils peuvent vous surprendre).

---

## 13. Dans CPP02 — la class Fixed

L'ensemble du module est une étude sur l'operator overloading.

**ex02** — vous implémentez, sur une class de virgule fixe :
- `operator+`, `-`, `*`, `/` (arithmétique)
- `operator==`, `!=`, `<`, `>`, `<=`, `>=` (les six comparaisons)
- `operator++`, `--` (à la fois préfixé et postfixé)
- `operator<<` (affichage)
- Deux surcharges statiques de `min`/`max`

**ex03** — la class `Fixed` devient une brique de base pour `Point` et les tests de triangles. Le but est que tous les operators paraissent naturels : `(a + b) / 2`, `if (p1 == p2)`, `std::cout << f`.

Le bon ordre pour les construire :
1. OCF (default ctor, copy ctor, `operator=`, dtor).
2. `toFloat()` / `toInt()` / `toRaw()` — les accesseurs.
3. `operator<<` — pour pouvoir afficher avec `std::cout` votre class dès le premier jour (rend le débogage infiniment plus facile).
4. `operator==`, `<` — la base de toutes les comparaisons.
5. Les quatre autres comparaisons exprimées en termes de `==` et `<`.
6. `operator+=`, `-=`, `*=`, `/=` — puis en dériver `+`, `-`, `*`, `/`.
7. `operator++`, `--` — les deux formes.

---

## 14. Règles empiriques

- **Reproduire la sémantique des types intégrés.** `+` ne doit pas avoir d'effets de bord. `==` doit être réflexif et symétrique. `<` doit être un ordre strict faible (strict weak ordering).
- **`+=` avant `+`.** Écrivez d'abord la version composée, dérivez la version binaire.
- **Les types de retour comptent.** Arithmétique binaire : par valeur. Compound assign : par reference sur `*this`. Comparaisons : `bool`. Streams : le stream par reference.
- **Const tout ce qui peut être const.** Une comparaison ne modifie rien ; chaque paramètre et `*this` doivent être `const`.
- **Non-member pour tout ce où l'opérande de gauche peut ne pas être votre class** (streams, arithmétique nombre-vers-class).
- **Friend seulement si nécessaire.** Si un operator en free function peut être écrit avec des méthodes publiques, ne le rendez pas friend.

---

## 15. Pièges à éviter

- **Mélanger les overloads membres et non-membres d'un même operator** crée une ambiguïté. Choisissez une seule forme.
- **Un `operator=` qui oublie le self-assignment check** casse de façon étrange lorsque vous écrivez `obj = obj` (ou quand `obj` est aliasé via un autre pointer).
- **Un `operator<<` qui prend une `T&` non-const** ne peut pas afficher de temporaires. Toujours `const T&`.
- **Un `++`/`--` postfixé qui retourne une reference** est un UB — vous retourneriez une reference sur une variable locale.
- **Overload `&&` ou `||`** fait perdre l'évaluation en court-circuit (short-circuit evaluation) ; les versions surchargées deviennent des appels de fonction classiques, évaluant les deux côtés. Ne le faites pas.
- **`operator,`** a l'air inoffensif mais c'est un piège sans fond. Ne le surchargez pas.
- **Conversion operators + ctor** peuvent créer une ambiguïté : `Fixed(int)` et `operator int() const` signifient que `a + 1` a deux interprétations valides. Marquez les constructeurs `explicit` lorsqu'ils ne sont pas censés convertir silencieusement.
- **`operator[]` retournant par valeur** fait échouer silencieusement `a[0] = 42` (vous assigneriez à un temporaire). Retournez toujours par reference.

---

## 16. Vue matérielle — ce en quoi les overloaded operators se compilent

### `operator+` sur une class de type primitive

```cpp
Fixed Fixed::operator+(Fixed const& rhs) const {
    Fixed r;
    r.setRawBits(m_raw + rhs.m_raw);
    return r;
}
```

Se compile (avec la RVO) approximativement en :

```asm
Fixed::operator+(Fixed const&):
    mov  eax, [rdi]           ; this->raw
    add  eax, [rsi]           ; + rhs.raw
    mov  [rdx], eax           ; store into the caller's return slot
    ret
```

`rdx` ici est l'emplacement de « retour par valeur » que l'appelant a préalloué.
Aucun objet `Fixed` temporaire n'est construit puis copié —
la RVO le construit sur place. C'est pourquoi le message de trace du copy ctor
**ne se déclenche pas** lorsque vous écrivez `Fixed c = a + b;`.

### `operator*` avec le décalage Q8

```cpp
Fixed Fixed::operator*(Fixed const& rhs) const {
    Fixed r;
    r.setRawBits((m_raw * rhs.m_raw) >> 8);
    return r;
}
```

```asm
Fixed::operator*(Fixed const&):
    mov  eax, [rdi]
    imul eax, [rsi]           ; signed multiply
    sar  eax, 8               ; arithmetic shift right
    mov  [rdx], eax
    ret
```

Un `imul`, un `sar`. Sur un x86 moderne, `imul` prend 3 cycles, `sar` prend
1 cycle. La multiplication en virgule fixe complète prend environ 5 cycles. Comparez à une
multiplication en virgule flottante (`mulss`) : environ 5 cycles également, mais avec
des blocages de 20 cycles sur la gestion des dénormaux/NaN. La virgule fixe l'emporte lorsque
vous opérez sur des pipelines d'entiers.

### `operator<<` pour l'insertion de stream

```cpp
std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

Se compile en un appel à `std::ostream::operator<<(float)`, précédé
par le corps de `Fixed::toFloat()` (généralement inlined). L'operator de stream
n'est **pas** virtual — la résolution a lieu au moment de la compilation
en fonction du type d'argument `float`. C'est pourquoi l'extensibilité des streams
« fonctionne tout simplement » : ajoutez un nouvel overload de `operator<<` pour votre type,
le compiler le trouve via l'ADL (argument-dependent lookup).

### Pré-incrémentation vs post-incrémentation — la différence de coût

```cpp
Fixed& operator++();      // pre
Fixed  operator++(int);   // post
```

Pré-incrémentation : **un store** (`++m_raw`), return `*this`. Un load
(le retour de la ref) sur le site d'appel.

Post-incrémentation : **un copy ctor** (l'instantané préalable), **un store**
(`++m_raw`), **un destructeur** sur le temporaire après que l'appelant
l'a lu. Trois opérations contre une seule.

Pour un `Fixed` (un seul `int`), la copie est gratuite et la différence
disparaît. Pour une class avec un membre `std::string`, la copie est une
allocation. **Règle : préférez `++i` à `i++` lorsque vous n'avez pas besoin de
l'ancienne valeur.** Les boucles avec pré-incrémentation représentent un gain minime et gratuit sur tout
objet qui alloue dans son copy ctor.

### Comparison operators — le chemin rapide

```cpp
bool operator<(Fixed const& rhs) const { return m_raw < rhs.m_raw; }
```

```asm
Fixed::operator<(Fixed const&):
    mov  eax, [rdi]
    cmp  eax, [rsi]
    setl al
    ret
```

Trois instructions. Sans branchement (branch-free). Les processeurs modernes exécutent cela en 1
cycle. Les comparaisons sur `Fixed` sont aussi rapides que sur un `int` brut.

Comparer des `float`s nécessite `ucomiss` + un branchement sur les flags
d'état, plus la gestion des NaN — plus lent et plein de cas particuliers.
Une autre raison pour laquelle la virgule fixe l'emporte pour le code de comparaison critique en performances.