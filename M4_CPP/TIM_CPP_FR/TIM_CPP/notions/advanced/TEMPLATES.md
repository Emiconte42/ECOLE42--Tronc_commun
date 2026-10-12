# Templates — Guide complet

> **Zoom sur les mots-clés :** [`TEMPLATE`](../../lexique/TEMPLATE.md) · [`TYPENAME`](../../lexique/TYPENAME.md)

Les templates sont la réponse de C++ à : « comment écrire du code qui fonctionne pour n'importe quel type, sans renoncer au type-safety ? »

```cpp
template <typename T>
T max(T a, T b) { return a > b ? a : b; }

max(3, 5);                   // T = int         → 5
max(3.14, 2.71);             // T = double      → 3.14
max(std::string("a"), std::string("b"));    // T = std::string → "b"
```

Une seule source ; trois vraies fonctions dans le binaire final, chacune générée par le compiler au point d'appel.

---

## Table des matières

1. [Pourquoi les templates existent](#1-pourquoi-les-templates-existent)
2. [Function templates](#2-function-templates)
3. [Class templates](#3-class-templates)
4. [Arguments de template — trois variantes](#4-arguments-de-template--trois-variantes)
5. [`typename` vs `class`](#5-typename-vs-class)
6. [Déduction des arguments de template](#6-déduction-des-arguments-de-template)
7. [Spécialisation](#7-spécialisation)
8. [Pourquoi les templates résident dans les headers](#8-pourquoi-les-templates-résident-dans-les-headers)
9. [Member function templates](#9-member-function-templates)
10. [Templates et héritage](#10-templates-et-héritage)
11. [Dans le CPP07 — Whatever & Array](#11-dans-le-cpp07--whatever--array)
12. [Templates vs la philosophie STL](#12-templates-vs-la-philosophie-stl)
13. [Règles empiriques](#13-règles-empiriques)
14. [Pièges courants](#14-pièges-courants)

---

## 1. Pourquoi les templates existent

Sans templates, « écrire une fonction qui marche pour n'importe quel type » offre trois mauvaises options en C :

```c
// 1. Dupliquer pour chaque type — enfer de maintenance
int    max_int(int a, int b);
double max_dbl(double a, double b);

// 2. Utiliser void* et size_t — perte du type safety, nécessite un dispatch au runtime
void max_generic(void* a, void* b, void* out, size_t sz, int (*cmp)(void*,void*));

// 3. Macro — aucune vérification de type, horreur avec les expressions
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

Les templates vous apportent la rapidité et la sécurité de l'option 1 avec la flexibilité de l'option 3, le tout *entièrement type-checked au compile time.* Le compiler génère une fonction sur mesure pour chaque type utilisé — ainsi, `max<int>` est aussi rapide qu'un `max_int` écrit à la main.

---

## 2. Function templates

### Déclaration

```cpp
template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}
```

- `template <typename T>` — « ce qui suit est paramétré par un type nommé `T`. »
- À l'intérieur de la fonction, `T` remplace le type que l'appelant choisit.

### Appel

Habituellement, le compiler **déduit** `T` à partir des arguments — vous n'avez pas besoin de l'expliciter :

```cpp
max(3, 5);           // int
max(3.14, 2.71);     // double
max<int>(3, 5);      // explicite — également légal, rarement nécessaire
```

### Paramètres de template multiples

```cpp
template <typename T, typename U>
void iter(T* array, size_t len, U fn) {
    for (size_t i = 0; i < len; ++i)
        fn(array[i]);
}
```

`U` peut être un function pointer, un foncteur (class avec `operator()`), ou n'importe quoi d'autre. C'est l'exercice CPP07 ex01.

---

## 3. Class templates

Une **class entière** paramétrée par un type :

```cpp
template <typename T>
class Array {
    T*     _data;
    size_t _size;
public:
    Array() : _data(NULL), _size(0) {}
    Array(size_t n) : _data(new T[n]()), _size(n) {}
    ~Array() { delete[] _data; }

    T&       operator[](size_t i)       { return _data[i]; }
    const T& operator[](size_t i) const { return _data[i]; }
    size_t   size() const               { return _size; }
};

Array<int>           ints(10);
Array<std::string>   names(5);
Array<Dog*>          dogs(3);
```

`Array<int>` et `Array<std::string>` sont des *classes complètement distinctes* — deux ensembles complets de méthodes compilés indépendamment.

### Détail syntaxique clé

Lorsque vous mentionnez la classe par son nom **en dehors de sa propre définition**, vous incluez les paramètres de template :

```cpp
template <typename T>
Array<T>::Array(size_t n) : _data(new T[n]()), _size(n) {}
//    ^^^
//    PAS juste "Array"
```

---

## 4. Arguments de template — trois variantes

Les paramètres de template peuvent être :

### 1. Paramètres de type — le cas classique

```cpp
template <typename T>    class Vec;
template <class T>       class Vec;    // synonyme — voir §5
```

### 2. Paramètres non-type — une *valeur* connue au compile time

Entiers, pointers vers des variables globales, enums :

```cpp
template <typename T, size_t N>
class FixedArray {
    T _data[N];
public:
    size_t size() const { return N; }
};

FixedArray<int, 10> arr;        // N = 10 est gravé dans le type
FixedArray<int, 20> arr2;       // Type DIFFÉRENT de arr
```

La valeur doit être une constante connue au compile time. Passer une variable ne fonctionnera pas.

### 3. Paramètres template template — un *template* comme paramètre

Rare ; utilisé par du code de conteneurs sophistiqué :

```cpp
template <typename T, template <typename> class Container>
class Stack {
    Container<T> _data;
};

Stack<int, std::vector> s;      // Container = std::vector (en tant que template lui-même)
```

Vous n'y toucherez pas à 42. Ils ne sont mentionnés que pour que vous puissiez les reconnaître dans la nature.

---

## 5. `typename` vs `class`

Dans une liste de paramètres de template, `typename` et `class` sont des **synonymes** :

```cpp
template <typename T> void f(T);
template <class    T> void f(T);        // identique
```

Convention : utilisez `typename` pour « n'importe quel type », `class` quand vous savez que ce sera un type de classe. Les deux sont corrects ; `typename` est plus clair à la lecture lorsque `T` peut être un `int` ou un `double`.

Ailleurs, `typename` a un autre rôle : lever l'ambiguïté sur les **dependent types** à l'intérieur d'un template :

```cpp
template <typename T>
void f() {
    typename T::iterator it;    // "T::iterator EST un type" — indique au compiler
}
```

Sans `typename`, le compiler ne peut pas savoir si `T::iterator` est un type ou une valeur (il ne sait pas encore ce qu'est `T`). C'est une particularité de C++98 que vous croiserez dans le code gravitant autour de la STL.

---

## 6. Déduction des arguments de template

Le compiler déduit `T` à partir de l'appel :

```cpp
template <typename T> void f(T x);

f(42);             // T = int
f(3.14);           // T = double
f("hello");        // T = const char*
```

Il fait correspondre les types des arguments avec le motif du paramètre. Si le motif est `T`, `T` prend le type de l'argument. Si c'est `T&` ou `const T&`, le compiler retire la reference pour les besoins de la déduction.

### Quand la déduction échoue

- Les types de retour ne sont **pas déductibles** depuis l'appel — uniquement à partir des arguments :
  ```cpp
  template <typename T> T zero();
  int n = zero<int>();            // T doit être spécifié explicitement
  ```
- Déductions contradictoires :
  ```cpp
  template <typename T> void f(T a, T b);
  f(3, 3.14);                     // ERREUR — T ne peut pas être à la fois int et double
  ```

---

## 7. Spécialisation

Parfois, la solution universelle ne convient pas. Vous voulez une implémentation personnalisée pour un type spécifique.

### Spécialisation totale (full specialization)

```cpp
template <typename T>
void print(T x) { std::cout << x << '\n'; }

// Spécialisation pour bool
template <>
void print<bool>(bool x) { std::cout << (x ? "true" : "false") << '\n'; }

print(42);        // "42"
print(true);      // "true"  — version spécialisée
```

Le `template <>` signifie « aucun nouveau paramètre de template — il s'agit d'une version concrète pour `bool` ».

### Spécialisation partielle (classes uniquement)

Vous pouvez spécialiser une partie de la liste des paramètres :

```cpp
template <typename T>          class Box;                 // primaire
template <typename T>          class Box<T*> { /* ... */ };   // pour tout type pointer
template <typename T, size_t N> class Box<T[N]>;          // pour les tableaux

Box<int>    b1;       // primaire
Box<int*>   b2;       // spécialisation pour pointer
```

**Les function templates ne peuvent pas être partiellement spécialisés** — vous les surchargez à la place.

---

## 8. Pourquoi les templates résident dans les headers

Lorsque vous instanciez `max<int>` dans `main.cpp`, le compiler a besoin du **code source complet de `max`** directement sous les yeux pour générer la version int. Si le corps de `max` se trouvait dans `utils.cpp`, le compiler compilant `main.cpp` ne pourrait pas le voir.

```
              PAS CECI :                       CECI :

utils.hpp:    template<typename T>            template<typename T>
              T max(T a, T b);                T max(T a, T b) { return ... }
utils.cpp:    template<typename T>
              T max(T a, T b) { return ... }  (rien — le header a tout)

main.cpp:     utilise max<int>                 utilise max<int>
              ↓                                ↓
              le compiler ne voit pas le corps le compiler voit le corps — génère la version int
              → erreur de link                 → fonctionne
```

**Conventions :**

1. Tout mettre dans le `.hpp` / `.h`. Simple.
2. Mettre les déclarations dans le `.hpp`, les définitions dans un fichier séparé `.tpp` (ou `.ipp`) inclus tout en bas du `.hpp`. C'est plus propre :
   ```cpp
   // Array.hpp
   template <typename T>
   class Array { /* decls */ };

   #include "Array.tpp"   // tout en bas
   ```

Les deux fonctionnent. À 42, choisissez celle que le sujet ou vos préférences préconisent ; beaucoup d'étudiants adoptent l'option 2 une fois qu'ils l'ont découverte.

---

## 9. Member function templates

Vous pouvez placer un template sur une simple méthode d'une classe (template ou non-template) :
```cpp
class Logger {
public:
    template <typename T>
    void log(const T& value) {
        std::cout << "[log] " << value << '\n';
    }
};

Logger l;
l.log(42);
l.log("hello");
l.log(std::string("oui"));
```

Chaque appel génère une nouvelle instanciation. La class elle-même n'est pas templated ; seule la méthode l'est.

Dans une class templated :

```cpp
template <typename T>
class Array {
    T* _data;
public:
    template <typename U>        // template interne
    U getAs(size_t i) const { return static_cast<U>(_data[i]); }
};

Array<int> a(5);
double d = a.getAs<double>(0);
```

---

## 10. Templates et héritage

Les templates peuvent servir de base classes :

```cpp
template <typename T>
class Container {
public:
    virtual ~Container() {}
    virtual void add(const T&) = 0;
};

class IntList : public Container<int> {
public:
    void add(const int& v) { /* ... */ }
};
```

Et vous pouvez templatiser la classe dérivée :

```cpp
template <typename T>
class MyList : public Container<T> {
public:
    void add(const T& v) { /* ... */ }
};
```

Attention : **lorsque vous héritez d'une base dépendante, vous devez qualifier explicitement les membres de la base.**

```cpp
template <typename T>
class Derived : public Base<T> {
public:
    void f() {
        // method();             // ERREUR — le compiler ne sait pas qu'elle existe
        this->method();          // OK — retarde la résolution au moment de l'instanciation
        Base<T>::method();       // également OK
    }
};
```

Il s'agit de la règle de "two-phase name lookup" du C++98. Rarement nécessaire mais surprenante.

---

## 11. Dans CPP07 — Whatever & Array

Le CPP07 vous guide à travers les templates, du plus simple au plus utile.

### ex00 — `whatever.hpp` — function templates

Trois templates de fonctions libres :
```cpp
template <typename T> void   swap(T& a, T& b);
template <typename T> T      min(const T& a, const T& b);
template <typename T> T      max(const T& a, const T& b);
```
Ceux-ci doivent fonctionner pour tout type prenant en charge `<` / `>` et l'assignation. Le `const T&` sur `min` et `max` est important — il évite de copier des types coûteux.

### ex01 — `iter.hpp` — template de fonction d'ordre supérieur

```cpp
template <typename T, typename F>
void iter(T* array, size_t len, F fn) {
    for (size_t i = 0; i < len; ++i)
        fn(array[i]);
}
```

`F` peut être un function pointer, un foncteur, ou — si vous êtes passé au C++11 — une lambda. Le point clé : les templates ne se limitent pas à des paramètres de type décrivant des *données* ; ils décrivent aussi des *opérations*.

### ex02 — `Array.hpp` — class template

Construire un conteneur générique qui :
- Est default-constructed avec `size() == 0`.
- Prend un `unsigned int n` pour créer un array de `n` instances de `T` default-constructed.
- Possède un copy constructor et un `operator=` fonctionnels (deep-copy — OCF !).
- Dispose d'un `operator[]` avec **bounds checking** qui lance une `std::exception` (ou une sous-classe).
- Fournit `size()`.

C'est ici que vous assemblez : templates, OCF, exceptions, et le bon usage de `new T[n]()` (value-initialization, pour que les ints soient à zéro) par rapport à `new T[n]` (default-init uniquement).

---

## 12. Templates vs l'esprit STL

L'ensemble de la STL est construit sur les templates. Dès lors que vous comprenez les templates, la STL cesse de paraître magique :

- `std::vector<T>` est *littéralement* un class template que vous auriez pu écrire.
- `std::sort(begin, end, cmp)` est un function template qui accepte n'importe quel iterator et n'importe quel comparateur.
- Les iterators fonctionnent parce que les templates ne "voient" jamais le type réel de l'iterator — ils appellent simplement `++it`, `*it`, `!=`, et quel que soit ce que le type fournit, le compiler le lie lors de l'instanciation.

Ce style s'appelle le **duck typing à la compilation** : si `T` prend en charge les opérations utilisées par le template, cela fonctionne. Aucun héritage requis. Aucune interface à implémenter.

Consultez [STL.md](STL.md) une fois que vous aurez conçu un class template vous-même.

---

## 13. Règles empiriques

- **Commencez par le template primaire. Ne spécialisez que si nécessaire.**
- **Utilisez `const T&` pour les paramètres que vous ne modifiez pas** — évite des copies coûteuses quand `T` est lourd.
- **Initialisez par défaut avec `T()`** lorsque vous avez besoin d'un "zéro" d'un type inconnu. Fonctionne pour tous les types intégrés (built-in) et toute class munie d'un default constructor.
- **Les membres d'un template doivent être visibles à l'instanciation** — c'est pourquoi ils se placent dans des headers ou des fichiers `.tpp`.
- **Préférez les function templates à `void*` + function pointers.** À chaque fois.
- **Si une solution non-template fait l'affaire, utilisez une solution non-template.** Les templates sont plus lents à compiler et produisent des binaires plus volumineux.

---

## 14. Pièges à éviter

- **Définitions dans des fichiers .cpp → erreurs de linker.** Si le compiler ne peut pas voir le corps au point d'appel, il ne peut pas instancier. Déplacez-le dans le header (ou `.tpp`).
- **`typename T::something` est obligatoire** quand `something` peut être un type à l'intérieur d'un paramètre de template. Sinon, le compiler suppose qu'il s'agit d'une valeur.
- **Two-phase lookup** : les noms qui dépendent de `T` ne sont pas résolus avant l'instanciation. `this->method()` ou `Base<T>::method()` permet d'indiquer au compiler "fais-moi confiance, il sera là".
- **Les messages d'erreur des templates sont réputés pour être affreux.** Une petite erreur peut générer des centaines de lignes. Lisez la *première* erreur — tout le reste n'est qu'une conséquence.
- **La déduction des arguments de template ignore les conversions.** `max(3, 3.14)` échoue car `T` ne peut pas être à la fois `int` et `double` ; spécifier explicitement `max<double>(3, 3.14)` fonctionne.
- **`new T[n]` vs `new T[n]()`** — les parenthèses comptent. `new int[5]` produit des ints non initialisés ; `new int[5]()` initialise à zéro. Pour les types de class avec des constructeurs écrits par l'utilisateur, ils sont équivalents. Ce piège se manifeste dans CPP07 ex02.
- **La full specialization des member function templates doit se faire hors de la class.** La full specialization in-class n'est pas autorisée en C++98.
- **Code bloat** : chaque instanciation distincte est une fonction compilée séparée. `Array<int>` et `Array<long>` sur un système 64-bit semblent identiques mais vous coûtent le double en taille de segment text.