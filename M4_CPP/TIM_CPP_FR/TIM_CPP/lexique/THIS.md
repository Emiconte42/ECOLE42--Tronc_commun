# `this` — Le pointer caché reçu par chaque member function

> **TL;DR.** `this` est un pointer vers l'objet courant, disponible à l'intérieur de chaque non-static member function. Le compiler le passe comme un premier argument invisible. Il est typé `T*` dans les membres normaux, `const T*` dans les membres `const`, et n'existe pas dans les membres `static`.

Voir aussi : [`CLASS.md`](CLASS.md) · [`CONST.md`](CONST.md) · [`OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. Le modèle

Une member function ressemble à une méthode, mais sous le capot, c'est une fonction ordinaire qui prend l'objet par pointer :

```cpp
class Point {
    int _x, _y;
public:
    void translate(int dx, int dy) { _x += dx; _y += dy; }
};

Point p(3, 4);
p.translate(1, 1);
```

Le compiler réécrit concrètement l'appel sous la forme :

```cpp
Point__translate(&p, 1, 1);
```

À l'intérieur de `translate`, `this` est le nom de ce premier paramètre caché :

```cpp
void Point::translate(int dx, int dy) {
    this->_x += dx;        // explicite — identique à ci-dessous
    _y += dy;              // implicite — le compiler insère this->
}
```

```
                caller                          callee
              ┌────────────┐               ┌──────────────────────┐
              │ Point p    │               │ translate(this, dx, dy)
              │  _x = 3    │ ── &p ──►    │   this points at p
              │  _y = 4    │               │   dx = 1, dy = 1
              └────────────┘               │   *this._x += dx
                                            │   *this._y += dy
                                            └──────────────────────┘
```

Après l'appel : `p._x == 4`, `p._y == 5`.

---

## 2. Le type de `this`

À l'intérieur de `class T` :

| Member function déclarée comme… | Type de `this` |
|---|---|
| `void f()`                  | `T *` |
| `void f() const`            | `const T *` |
| `void f() volatile`         | `volatile T *` |
| `void f() const volatile`   | `const volatile T *` |
| `static void f()`           | `this` n'existe pas |

Le `const` final après la liste de paramètres d'une member function s'applique à `*this`, et à rien d'autre.

```cpp
class Box {
    int _value;
public:
    int get() const { return _value; }   // this est const Box*
    void set(int v) { _value = v; }      // this est Box*
};
```

```cpp
const Box b;
b.get();          // OK — get est const, this est const Box*
b.set(1);         // ERREUR — le this de set est Box*, ne peut pas être lié à un const Box
```

---

## 3. Quand `this` est indispensable

### 3.1 Garde d'auto-assignation (self-assignment guard)

```cpp
MyClass& MyClass::operator=(const MyClass& other) {
    if (this != &other) {        // sommes-nous en train d'assigner à nous-mêmes ?
        delete _data;
        _data = new int[other._size];
        std::memcpy(_data, other._data, other._size * sizeof(int));
    }
    return *this;
}
```

Sans cette garde, `a = a;` ferait `delete _data` puis tenterait de copier depuis de la mémoire déjà libérée — undefined behavior.

### 3.2 Retourner `*this` pour le chaînage (chaining)

```cpp
class Builder {
    std::string _s;
public:
    Builder& add(const std::string& part) { _s += part; return *this; }
};

Builder b;
b.add("hello").add(", ").add("world");
```

`*this` déréférence le pointer et renvoie une reference vers l'objet réel. Retourner par reference (pas par valeur !) est indispensable pour le chaînage.

### 3.3 Désambiguïser un membre d'un paramètre

```cpp
class C {
    int x;
public:
    C(int x) : x(x) {}              // OK dans la liste d'initialisation
    void set(int x) {
        x = x;                      // BUG ! les deux désignent le paramètre
        this->x = x;                // OK
    }
};
```

La convention de l'underscore à 42 (`_x` pour les membres) contourne entièrement ce problème :

```cpp
void set(int x) { _x = x; }         // non ambigu
```

### 3.4 Passer l'objet à une free function

```cpp
class Logger {
    void log() const {
        write(std::cout, *this);    // passer soi-même par reference
    }
};
```

---

## 4. Vue hardware — `this` au niveau assembleur

L'ABI x86_64 System V passe le premier argument entier/pointer dans `rdi`. Pour une member function non-static, il s'agit de `this` :

```asm
Point::translate(int, int):
        ; rdi = this (pointer vers Point)
        ; esi = dx
        ; edx = dy
        mov     eax, dword ptr [rdi]        ; charge this->_x
        add     eax, esi                    ; += dx
        mov     dword ptr [rdi], eax        ; réécrit en mémoire
        mov     eax, dword ptr [rdi + 4]    ; charge this->_y
        add     eax, edx                    ; += dy
        mov     dword ptr [rdi + 4], eax    ; réécrit en mémoire
        ret
```

Le pointer n'est qu'un registre. Il n'y a **aucun surcoût** à utiliser des member functions par rapport à des free functions prenant un `T*` en premier argument.

L'ABI x64 de Microsoft fait de même avec `rcx`. L'ABI d'ARM64 utilise `x0`. Le pattern — le premier argument est `this` — est universel.

---

## 5. Accès aux membres via `this` — implicite vs explicite

```cpp
class C {
    int _x;
    int _y;
public:
    int sum() const {
        return _x + _y;             // implicite
        // identique à :
        // return this->_x + this->_y;
    }
};
```

Utilisez `this->` explicitement lorsque :

- Vous devez lever l'ambiguïté avec un paramètre (`this->x = x`).
- Vous voulez faire ressortir l'accès aux membres dans une fonction longue (choix de style).
- Vous êtes dans un template qui hérite d'un autre template, et le dependent name nécessite `this->` pour être résolu (avancé — voir [`TEMPLATES.md`](../notions/advanced/TEMPLATES.md)).

Sinon, omettez-le pour plus de concision.

---

## 6. `this` dans un constructor et destructor

```cpp
class C {
    int _x;
public:
    C() : _x(0) {
        // this pointe vers l'objet partiellement construit
        // l'initialisation des membres a eu lieu (via la liste d'initialisation) ;
        // nous sommes maintenant dans le corps
    }
    ~C() {
        // this pointe vers l'objet encore complètement construit
        // les membres sont vivants ; la destruction commence après le corps
    }
};
```

Un piège subtil : dans un constructor de base class, `this` *est* de type `Base*`, même si l'objet final est un `Derived*`. Appeler une virtual function depuis un constructor ou un destructor fera un dispatch vers la version de **base** — car la partie dérivée n'est pas encore (ou n'est plus) construite. Voir [`POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md).

