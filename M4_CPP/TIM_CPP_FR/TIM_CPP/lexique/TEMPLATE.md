# `template` — Du code qui écrit du code

> **TL;DR.** Un template est une **recette** pour une fonction ou une class. Le compiler génère une version concrète (une *instantiation*) par type ou valeur que le programme utilise réellement. Le mot-clé `template` introduit une liste de paramètres de types ou de constantes sur lesquels la recette est paramétrée.

Voir aussi : [`TYPENAME.md`](TYPENAME.md) · [`TEMPLATES.md`](../notions/advanced/TEMPLATES.md) · [`STL.md`](../notions/advanced/STL.md)

---

## 1. La forme

```cpp
template <typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}
```

`template <typename T>` signifie : *« cette fonction a un paramètre de type nommé `T` ; veuillez l'instancier au besoin. »*

Utilisation :

```cpp
max<int>(3, 5);          // explicite :  T = int
max(3, 5);               // déduit :     T = int
max(3.14, 2.71);         // déduit :     T = double
max(std::string("a"), std::string("b"));  // T = std::string
```

Le compiler **émet un corps de fonction distinct** pour chaque `T` réellement utilisé. Appeler `max<int>` et `max<double>` produit deux symboles de fonction distincts dans le binaire.

```
   source:                              binary:
   template <typename T> max(T,T)       max<int>(int,int)
                                        max<double>(double,double)
                                        max<std::string>(...)   ← only the ones called
```

C'est la **monomorphisation** — source générique, code spécifique.

---

## 2. Deux sortes de paramètres de template

### 2.1 Paramètre de type

```cpp
template <typename T>      // 'typename' ou 'class' — même chose pour les paramètres de type
template <class T>         // identique
```

`T` est un placeholder pour n'importe quel type fourni par l'appelant.

### 2.2 Paramètre non-type (une valeur, doit être une constante à la compilation)

```cpp
template <int N>
class FixedArray {
    int _data[N];
};

FixedArray<10> a;          // N = 10
FixedArray<20> b;          // N = 20 — type DIFFÉRENT de FixedArray<10>
```

Types de non-type autorisés en C++98 : types intégraux (int, char, etc.), enums, pointers/references vers des objets à liaison externe (external linkage), pointers vers des membres. **Pas** de floats ni d'objets arbitraires.

### 2.3 Combiner les deux

```cpp
template <typename T, int N>
class Buffer {
    T _data[N];
};

Buffer<int, 100>    bi;
Buffer<double, 50>  bd;
```

---

## 3. Function templates

```cpp
template <typename T>
void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}
```

Trois choses se produisent lorsque vous appelez `swap(x, y)` :

```
   1. argument deduction:  T = decltype(x) (with array-to-pointer, etc., decay rules)
   2. instantiation:       compiler emits swap<T>'s body
   3. type checking:       does T support copy and assignment? if not → compile error
```

Une erreur de compilation provenant d'une instantiation de template ressemble à :

```
   error: no match for 'operator>' (operand types are 'X' and 'X')
   note:  in instantiation of 'T max(T, T) [with T = X]'
```

Lisez-les depuis le **bas** — trouvez le code utilisateur qui a déclenché l'instantiation, puis remontez pour voir ce que le compiler attendait réellement.

### 3.1 Instantiation explicite

```cpp
max<int>(3.14, 2.71);    // force T = int ; l'argument 3.14 est converti en int (tronqué à 3)
```

Utilisez ceci lorsque la déduction choisirait un mauvais type, ou lorsque les arguments diffèrent.

### 3.2 Astuces de déduction

```cpp
template <typename T>
void f(T value);

f(42);          // T = int
f("hi");        // T = const char*
f<int>("hi");   // T = int explicitement ; "hi" ne se convertit pas → ERROR

template <typename T>
void g(T &ref);

int x;
g(x);           // T = int ; ref est int&
const int y = 5;
g(y);           // T = const int ; ref est const int&
```

La déduction de reference préserve les qualificateurs cv ; la déduction par valeur retire le `const` de premier niveau (top-level).

---

## 4. Class templates

```cpp
template <typename T>
class Stack {
    T   _data[100];
    int _top;
public:
    Stack() : _top(0) {}
    void push(const T& x) { _data[_top++] = x; }
    T    pop()            { return _data[--_top]; }
    bool empty() const    { return _top == 0; }
};

Stack<int>          si;
Stack<std::string>  ss;
```

`Stack<int>` et `Stack<std::string>` sont des types entièrement distincts. De même pour toute différence dans les arguments de template.

