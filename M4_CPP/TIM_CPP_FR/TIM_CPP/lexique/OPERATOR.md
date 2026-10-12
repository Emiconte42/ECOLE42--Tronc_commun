# `operator` — Faire en sorte que votre type se comporte comme un type natif

> **TL;DR.** `operator` est un mot-clé utilisé pour déclarer une fonction qui surcharge un opérateur natif (`+`, `-`, `<<`, `[]`, `()`, conversions, etc.). Cela ressemble à du sucre syntaxique, mais c'est une fonction ordinaire — même convention d'appel, même résolution de surcharge, mêmes décisions à la compilation.

Connexe : [`OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`FRIEND.md`](FRIEND.md) · [`EXPLICIT.md`](EXPLICIT.md) · [`THIS.md`](THIS.md)

---

## 1. La syntaxe

```cpp
ReturnType operator<symbol>(parameters);
```

Le mot-clé `operator` suivi du symbole de l'opérateur devient le nom de la fonction. La fonction peut être une fonction membre ou une fonction libre (selon l'opérateur).

```cpp
class Fixed {
    int _raw;
public:
    Fixed operator+(const Fixed& other) const;          // membre
    Fixed& operator++();                                 // membre, ++ prefixe
    Fixed  operator++(int);                              // membre, ++ postfixe (parametre int factice)
    bool operator<(const Fixed& other) const;           // membre, comparaison
};

std::ostream& operator<<(std::ostream& os, const Fixed& f);   // libre, insertion de flux
```

C'est tout le principe. Désormais, `a + b`, `a < b`, `++a`, `std::cout << a` fonctionnent tous pour votre type.

---

## 2. La liste complète des opérateurs surchargeables (C++98)

```
   arithmetic:      +    -    *    /    %
   compound assign: +=   -=   *=   /=   %=
   bitwise:         &    |    ^    ~    <<   >>
   compound assign: &=   |=   ^=   <<=  >>=
   logical:         !    &&   ||
   comparison:      ==   !=   <    >    <=   >=
   increment/dec:   ++   --
   special:         =    []   ()   ->   ->*   ,
   memory:          new  new[]  delete  delete[]
   conversion:      operator T()
   address:         &    *
```

Vous **ne pouvez pas** surcharger :

- `.` (accès aux membres — trop spécifique)
- `.*` (accès par pointer-to-member)
- `::` (résolution de portée)
- `?:` (ternaire)
- `sizeof`
- `typeid`
- les éléments du préprocesseur comme `#`, `##`

Vous **ne pouvez pas** inventer de nouveaux opérateurs (`**`, `<>`, etc.). Uniquement ceux qui existent déjà.

---

## 3. Fonction membre vs fonction libre — laquelle choisir ?

```
    membre          quand l'operande de gauche de l'operateur est VOTRE type
                    et que vous voulez un acces privilegie a son etat.

    fonction libre  quand le membre de gauche (LHS) peut ne pas etre votre type
                    (par ex., 2 * MyVec, std::cout << MyType).
```

Ces opérateurs **doivent** être membres :

| Raison | Opérateurs |
|---|---|
| Sémantique d'accès liée à `this` | `=`, `[]`, `()`, `->`, `->*` |

Ces opérateurs **doivent** être des fonctions libres :

| Raison | Opérateurs |
|---|---|
| Symétriques, peuvent nécessiter de convertir le LHS | (Pas d'obligation formelle, mais `<<` / `>>` pour les flux sont pratiquement toujours libres) |

Le reste peut être l'un ou l'autre. En règle générale :

- **L'assignation composée d'abord** (`+=`, `-=`, `*=`, etc.) sous forme de membres.
- **Puis les versions symétriques** (`+`, `-`, `*`) sous forme de fonctions libres définies en termes d'assignation composée.
- **Les opérateurs de flux** sous forme de fonctions libres.

```cpp
class Vec {
    float _x, _y;
public:
    Vec& operator+=(const Vec& other) {
        _x += other._x; _y += other._y;
        return *this;
    }
};

Vec operator+(Vec a, const Vec& b) {     // passage par valeur du LHS, renvoie une copie
    a += b;
    return a;
}
```

Pourquoi passer `Vec a` par valeur ? Vous allez de toute façon le copier pour produire le résultat ; laissez le compiler appliquer la RVO/move.

---

## 4. Patrons d'implémentation par opérateur

### 4.1 `operator=` (assignation par copie) — voir OCF

```cpp
MyClass& MyClass::operator=(const MyClass& other) {
    if (this != &other) {
        // copier chaque membre
        _data = other._data;
    }
    return *this;
}
```

Trois règles :
1. Prendre par `const&`.
2. Garde d'auto-assignation (`if (this != &other)`).
3. Renvoyer `*this` par reference.

### 4.2 `operator==` et `operator!=`

Définir `==` et en dériver `!=` :

```cpp
bool operator==(const Fixed& a, const Fixed& b) { return a.raw() == b.raw(); }
bool operator!=(const Fixed& a, const Fixed& b) { return !(a == b); }
```

En C++20+, vous utiliseriez `<=>` (l'opérateur spaceship), mais ce n'est pas disponible à 42.

### 4.3 `operator<` etc. — définir l'un, dériver les autres

```cpp
bool operator<(const F& a, const F& b)  { return a.raw() <  b.raw(); }
bool operator>(const F& a, const F& b)  { return  b < a; }
bool operator<=(const F& a, const F& b) { return !(b < a); }
bool operator>=(const F& a, const F& b) { return !(a < b); }
```

Il s'agit du modèle de **strict weak ordering**.

### 4.4 `operator++` et `operator--` — pré vs post

```cpp
class C {
public:
    C& operator++();         // prefixe :  ++c → renvoie *this par reference
    C  operator++(int);      // postfixe : c++ → renvoie l'ANCIENNE valeur par valeur
};
```

Le paramètre `int` factice est le seul moyen de lever l'ambiguïté entre préfixe et postfixe au niveau de la syntaxe. Ne lui passez aucune valeur ; il s'agit d'une étiquette (tag).

```cpp
C& C::operator++()        { _v += 1; return *this; }
C  C::operator++(int)     { C tmp = *this; ++(*this); return tmp; }
```

Le postfixe est plus coûteux (il effectue une copie). Préférez le préfixe quand vous n'avez pas besoin de l'ancienne valeur.

### 4.5 `operator[]` — indexation de tableau

```cpp
class Buffer {
    char _data[1024];
public:
    char&       operator[](std::size_t i)       { return _data[i]; }
    const char& operator[](std::size_t i) const { return _data[i]; }
};
```

Deux overloads — une pour l'accès mutable, une pour l'accès en lecture seule. Le compiler choisit la bonne selon que le buffer est `const` ou non.

### 4.6 `operator()` — appel de fonction (foncteur)

```cpp
class Adder {
    int _n;
public:
    Adder(int n) : _n(n) {}
    int operator()(int x) const { return x + _n; }      // un "callable"
};

Adder add5(5);
add5(10);          // 15
```

Les foncteurs sont omniprésents dans la STL — `std::sort(v.begin(), v.end(), Adder(...))` et consorts.

### 4.7 `operator<<` pour la sortie

```cpp
std::ostream& operator<<(std::ostream& os, const Fixed& f) {
    return os << (f.raw() / 256.0f);
}
```

Toujours :
- Prendre `os` par reference (non-const — les flux mutent).
- Prendre votre type par reference const.
- Renvoyer `os` pour permettre le chaînage (`std::cout << a << b << '\n';`).
- Fonction libre, souvent `friend` si vous avez besoin d'un accès privé (ou utilisez un accesseur public).

### 4.8 Opérateur de conversion

```cpp
class Fixed {
public:
    operator float() const { return _raw / 256.0f; }
};

Fixed f(2);
float x = f;       // conversion implicite via operator float
```

En C++98, vous ne pouvez pas marquer les opérateurs de conversion comme `explicit`. Soyez très prudent — ils se déclenchent dans de nombreux contextes et constituent une cause majeure de bugs du type « que vient-il de se passer ? ».

---

## 5. Vision matérielle — identique à n'importe quelle fonction

`operator+` est simplement une fonction. Il n'y a pas de « stub d'opérateur » ni de chemin d'exécution spécial. Après la résolution de surcharge :

```cpp
Vec a, b, c;
c = a + b;
```

devient :

```cpp
c.operator=(operator+(a, b));      // + libre et = membre
```

Est compilé en deux appels de fonction. L'optimiseur peut inliner les deux, particulièrement pour les petits types.

```
   source: c = a + b;

   compiler: which 'operator+' fits (Vec, Vec)? → free fn version.
             which 'operator=' fits Vec on Vec? → member version.

   IR: t = operator+(a, b);
       c.operator=(t);

   asm (after inlining):
   addss  xmm0, xmm1            ; vector add (if compiler vectorized)
   ...
```

Il n'y a aucune distinction à l'exécution entre `a + b` et `operator+(a, b)`. Ce sont des synonymes.

---

## 6. Résolution de surcharge et `operator`

Les règles sont les mêmes que pour n'importe quelle fonction surchargée :

1. Le compiler rassemble toutes les overloads candidates (membres, libres, dans la portée, ADL).
2. Les paramètres de chaque candidate sont confrontés aux types réels des arguments.
3. La meilleure correspondance valide l'emporte. Une égalité entraîne une erreur d'ambiguïté.

C'est pourquoi les déclarations `friend` importent parfois — pour rendre l'opérateur visible au point d'appel.

---

## 7. Le patron d'opérateurs pour la norme 42

Pour une class `Fixed` avec un unique `int _raw` privé :

```cpp
// Fixed.hpp
class Fixed {
    int _raw;

public:
    // OCF
    Fixed();
    Fixed(int);
    Fixed(float);
    Fixed(const Fixed& other);
    Fixed& operator=(const Fixed& other);
    ~Fixed();

    // arithmetique (membres car nous avons besoin de _raw)
    Fixed operator+(const Fixed& other) const;
    Fixed operator-(const Fixed& other) const;
    Fixed operator*(const Fixed& other) const;
    Fixed operator/(const Fixed& other) const;

    // comparaison
    bool operator< (const Fixed& other) const;
    bool operator> (const Fixed& other) const;
    bool operator<=(const Fixed& other) const;
    bool operator>=(const Fixed& other) const;
    bool operator==(const Fixed& other) const;
    bool operator!=(const Fixed& other) const;

    // incrementation / decrementation
    Fixed& operator++();
    Fixed  operator++(int);
    Fixed& operator--();
    Fixed  operator--(int);

    // accesseurs
    int   getRawBits() const;
    void  setRawBits(int raw);
    float toFloat() const;
    int   toInt() const;
};

// insertion de flux (fonction libre)
std::ostream& operator<<(std::ostream& os, const Fixed& f);
```

Il s'agit de la structure canonique de la classe Fixed du CPP02.

---

## 8. Trucs & astuces

### 8.1 N'en abusez pas — ne surchargez que lorsque cela a du sens
`operator+` pour des matrices, des vecteurs, des nombres complexes — oui.
`operator+` pour signifier "concaténer des logs" ou "ajouter un nœud enfant" — non. Utilisez une méthode nommée (`addChild`, `appendLog`).

Le critère : un lecteur qui voit `a + b` saurait-il immédiatement ce que cela fait ? Si non, donnez-lui un nom.

### 8.2 Les opérateurs symétriques doivent commuter (lorsque les mathématiques l'exigent)

Si `Vec(1) + Vec(2) == Vec(2) + Vec(1)`, votre `+` doit refléter cela. Si un opérande non-Vec peut être impliqué, utilisez des fonctions libres et potentiellement deux surcharges non-membres :

```cpp
Vec operator*(float scalar, const Vec& v);
Vec operator*(const Vec& v, float scalar);
```

### 8.3 Ne surchargez pas `&&`, `||`, `,`

Les opérateurs `&&` et `||` natifs court-circuitent. Les versions **surchargées** ne le font pas — les deux arguments sont évalués. C'est surprenant et source d'erreurs. Même chose pour `operator,` (l'opérateur virgule).

### 8.4 La "règle du zéro" — laissez le compiler faire le travail

Si votre class encapsule des types qui implémentent déjà correctement les opérations (comme `std::string`), laissez le constructeur par copie / l'`=` généré par le compiler faire leur travail. N'écrivez pas un `=` dont vous n'avez pas besoin.

### 8.5 Les opérateurs de flux renvoient toujours le flux

```cpp
std::ostream& operator<<(std::ostream& os, const T& t) {
    // ...
    return os;            // obligatoire pour le chaînage
}
```

Tout autre type de retour casse `cout << a << b << '\n';`.

### 8.6 L'opérateur d'appel de fonction est le plus flexible

Un foncteur (`operator()`) peut transporter un état et être passé à des algorithmes. En C++11+, les lambdas les remplacent en grande partie, mais en C++98, les foncteurs sont la façon de faire du "code sous forme de données" générique.

---

## 9. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `operator+ must take exactly N arguments` | Mauvaise arité (par ex., binaire en tant que membre à 1 argument) | Le binaire membre prend 1 paramètre explicite + le `this` implicite ; le binaire libre en prend 2 |
| `no match for operator+` | Surcharge non visible / mauvais types | Vérifiez le namespace, l'ADL, les signatures |
| `operator= must be a non-static member function` | Défini comme une fonction libre | Déplacez-le à l'intérieur de la class |
| `recursive call to operator=` | `*this = other` écrit à l'intérieur de `operator=` | Effectuez des copies membre par membre ; ne vous appelez jamais vous-même |
| Conversions silencieuses surprenantes | Opérateur de conversion non-`explicit` | Rendez les conversions explicites ou supprimez-les |

---

## 10. Résumé visuel

```
                  ┌────────────────────────────────────────────┐
                  │   keyword: operator                         │
                  │                                              │
                  │   names a function that overloads a         │
                  │   built-in symbol. compile-time dispatch    │
                  │   via normal overload resolution.            │
                  └─────────────────────┬──────────────────────┘
                                        │
            ┌───────────────────────────┼───────────────────────────┐
            ▼                           ▼                           ▼
     member operator             free operator              conversion operator
     ───────────────             ─────────────              ──────────────────
     when LHS is your type       when LHS may not be        operator T() — converts
     and you need this access.   yours (streams, scalar     your type to T. fires
     mandatory for: =, [], (),   * vector, etc.) or you     implicitly in many
     ->, ->*                     prefer symmetry.           contexts; use sparingly.

   pattern:
     1. compound-assign (+=, -=) as members → mutate this
     2. binary symmetric (+, -) as free functions in terms of compound
     3. comparison: implement < and ==, derive the rest
     4. << / >> as free functions (often friends)
```

---

## 11. Pratique

1. Pourquoi `operator=` doit-il être un membre non-static ? *(Symétrie/convention ; le standard l'exige. Le LHS est l'objet ; l'opérateur signifie conceptuellement "assigne à moi-même".)*
2. Pourquoi la forme postfixée `operator++(int)` est-elle moins efficace que la forme préfixée `operator++()` ? *(La forme postfixée renvoie l'ancienne valeur, elle doit donc sauvegarder une copie avant de muter.)*
3. Pourquoi `&&`, `||` et `,` sont-ils rarement surchargés ? *(Les surcharger désactive la sémantique de court-circuit / de séquencement, ce qui est surprenant.)*
4. Pourquoi `operator<<` ne peut-il pas être un membre de votre custom class ? *(Son opérande gauche est `std::ostream`. Le `this` d'un opérateur membre serait le flux — et vous ne pouvez pas ajouter de fonctions membres à `std::ostream`.)*