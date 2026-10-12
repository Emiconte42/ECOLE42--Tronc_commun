# `friend` — Une trappe à travers le contrôle d'accès

> **TL;DR.** Les déclarations `friend` accordent à une fonction ou une class spécifique l'accès à vos membres `private` et `protected`. Ce n'est **pas transitif**, **pas hérité**, et **pas symétrique**. À utiliser avec parcimonie — typiquement pour les overloads d'opérateurs non-membres.

Associé : [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) · [`OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`CLASS.md`](CLASS.md)

---

## 1. La motivation — `operator<<`

```cpp
class Account {
    int _balance;
public:
    Account(int b) : _balance(b) {}
};

std::ostream& operator<<(std::ostream& os, const Account& a) {
    os << "Balance: " << a._balance;        // ERREUR — _balance est private
    return os;
}
```

`operator<<` ne peut pas être une fonction membre de `Account` (l'opérande de gauche est `std::ostream`, pas `Account`). Il doit s'agir d'une fonction libre. Mais les fonctions libres ne peuvent pas voir `_balance`.

La solution :

```cpp
class Account {
    int _balance;
    friend std::ostream& operator<<(std::ostream&, const Account&);
public:
    Account(int b) : _balance(b) {}
};

std::ostream& operator<<(std::ostream& os, const Account& a) {
    return os << "Balance: " << a._balance;     // désormais légal
}
```

`friend` signifie : « cette fonction peut nommer mes membres private ». Rien de plus, rien de moins.

---

## 2. Trois formes

### 2.1 Fonction amie — déclarée à l'intérieur de la class

```cpp
class Account {
    int _balance;
    friend std::ostream& operator<<(std::ostream&, const Account&);
};
```

La fonction n'est **pas** un membre de `Account`. C'est une fonction libre avec la permission de regarder à l'intérieur.

### 2.2 Class amie

```cpp
class Account {
    int _balance;
    friend class Bank;       // chaque fonction membre de Bank peut accéder aux éléments private de Account
};

class Bank {
public:
    void audit(const Account& a) {
        std::cout << a._balance;     // OK
    }
};
```

C'est une solution plus lourde — *chaque* méthode de `Bank` obtient un accès complet. Utilisez-la lorsque deux classes sont étroitement couplées (typiquement une class et son itérateur).

### 2.3 Fonction membre amie

```cpp
class Account;                      // forward decl

class Bank {
public:
    void audit(const Account& a);
};

class Account {
    int _balance;
    friend void Bank::audit(const Account&);    // uniquement cette méthode spécifique de Bank
};
```

L'option la plus chirurgicale. Accorde l'accès à une méthode spécifique d'une class spécifique.

---

## 3. Les quatre faits concernant `friend`

```
              ┌─────────────────────────────────────────────┐
              │ 1. N'est PAS un membre.                     │
              │    Une fonction friend n'est pas un membre  │
              │    de la class qui l'accorde. Pas de        │
              │    pointer 'this'.                          │
              ├─────────────────────────────────────────────┤
              │ 2. N'est PAS transitif.                     │
              │    L'ami B de A ne devient pas un ami       │
              │    des autres amis de B.                    │
              ├─────────────────────────────────────────────┤
              │ 3. N'est PAS symétrique.                    │
              │    A dit "B est mon ami" — c'est à sens     │
              │    unique. B ne peut pas lire les éléments  │
              │    private de A sauf si A le stipule.       │
              ├─────────────────────────────────────────────┤
              │ 4. N'est PAS hérité.                        │
              │    Les sous-classes de A n'héritent pas     │
              │    des amitiés de A (et vice versa).        │
              └─────────────────────────────────────────────┘
```

Ces règles existent parce que l'amitié est censée être une exception délibérée et restreinte — pas une porte dérobée pour un accès général.

---

## 4. Vision hardware/compiler

`friend` est purement un **mécanisme du type-system**, tout comme `private`. Le compiler :

- Enregistre l'amitié au moment du parsing.
- Ignore les vérifications d'accès lorsqu'un friend nomme un membre private/protected.
- Ne génère **aucun code supplémentaire, aucune métadonnée supplémentaire, aucun coût à l'exécution**.

```
   class Account {
     int _balance;
     friend f();
   };
   void f(Account &a) { a._balance = 100; }

   compiler:
     "f nomme a._balance. Est-ce que f est un membre ? non.
      Est-ce que _balance est accessible pour f ? oui — f est un friend.
      Autoriser."
   
   machine code émis : identique à un accès de membre public.
```

---

## 5. Les deux usages corrects

### 5.1 Overloads d'opérateurs non-membres

Le cas classique :

```cpp
class Vec3 {
    float _x, _y, _z;
public:
    Vec3(float x, float y, float z) : _x(x), _y(y), _z(z) {}

    friend Vec3 operator+(const Vec3& a, const Vec3& b);
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
};

Vec3 operator+(const Vec3& a, const Vec3& b) {
    return Vec3(a._x + b._x, a._y + b._y, a._z + b._z);
}

std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << '(' << v._x << ", " << v._y << ", " << v._z << ')';
}
```

Pourquoi une fonction libre ? Pour que `2.0f * vec` puisse fonctionner — l'opérande de gauche doit être celui qui dispose d'une conversion. Les opérateurs membres ne peuvent pas être flexibles quant à leur opérande de gauche.

### 5.2 Types étroitement couplés — le pattern iterator

```cpp
class List {
    struct Node { int value; Node *next; };
    Node *_head;
    friend class Iterator;
public:
    class Iterator {
        Node *_node;
    public:
        Iterator(Node *n) : _node(n) {}
        int value() const { return _node->value; }
        Iterator& operator++() { _node = _node->next; return *this; }
    };