### 4.1 Les member functions sont aussi des templates

Lorsque vous les définissez en dehors de la class, vous devez répéter la clause `template` :

```cpp
template <typename T>
void Stack<T>::push(const T& x) {
    _data[_top++] = x;
}
```

`Stack<T>::` rattache le membre au template ; le `template <typename T>` initial réintroduit `T` pour ce scope.

### 4.2 Header-only

Les templates sont **presque toujours entièrement définis dans des headers**. La raison : l'instantiation a lieu dans l'unité de traduction (TU) qui *utilise* le template, et celle-ci a besoin du corps complet, pas seulement d'une déclaration. Placer le corps dans un `.cpp` fonctionne pour une seule TU mais échoue pour les autres.

Pattern de la norme 42 :

```cpp
// Stack.hpp
template <typename T>
class Stack {
public:
    void push(const T& x);
};

template <typename T>
void Stack<T>::push(const T& x) {
    // ...
}

// (pas de Stack.cpp)
```

Ou définissez tout à l'intérieur du corps de la class. Les deux approches conviennent.

---

## 5. Spécialisation

Vous pouvez écrire une implémentation différente pour un type spécifique :

```cpp
template <typename T>
struct Printer {
    static void print(const T& x) { std::cout << x; }
};

template <>                                  // spécialisation explicite
struct Printer<bool> {
    static void print(const bool& x) {
        std::cout << (x ? "true" : "false");
    }
};

Printer<int>::print(42);            // 42
Printer<bool>::print(true);         // true (pas "1")
```

Le compiler choisit la version correspondante la plus spécialisée.

Spécialisation partielle (class templates uniquement — les function templates ne peuvent pas être partiellement spécialisés en C++) :

```cpp
template <typename T> struct IsPointer        { static const bool value = false; };
template <typename T> struct IsPointer<T*>    { static const bool value = true;  };

IsPointer<int>::value;     // false
IsPointer<int*>::value;    // true
```

Très utilisé dans les type traits (par ex. `std::is_pointer`).

---

## 6. Vue matérielle/compilation — le coût des templates

### 6.1 À la compilation (Compile-time)

Les templates sont lents à compiler car :
- Le compiler doit analyser (parse) le corps du template deux fois — une fois lors de la définition (vérification syntaxique) et une fois par instantiation (vérification sémantique).
- Chaque TU qui utilise une instantiation de template la ré-analyse et la ré-instancie.
- Les bibliothèques génériques lourdes (par ex. Eigen, Boost) explosent en une multitude d'instantiations internes.

Atténuations : precompiled headers, `extern template` (C++11), garder les templates simples.

### 6.2 À l'exécution (Run-time)

Coût nul (Zero cost). Le code instancié est exactement aussi rapide qu'une version spécifique écrite à la main. Souvent *plus rapide* que le polymorphisme à l'exécution (pas de vtable, plus d'opportunités d'inlining).

### 6.3 Taille du binaire

Chaque instantiation utilisée contribue son propre code machine :

```
   Stack<int>::push    ─┐
   Stack<int>::pop      ├──── three full sets of methods in the binary
   Stack<int>::empty    │
                        │
   Stack<string>::push  ─┐
   Stack<string>::pop    ├──── another full set
   Stack<string>::empty  │
```

