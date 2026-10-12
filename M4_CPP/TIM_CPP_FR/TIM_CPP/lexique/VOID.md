# `void` — « Rien », avec des exceptions

> **TL;DR.** `void` est le type pour « aucune valeur ». Les fonctions qui ne renvoient rien renvoient `void`. Les pointeurs vers « n'importe quel type » utilisent `void*`. Vous ne pouvez jamais avoir de valeur de type `void`, jamais déclarer de variable `void`, et jamais appliquer la plupart des opérateurs à un `void`.

En lien : [`CASTS.md`](CASTS.md) · [`NEW.md`](NEW.md)

---

## 1. Les trois utilisations

### 1.1 Fonction ne renvoyant rien

```cpp
void greet() {
    std::cout << "hi\n";
}                              // no return needed
```

Vous pouvez écrire `return;` pour sortir prématurément, mais vous ne devez pas renvoyer de valeur.

### 1.2 Fonction ne prenant rien en paramètre

```cpp
void f();             // C++ — empty parens already mean "no parameters"
void f(void);         // C — required to mean "no parameters"
                       // legal in C++ but redundant
```

En C, `void f()` signifie « fonction avec des paramètres non spécifiés » (style K&R). En C++, les deux formes signifient « aucun paramètre » — mais écrivez `void f()` par souci d'idiomatisme C++.

### 1.3 Pointeur générique : `void*`

```cpp
void *p;              // pointer to "any" type
int  n = 42;
p = &n;
int *pn = static_cast<int*>(p);    // recover the typed pointer
*pn = 100;
```

`void*` transporte une adresse sans aucun type. Vous ne **pouvez pas** le déréférencer directement :

```cpp
*p;                   // ERROR — what type is the result?
p[0];                 // ERROR — same problem
```

Pour l'utiliser, effectuez un cast vers un pointer typé.

### 1.4 (Bonus) `(void)expr` — ignorer la valeur d'une expression

```cpp
(void)unused_param;   // tells the reader/compiler: "yes, I'm intentionally not using this"
```

Courant dans les bases de code de style C. En C++, on commente souvent le nom du paramètre à la place :

```cpp
void f(int /*unused*/) { ... }
```

---

## 2. Ce qu'on ne peut pas faire avec `void`

```cpp
void v;               // ERROR — cannot declare a void variable
void arr[10];         // ERROR — cannot have an array of void
void f();
v = f();              // not even legal — f returns void; you can't assign void
sizeof(void);         // ERROR — void has no size
```

Le seul endroit où `void` « existe » comme quelque chose ressemblant à une valeur est l'instruction de retour d'une fonction renvoyant void :

```cpp
void g() { return; }            // OK — explicit void return
void h() { return f(); }        // OK in C++ — propagating a void result is allowed
```

Cette dernière forme est parfois utile dans les templates.

---

## 3. `void*` — le pointer générique du C

`void*` est la porte de sortie de l'ère C pour dire « j'ai un pointer mais je ne veux pas m'engager sur un type ». Utilisé dans :

- `malloc` / `free` / `realloc` (qui renvoient / prennent `void*`).
- Les callbacks de `qsort`, `bsearch`.
- L'argument de thread de `pthread_create`.
- Les callbacks de bibliothèques C acceptant des données utilisateur.

```cpp
void *malloc(size_t n);
void  free(void *p);
```

En C, `void*` se convertit implicitement vers et depuis n'importe quel pointer d'objet. En C++, c'est plus strict — la conversion **vers** `void*` est implicite, mais la conversion **depuis** `void*` nécessite un cast :

```cpp
int *p = malloc(sizeof(int));               // ERROR in C++ (would compile in C)
int *p = static_cast<int*>(malloc(sizeof(int)));   // OK
int *q = (int*)malloc(sizeof(int));         // C-style cast, also works
```

La norme 42 préfère `static_cast` aux casts de style C.

### Vue matérielle

Un `void*` est simplement une adresse. Sur x86_64, il fait 8 octets. Il n'y a aucune métadonnée sur ce vers quoi il pointe — cette information ne vit que dans la tête du programmeur (ou dans un paramètre `size_t` séparé).

```
   void *p;
   ┌────────────────┐
   │  raw address   │  8 bytes — no type info
   └────────────────┘
```

C'est exactement la raison pour laquelle le C++ ajoute les templates — du code générique typé sans `void*`-et-cast.

---

## 4. `void*` vs `T*` — perte de la sûreté de typage

```cpp
int    n = 42;
double d = 3.14;

void *p = &d;
int  *pi = static_cast<int*>(p);     // legal cast; *pi is now garbage
*pi = 100;                            // writes to d's memory as if it were int — UB
```

Le compiler ne peut pas vous aider — une fois passé par `void*`, vous avez supprimé l'information de type, et le cast de retour vers `int*` n'est qu'une simple affirmation sur laquelle vous pouvez vous tromper.

Pour les modules de 42 : évitez `void*` à moins d'interfacer avec une API C. Préférez les templates ou le polymorphisme pour la généricité.

---

## 5. `void` dans les templates