    Iterator begin() { return Iterator(_head); }
};
```

`Iterator` doit parcourir des `Node*` — qui sont private pour `List`. Rendre `Iterator` ami (ou, de manière équivalente, une class imbriquée — les types imbriqués ont automatiquement accès aux membres private englobants) est la norme.

---

## 6. Les mauvais usages (à éviter)

### 6.1 « Je veux juste que cette fonction accède aux membres privates »

Si une fonction libre a régulièrement besoin de voir l'état private, demandez-vous si elle ne devrait pas être une fonction membre. `friend` est destiné aux cas où elle *ne peut pas* l'être (opérateurs du mauvais côté, couplage de deux classes) — pas parce qu'écrire un getter semble être du travail de frappe supplémentaire.

### 6.2 « Rendre friend tout mon namespace »

Vous ne pouvez pas, et vous ne devriez pas le vouloir. L'amitié se définit par nom.

### 6.3 Friending de masse

```cpp
class A {
    friend class B;
    friend class C;
    friend class D;        // mauvaise odeur de code (smell)
    friend class E;
    friend class F;
};
```

Si cinq classes ont besoin des éléments internes, votre encapsulation est mauvaise. Refactorisez — une interface partagée, une vue publique en lecture seule, etc.

---

## 7. Forward declarations et friends

Si vous déclarez une class amie, la class qui accorde l'amitié n'a pas besoin de connaître sa définition complète :

```cpp
class Bank;                          // forward decl suffit

class Account {
    int _balance;
    friend class Bank;
};
```

Si vous déclarez une fonction membre spécifique en tant qu'amie, vous avez d'abord besoin de la définition complète de la class de ce membre :

```cpp
class Bank;                          // forward decl insuffisante
void Bank::audit(const Account&);    // a besoin de la définition de Bank
```

En pratique, déclarez les classes dans l'ordre des dépendances ou utilisez les forward declarations avec précaution.

---

## 8. `friend` dans un template

```cpp
template <typename T>
class Box {
    T _value;
    friend std::ostream& operator<<(std::ostream& os, const Box<T>& b) {
        return os << b._value;       // défini inline, ne nécessite pas de forward decl séparée
    }
};
```

C'est un pattern courant : définir le `operator<<` ami *à l'intérieur* du corps du template. Chaque instanciation de template obtient son propre friend.

Si vous souhaitez déclarer le friend séparément, vous aurez besoin de mécanismes de templates qui deviennent un peu complexes — pour les modules de 42, la forme inline ci-dessus convient parfaitement.

---

## 9. Astuces et conseils

### 9.1 Privilégier le non-friend par défaut

Chaque fois que vous envisagez d'utiliser `friend`, demandez-vous : « cela pourrait-il être un membre public à la place ? » Souvent, oui.

### 9.2 Les déclarations friend ne se soucient pas des labels d'accès

```cpp
class A {
private:
    friend class B;            // fonctionne
public:
    friend class C;            // fonctionne aussi — même effet
};
```

Le compiler traite les déclarations `friend` à l'identique, quelle que soit la section dans laquelle elles se trouvent. La convention est de les placer au sommet de la définition de la class ou juste à côté des membres concernés, selon le style adopté.

### 9.3 L'amitié n'est pas héritée — dans les deux sens

```cpp
class Base { friend class Buddy; };
class Derived : public Base {};