```cpp
class Animal {
public:
    Animal() { sound(); }                  // appelle Animal::sound, même si 'new Dog()'
    virtual void sound() = 0;
};

class Dog : public Animal {
public:
    void sound() { /* aboie */ }
};

new Dog();      // appelle Animal() → Animal::sound() → undefined behavior (pure virtual)
```

La solution consiste à ne jamais appeler de virtual functions depuis les constructors/destructors.

---

## 7. `*this` renvoie l'objet ; `this` renvoie le pointer

```cpp
class C {
public:
    C&        ref()     { return *this; }   // par reference
    C*        ptr()     { return  this; }   // par pointer
    C         copy()    { return *this; }   // par VALEUR — une copie est effectuée !
};
```

Le troisième est un aimant à bugs subtil : il renvoie une copie, pas l'original. Si vous vouliez « renvoie-moi cet objet », vous voulez presque toujours une reference (`return *this;` et type de retour `C&`).

---

## 8. `this` ne peut pas être réassigné

```cpp
void C::f() {
    this = something;       // ERREUR — this n'est pas une lvalue
}
```

`this` est une **pure rvalue** — une valeur, pas une variable. Vous pouvez l'utiliser (`this->x`, `*this`, `&*this`), mais vous ne pouvez pas modifier ce vers quoi il pointe.

---

## 9. `this` n'existe pas dans les membres static

```cpp
class C {
public:
    static void s() {
        // this ici       — ERREUR : 'this' n'est pas disponible pour les static member functions
    }
};
```

Les static member functions ne prennent pas de `this` implicite. Elles opèrent sur la class, pas sur une instance.

---

## 10. Trucs & astuces