```cpp
template <typename T>
T identity(T x) { return x; }

void v = identity<void>(?);     // can't — no value of type void
```

`void` ne peut pas apparaître dans un contexte qui requiert une valeur. Les templates qui pourraient se retrouver avec `T = void` nécessitent une spécialisation (par ex., `std::enable_if`, SFINAE — hors programme pour 42).

Une règle plus sûre : concevez des templates qui n'ont pas besoin de `T = void`.

---

## 6. Le cast `(void)expr`

En C, `(void)expr` est la façon standard de dire « évalue cette expression et ignore le résultat, supprime les avertissements concernant les valeurs inutilisées » :

```cpp
(void)printf("hi");     // ignore printf's return value (which is num chars written)
```

En C++, on ne s'en préoccupe généralement pas — la plupart des compilers n'émettent pas d'avertissement pour cela, et le cast est verbeux. Utile dans les définitions de macros lorsque vous voulez une « instruction sans effet » :

```cpp
#define ASSERT(cond) ((void)0)     // disable in release builds
```

---

## 7. Conseils & astuces

### 7.1 Évitez `void*` en C++

Si vous vous surprenez à vouloir utiliser `void*`, demandez-vous :
- Un template pourrait-il faire cela avec une sûreté de typage ?
- Une classe de base avec des fonctions virtuelles pourrait-elle faire cela avec du polymorphisme ?
- S'agit-il d'une frontière d'interopérabilité avec le C ? (Dans ce cas, c'est acceptable.)

### 7.2 Le retour `void` n'est pas « pas de retour »

Une fonction `void` rend toujours la main à l'appelant — elle ne renvoie simplement aucune valeur. Ne la confondez pas avec `[[noreturn]]` (C++11) ou les fonctions qui bouclent indéfiniment / appellent `exit`.

### 7.3 Utilisez `void` pour marquer « aucun paramètre » en C++

```cpp
void f();         // canonical C++
void f(void);     // C-flavored, redundant in C++
```

### 7.4 Cohérence des retours de fonctions void

Dans une fonction `void` :

```cpp
void f(int n) {
    if (n < 0)
        return;             // early exit — fine
    // ...
}
```

Un simple `return;` (sans valeur) est valide et représente souvent la manière la plus propre de sortir prématurément.

### 7.5 N'utilisez pas `void*` pour « cacher » un type de class

Si vous voulez masquer les détails d'implémentation, utilisez l'**idiome Pimpl** — un type `Impl` déclaré par anticipation (forward declaration) auquel on accède par un pointer. Sûr au niveau des types, sûr au niveau des exceptions, facile à déboguer. Bien meilleur que `void*`.

```cpp
// MyClass.hpp
class MyClass {
    class Impl;            // forward decl
    Impl *_impl;
};
```

---

## 8. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `cannot convert 'void*' to 'T*' without a cast` | Conversion implicite de style C non autorisée en C++ | `static_cast<T*>(...)` |
| `void value not ignored as it ought to be` | Utilisation d'une expression renvoyant `void` là où une valeur est attendue | Ne pas l'assigner / ne pas la renvoyer là où une valeur typée est requise |
| `array of void` / `void variable` | Tentative de déclarer `void v;` ou `void arr[10];` | Choisir un type réel |
| Mauvaise utilisation de `void*` causant une corruption mémoire | Pointer casté vers le mauvais type | Éviter `void*` en C++ ; utiliser des templates ou le dispatch virtuel |

---

## 9. Résumé visuel

```
              ┌────────────────────────────────────────────┐
              │   keyword: void                             │
              └─────────────────────┬──────────────────────┘
                                    │
              ┌─────────────────────┼─────────────────────┐
              ▼                     ▼                     ▼
        return type            no parameters         generic pointer
        ─────────────          ──────────────        ──────────────
        function returns       void f();             void *p;
        nothing.               (in C++; C uses       address only,
        plain return; or       'void f(void)')       no type info.
        omit return.                                  must cast back
                                                      to a typed pointer
                                                      to use.

        cannot:
          - declare 'void' variable
          - take sizeof(void)
          - apply * (deref) to void*
          - have an array of void

        in C++ prefer templates and polymorphism over void*.
        in C interop, void* is unavoidable.
```

---

## 10. Pratique

1. Pourquoi `*p` ne fonctionne-t-il pas lorsque `p` est un `void*` ? *(Le type du résultat est inconnu — `void` n'a pas de taille et aucune opération.)*
2. Pourquoi le C++ exige-t-il un cast de `void*` vers `T*` alors que le C le fait implicitement ? *(Le C++ est plus strict sur la sûreté du typage ; le cast force le programmeur à reconnaître qu'il affirme le type.)*
3. Pourquoi `void f(void)` est-il redondant en C++ ? *(En C++, `void f()` signifie déjà « aucun paramètre » ; le `(void)` de style C sert à la compatibilité.)*
4. Une fonction peut-elle renvoyer `void` ? Une variable peut-elle avoir le type `void` ? *(Oui / non. `void` représente « l'absence de valeur » — vous pouvez le renvoyer depuis une fonction mais vous ne pouvez pas le stocker.)*