class Buddy {
    void f(Derived &d) {
        // peut accéder aux éléments private de Base hérités par Derived (puisque Buddy est ami de Base)
        // NE PEUT PAS accéder aux éléments private propres à Derived — Derived n'a pas accordé d'amitié
    }
};
```

Et :

```cpp
class A { int x; friend class B; };
class B {};
class C : public B {};               // C n'est PAS un ami de A
```

### 9.4 Pattern de stream operator à 42

```cpp
// Account.hpp
class Account {
    int _balance;
public:
    Account(int b);

    int  getBalance() const;       // getter public
};

std::ostream& operator<<(std::ostream& os, const Account& a);  // décl de fonction libre
```

```cpp
// Account.cpp
std::ostream& operator<<(std::ostream& os, const Account& a) {
    return os << "Balance: " << a.getBalance();    // utilise le getter public — aucun friend nécessaire
}
```
Si vous avez un getter public, vous n'avez pas besoin de `friend`. Certains évaluateurs de 42 préfèrent cela — sans amitié, avec l'operator utilisant l'interface publique.

Si vous n'avez pas de getter (et n'en voulez pas), utilisez `friend`.

### 9.5 Les fonctions friend définies à l'intérieur de la class restent des free functions

```cpp
class Box {
    int _v;
public:
    friend bool operator==(const Box& a, const Box& b) {     // corps inline
        return a._v == b._v;
    }
};
```

`operator==` n'est **pas** un member de `Box` même s'il est défini ici. Il est uniquement **trouvé** via l'argument-dependent lookup lorsqu'au moins un opérande est une `Box`. C'est tout à fait correct et idiomatique.

---

## 10. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'_x' is private` depuis l'intérieur d'`operator<<` | Déclaration de friend manquante ou mal orthographiée | Ajouter la déclaration `friend` dans la class |
| `friend declaration ... not a member of any class` | Écriture de `friend void f();` en dehors d'une class | L'amitié ne peut être accordée que depuis l'intérieur d'une définition de class |
| L'héritage n'accorde pas le statut de friend | Tentative d'accéder aux privates de la base depuis une sous-classe qui pensait hériter de l'amitié | L'amitié n'est pas héritée ; refactoriser |
| Linker error sur une fonction friend | Le friend a été déclaré mais jamais défini | Fournir la définition dans un `.cpp` (ou inline dans la class) |

---

## 11. Visual summary

```
                  ┌────────────────────────────────────────┐
                  │     class Granter {                     │
                  │         int _secret;                    │
                  │                                         │
                  │         friend class Receiver;          │  ─── permission
                  │         friend void f(...);             │
                  │     };                                  │
                  └─────────────────────┬───────────────────┘
                                        │
                                        ▼
                  ┌────────────────────────────────────────┐
                  │     class Receiver {                    │
                  │         void f(Granter& g) {            │
                  │             g._secret = 42;             │  ✓ allowed
                  │         }                               │
                  │     };                                  │
                  └────────────────────────────────────────┘

                  ✗ NOT inherited      ✗ NOT transitive
                  ✗ NOT symmetric     ✗ NOT a member
```

---

## 12. Practice

1. Pourquoi `operator<<` pour une class personnalisée doit-il être une free function (généralement un friend) ? *(Son opérande de gauche est `std::ostream` ; la fonction ne peut pas être un member de la class de l'utilisateur car `*this` serait un stream.)*
2. Si `class A { friend class B; }; class A2 : public A {};` — est-ce que `A2` accorde l'amitié à `B` ? *(Non — l'amitié n'est pas héritée.)*
3. Si `friend bool operator==(const Box&, const Box&) { ... }` est défini à l'intérieur de `Box`, est-il un member de `Box` ? *(Non — c'est une free function. Simplement définie à l'intérieur du corps de Box pour des raisons de commodité.)*
4. Quand l'utilisation d'un getter public pourrait-elle éliminer le besoin de `friend` ? *(Dès lors que la fonction friend n'a besoin que d'un accès en lecture. Getter public + free function = pas d'amitié nécessaire.)*