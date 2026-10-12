# `typename` — "Fais-moi confiance, c'est un type"

> **TL;DR.** Deux rôles sans rapport partagent ce mot-clé : (1) déclarer un template type parameter (interchangeable avec `class`) ; (2) indiquer au compiler qu'un *dependent name* à l'intérieur d'un template fait référence à un type, et non à une valeur.

En lien : [`TEMPLATE.md`](TEMPLATE.md) · [`TEMPLATES.md`](../notions/advanced/TEMPLATES.md) · [`STL.md`](../notions/advanced/STL.md)

---

## 1. Rôle #1 — déclarer un template parameter

```cpp
template <typename T>            // T est un type parameter
void f(T value);

template <class T>               // identique — 'class' est juste une syntaxe plus ancienne
void g(T value);
```

Dans une template parameter list, **`typename` et `class` signifient la même chose**. Le choix relève du style. La plupart du code moderne préfère `typename` (il ne ment pas sur la nature du type — `T` peut être une class, un int, un pointer, n'importe quoi).

---

## 2. Rôle #2 — désambiguïser les dependent names

Voici le problème. À l'intérieur d'un template, le compiler est parfois confronté à un nom comme `T::foo`. Est-ce que `foo` est :

- Un type imbriqué (par ex., `T::iterator` si `T` est `std::vector<int>`) ?
- Une static member variable / constante ?
- Une static member function ?

Tant que vous n'avez pas instancié le template, le compiler ne sait pas ce qu'est `T` — et ne peut donc pas le déterminer. Par défaut, la norme C++ stipule : **suppose qu'il s'agit d'une valeur, pas d'un type, sauf si `typename` est utilisé.**

```cpp
template <typename T>
void f() {
    T::iterator it;          // ERROR — le compiler pense que T::iterator est une valeur
}
```

Indiquez-le-lui explicitement :

```cpp
template <typename T>
void f() {
    typename T::iterator it; // OK — promesse : T::iterator est un type
}
```

---

## 3. Qu'est-ce qu'un dependent name ?

Un *dependent name* est un nom dont la signification dépend d'un template parameter. `T` lui-même est dépendant. Tout ce qui est accessible via `T` l'est également :

```cpp
template <typename T>
struct X {
    typename T::value_type     v;        // ✓ — typename nécessaire : type
    typename T::size_type      n;        // ✓
    T::static_constant;                  // serait une valeur (pas de typename)
    T t;                                 // ✓ — T est un type déjà connu
};
```

Les noms non dépendants n'ont pas besoin de `typename` :

```cpp
template <typename T>
void f() {
    int x;                               // 'int' n'est pas dépendant ; pas besoin de typename
    std::vector<int>::iterator i;        // non dépendant de T ; pas besoin de typename
    typename std::vector<T>::iterator j; // dépendant de T ; typename requis
}
```

La **règle empirique** : si le nom contient un template parameter dans le chemin qui y mène, préfixez-le avec `typename`.

---

## 4. Exemple pratique — utiliser la STL dans un template

```cpp
template <typename Container>
void printAll(const Container& c) {
    typename Container::const_iterator it;       // typename — Container est un template param
    for (it = c.begin(); it != c.end(); ++it) {
        std::cout << *it << ' ';
    }
}
```

Sans `typename`, le compiler renvoie une erreur :

```
   error: need 'typename' before 'Container::const_iterator' because
          'Container' is a dependent scope
```

Ajoutez `typename` devant tout type imbriqué accédé via un template parameter.

---

## 5. Le cas compagnon — `template` comme désambiguïsateur

Tout comme `typename` pour « ceci est un type », il existe un pendant pour « ceci est un member template » :

```cpp
template <typename T>
void f(T t) {
    t.template doSomething<int>();   // 'template' indique au compiler que doSomething est un template
}
```

Sans cela, le compiler lit `t.doSomething<int>` comme `(t.doSomething) < int >` — une comparaison, et non un appel de template. C'est rare, mais vous le verrez dans du code générique avancé.

---

## 6. Pourquoi cette règle existe-t-elle ?

```cpp
template <typename T>
void f() {
    T::foo * p;
}
```

Qu'est-ce que ceci représente ?

- `T::foo` (un type) `* p` (déclaration d'un pointer nommé `p`) ?
- `T::foo` (une valeur) `*` (multipliée par) `p` (un nom quelconque) ?

Sans information, le compiler doit en choisir un. Le comité a choisi : **supposer qu'il s'agit d'une valeur**. Vous devez donc passer outre cela avec `typename` :

```cpp
typename T::foo *p;        // déclare p comme un pointer vers T::foo (un type)
```

Cette règle a été adoptée en 1998 et codifiée dans C++03 ; vous verrez des livres plus anciens écrits avant que cette règle ne soit largement prise en charge.

---

## 7. Trucs & astuces

### 7.1 Saupoudrez de `typename` dès que vous accédez à un type via un template parameter

```cpp
template <typename T>
struct Adapter {
    typename T::iterator   begin_;
    typename T::iterator   end_;
    typename T::size_type  size_;
};
```

Il vaut mieux ajouter un `typename` inutile que d'en oublier un nécessaire. La plupart des compilers le tolèrent.

### 7.2 En C++20+, `typename` n'est plus requis dans certains contextes (pas en C++98 à 42)

Le comité a assoupli la règle pour les endroits où seul un type a du sens (return types, types de member variables, etc.). C++98/C++17 exige toujours `typename` à ces emplacements.

### 7.3 N'utilisez pas `typename` en dehors des templates

```cpp
typename std::vector<int>::iterator it;   // valide mais inutile en dehors des templates
                                           // plus simple : std::vector<int>::iterator it;
```

`typename` n'a de sens que là où une ambiguïté peut exister — à l'intérieur des templates.

### 7.4 Lisez attentivement : `typename T::iterator` vs `typename T::iterator()`

```cpp
typename T::iterator i;       // déclare une variable
typename T::iterator i();     // PIÈGE — déclare une fonction retournant T::iterator !
typename T::iterator i = T::iterator();   // construit une valeur par défaut
```

Le même piège du « most vexing parse » qui empoisonne le C++ classique.

### 7.5 Les templates avec des templates comme paramètres

```cpp
template <template <typename> class Container>
void f() {
    Container<int> c;         // OK — Container est un template, donc Container<int> est un type
}
```

Notez le `class` intérieur (ou `typename` en C++17+). Cela déclare `Container` comme un *template template parameter* — un template qui accepte un type unique. Utilisé dans certaines conceptions d'allocator/policy ; rare à 42.

---

## 8. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `need 'typename' before 'T::iterator'` | Utilisation d'un nom imbriqué issu d'un template param sans `typename` | Faire précéder de `typename` |
| `expected ';' before 'p'` (dans `T::foo *p`) | Le compiler l'a interprété comme une multiplication | Utiliser `typename T::foo *p;` |
| `expected primary-expression before 'class'` | Utilisation de `template <class T>` hors d'un template (par ex., dans un appel de fonction) | Le retirer ; `class` ne va que dans les template parameter lists |
| `'doSomething' was not declared in this scope` | Un member template a besoin du désambiguïsateur `template` | `obj.template doSomething<int>()` |

---

## 9. Résumé visuel

```
              ┌─────────────────────────────────────────┐
              │  keyword: typename                       │
              │                                          │
              │  two jobs:                                │
              │     1. introduce a type parameter        │
              │        (= 'class' here)                  │
              │     2. disambiguate a dependent name     │
              │        as a type                         │
              └────────────────────┬───────────────────┘
                                    │
                ┌───────────────────┼───────────────────┐
                ▼                                       ▼

       template <typename T>             typename T::iterator it;
       template <class T>                ─────────  ─────────────
       (interchangeable                  requis lors de l'accès
        à cette position)                à un type imbriqué via
                                          un template param.

       (sans cela, le compiler suppose que le dependent name est une valeur.)
```

---

## 10. Pratique

1. Pourquoi `template <class T>` fonctionne-t-il là où `template <typename T>` fonctionne ? *(Ce sont des synonymes dans les template parameter lists.)*
2. Pourquoi `typename T::iterator i;` est-il nécessaire à l'intérieur d'un function template, mais pas à l'extérieur ? *(`T` est un template parameter ; le compiler ne peut pas encore savoir si `T::iterator` est un type ou une valeur. `typename` indique « type ».)*
3. Que fait `t.template foo<int>()` que `t.foo<int>()` ne fait pas ? *(Dans un template, `t.foo<int>` pourrait être analysé comme `(t.foo) < int`. Le mot-clé `template` force l'interprétation comme un appel de member template.)*
4. Pourriez-vous écrire `class T::iterator i;` ? *(Non — `class` sert à déclarer des types class ou dans les template parameter lists, pas à désambiguïser les dependent names.)*