### 10.1 Toujours retourner `*this` (pas `this`) depuis les compound operators

```cpp
class C {
public:
    C& operator+=(int n) { _x += n; return *this; }
};
```

`return this` ne compilerait que si le type de retour était `C*` — et cela casserait l'idiome de chaînage `(a += 1) += 2`.

### 10.2 Le pattern « delete this » — existe, à ne presque jamais utiliser

```cpp
class Refcounted {
    int _refs;
public:
    void release() {
        if (--_refs == 0) delete this;     // légal — mais attention
    }
};
```

Après `delete this;`, la fonction ne doit toucher à aucun membre ni appeler de virtual function. Utilisé dans COM, dans les rouages internes de certains smart pointers — jamais dans les modules de 42.

### 10.3 Ne pas stocker `this` dans une structure de données à longue durée de vie

```cpp
class Logger {
public:
    Logger() { register_logger(this); }  // dangereux si Logger est sur la stack —
                                          // le registre pointe désormais vers un objet bientôt détruit
};

void f() {
    Logger l;
    do_work();    // do_work logue via 'l'
}                  // l détruit — le registre contient maintenant un dangling pointer
```

### 10.4 Comparer `this` aux paramètres

```cpp
operator==(const C& other) const {
    if (this == &other) return true;       // même objet → trivialement égaux
    return _x == other._x && _y == other._y;
}
```

Le test d'identité (`this == &other`) est peu coûteux et évite une comparaison en profondeur.
### 10.5 Dans les templates, `this->` peut être requis

```cpp
template <typename T>
class Base {
protected:
    int _value;
};

template <typename T>
class Derived : public Base<T> {
public:
    int get() {
        return _value;          // ERREUR — _value est un "dependent name"
        return this->_value;    // OK
    }
};
```

C'est l'un des rares cas où un `this->` explicite est obligatoire en C++98. Voir [`TEMPLATES.md`](../notions/advanced/TEMPLATES.md).

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `'this' was not declared in this scope` | Utilisé dans une fonction membre static ou une fonction libre | Passer l'objet explicitement |
| `passing 'const C' as 'this' argument discards qualifiers` | Appel d'une member fn non-const sur un objet const | Marquer la member fn `const` |
| `cannot bind 'C' lvalue to 'C&&' rvalue reference` | Retour par valeur depuis `operator=` (C++11+) | Retourner `*this` par reference, type de retour `C&` |
| Récursion infinie dans `operator=` | `*this = other` écrit à l'intérieur de `operator=` | Effectuer une copie membre par membre, puis retourner `*this` |

---

## 12. Résumé visuel

```
              ┌────────────────────────────────────────────────────┐
              │   p.translate(1, 2);                                │
              │                                                     │
              │      ▼  compiler rewrites as                        │
              │                                                     │
              │   Point::translate(&p, 1, 2);                       │
              │                                                     │
              │           ▲                                          │
              │           │ this                                     │
              │           │                                          │
              │       ┌───┴───┐                                      │
              │       │ Point │                                      │
              │       │ _x _y │ ← regular memory                     │
              │       └───────┘                                      │
              │                                                     │
              │   inside translate:                                 │
              │       this  is a regular pointer (rdi)              │
              │       *this is the object                           │
              │       this->_x is just a member load                │
              └────────────────────────────────────────────────────┘
```

---

## 13. Pratique

1. Quel est le type de `this` à l'intérieur de `void C::f() const` ? *(`const C*`.)*
2. Pourquoi `return *this;` (avec le type de retour `C&`) est-il préféré à `return this;` pour le chaînage ? *(Le chaînage nécessite que les expressions soient des lvalues ; une reference fonctionne, alors qu'un pointer forcerait la syntaxe `(b.add(...))->add(...)` partout.)*
3. Pourquoi appeler une fonction virtual depuis un constructor est-il problématique ? *(`this` a le type statique de la class du constructor, donc la résolution des fonctions virtuals se fait vers la classe de base, pas vers la classe dérivée. Potentiellement un UB s'il s'agit d'une pure virtual.)*
4. Peut-on appeler une fonction membre static via `this->` ? *(Oui — `this->staticMember()` est valide. `Class::staticMember()` est toutefois préféré par souci de clarté.)*