Les binaires grossissent si vous instanciez de nombreuses fois. Le linker déduplique les instantiations identiques entre les TU (vous n'obtenez donc pas cinq copies de `Stack<int>::push`), mais les types distincts embarquent chacun la leur.

---

## 7. Le mot-clé `typename` à l'intérieur des templates

Lorsque vous avez un *nom dépendant* (dependent name — un nom dont le sens dépend d'un paramètre de template), le compiler ne sait pas s'il s'agit d'un type ou d'une valeur. Vous devez le lui indiquer :

```cpp
template <typename T>
void f() {
    typename T::iterator it;        // 'typename' indique au compiler que T::iterator est un type
}
```

Voir [`TYPENAME.md`](TYPENAME.md).

---

## 8. Conseils & astuces

### 8.1 Le header-only est la norme

Définissez les fonctions template et les member templates de class dans le header. N'essayez pas de les séparer entre `.hpp/.cpp` — les instantiations provenant d'autres TU ne trouveront pas les définitions.

### 8.2 SFINAE et `enable_if` (C++98 le propose via `std::enable_if<bool, T>::type` — mais à peine utile avant C++11)

Substitution Failure Is Not An Error : un template qui échoue à s'instancier ne génère pas d'erreur s'il existe un autre candidat valide. Largement utilisé dans la programmation générique moderne. Hors programme pour 42.

### 8.3 Gardez les paramètres au strict minimum

Un template avec cinq paramètres de type et trois paramètres non-type est difficile à instancier correctement. Les paramètres de template avec argument par défaut (uniquement sur les class templates en C++98) peuvent aider :

```cpp
template <typename T, int N = 10>
class Buffer { /* ... */ };

Buffer<int> b;        // N vaut 10 par défaut
Buffer<int, 20> c;
```

### 8.4 Forcer l'instantiation lorsque vous avez besoin d'un symbole

```cpp
template class Stack<int>;       // instantiation explicite — force le compiler
                                  // à générer tout le code de Stack<int> dans cette TU
```

Utile dans la conception de certaines bibliothèques, mais rare à 42.

### 8.5 Les erreurs de compilation sont verbeuses

Une seule mauvaise instantiation produit souvent cinquante lignes d'erreur. Lisez la **première** erreur et la ligne qui indique `in instantiation of …` — c'est votre point d'appel (call site).

### 8.6 Les templates sont duck-typed
```cpp
template <typename T>
T add(T a, T b) { return a + b; }
```

`T` n'a pas besoin d'être déclaré où que ce soit ; tout ce que vous passez doit supporter `+`. Si ce n'est pas le cas, vous obtenez une erreur de compilation provenant de l'intérieur du corps du template — à l'emplacement du `+`.

Il s'agit d'une programmation « à concepts implicites ». Les concepts de C++20 rendent cela explicite ; en C++98, vous écrivez de la documentation.

### 8.7 Ne luttez pas contre le système de types

Si `T` peut être quelque chose pour lequel `+` ne fonctionne pas, vous obtiendrez des erreurs du compiler lorsque quelqu'un essaiera avec un type incompatible. Soit vous l'acceptez, documentez les exigences, soit vous limitez le template aux types connus via la specialization.

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `undefined reference to Stack<int>::push()` (linker) | Corps du template dans un `.cpp` non visible pour la TU qui l'utilise | Déplacez les définitions dans le header |
| `'iterator' in 'class T' does not name a type` | `typename` oublié pour un nom dépendant | Préfixez avec `typename` : `typename T::iterator it;` |
| `template argument deduction failed` | Les arguments ne correspondent pas de manière unique à la signature du template | Fournissez des arguments de template explicites, ou corrigez l'appel |
| `instantiation depth exceeds maximum` | Template récursif qui ne se termine pas | Ajoutez une specialization pour le cas de base |
| Temps de compilation qui durent des heures | Metaprogramming lourd avec templates | Réduisez la profondeur des templates ; mettez en cache les instantiations ; utilisez des precompiled headers |

---

## 10. Visual summary

```
              ┌────────────────────────────────────────────┐
              │  template <typename T>                      │
              │  T max(T a, T b) { return a > b ? a : b; } │
              └──────────────────┬─────────────────────────┘
                                 │
                                 │   compiler emits one body per
                                 │   distinct T actually used:
                                 ▼
              ┌──────────────────────────────────────────┐
              │   max<int>(int, int)        ← in binary  │
              │   max<double>(double, …)    ← in binary  │
              │   max<std::string>(…)       ← in binary  │
              └──────────────────────────────────────────┘

              parameters:   typename T          (a type)
                            class T             (same)
                            int N               (a non-type — value)

              specializations:   template <> struct X<T>{} (full)
                                 template <T> struct X<T*>{} (partial, classes only)

              header-only is the rule. compile-time cost: high.
              runtime cost: zero. binary size: grows per instantiation.
```

---

## 11. Practice

1. Pourquoi les templates sont-ils presque toujours définis dans des headers ? *(L'instantiation nécessite le corps complet dans chaque TU qui l'utilise ; une définition séparée dans un `.cpp` n'est pas visible au point d'appel.)*
2. Quelle est la différence entre `class T` et `typename T` dans la liste des paramètres de template ? *(Aucune — tous deux introduisent un paramètre de type.)*
3. Quelle est la différence entre `Stack<int>` et `Stack<int>` instanciés dans deux TU ? *(C'est le même type. Le linker déduplique le code ; les deux TU voient un seul symbole par membre.)*
4. Pourquoi les templates de fonction ne peuvent-ils pas faire l'objet d'une partial specialization ? *(Choix de conception de C++98 — l'overload couvre la plupart des cas d'usage ; la partial specialization ne fonctionne que pour les class templates.)*