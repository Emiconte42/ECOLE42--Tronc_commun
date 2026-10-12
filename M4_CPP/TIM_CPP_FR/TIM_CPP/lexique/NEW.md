# `new` — Allouer de la mémoire **ET** y construire un objet

> **TL;DR.** `new` fait **deux** choses : il demande de la mémoire brute au runtime, puis il exécute le constructor sur cette mémoire. `malloc` ne fait que la première moitié. Chaque `new` doit être associé à un `delete` correspondant (ou `delete[]` pour `new[]`).

Associé : [`DELETE.md`](DELETE.md) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#6.1%20new%20%2F%20delete%20instead%20of%20malloc%20%2F%20free)

---

## 1. Le modèle en deux étapes

```cpp
MyClass *p = new MyClass(42);
```

Ce qui se passe réellement, dans l'ordre :

```
   step 1: ::operator new(sizeof(MyClass))  → raw bytes from the heap
   step 2: placement-construct: MyClass(42) at that address
   step 3: return a typed pointer to it
```

Comparez avec le C :

```c
MyClass *p = malloc(sizeof(*p));   // step 1 only — bytes are uninitialized
                                    // (and there's no constructor in C anyway)
```

En C++, après `new MyClass(42)` :
- La mémoire est allouée.
- L'objet est entièrement construit — les invariants sont respectés, les membres sont initialisés.
- Le pointer de vtable est défini (si applicable).
- Vous obtenez un typed pointer ; le compiler sait ce qui s'y trouve.

---

## 2. Vue hardware/runtime — ce qu'est le heap

```
   process address space
   ┌─────────────────┐ high
   │      stack      │   ← function locals; managed by the CPU (rsp register)
   ├─────────────────┤
   │       …         │
   ├─────────────────┤
   │      heap       │   ← grown via brk()/mmap(); managed by the malloc allocator
   ├─────────────────┤
   │     globals     │
   ├─────────────────┤
   │      code       │
   └─────────────────┘ low
```

`new` appelle `::operator new(size_t)` qui (généralement) appelle le `malloc` de la bibliothèque C en sous-main, qui (finalement) effectue un syscall `brk` ou `mmap` pour obtenir des pages auprès du kernel. Le premier appel à `new` que vous effectuez peut prendre des microsecondes ; les suivants réutilisent la mémoire des free lists de l'allocator et prennent des nanosecondes.

```
   first new:    program → libc malloc → maybe a syscall → kernel allocates pages
   later new:    program → libc malloc → free list pop → return cached chunk
```

Ce que vous récupérez est une adresse alignée sur au moins `alignof(std::max_align_t)` (généralement 16 octets sur x86_64). L'allocator stocke ses propres métadonnées dans un header caché juste avant votre bloc — c'est pourquoi « écrire un octet avant le pointer » corrompt le heap.

---

## 3. Les cinq formes de `new`

### 3.1 Objet unique

```cpp
MyClass *p = new MyClass();           // appelle le ctor par défaut
MyClass *q = new MyClass(1, 2);       // appelle le ctor correspondant
delete p;
delete q;
```

### 3.2 Tableau

```cpp
int *arr = new int[10];                // 10 int, default-initialized (non initialisés pour les types intégrés)
MyClass *objs = new MyClass[5];        // 5 objets MyClass construits par défaut
delete[] arr;
delete[] objs;
```

En C++98, vous ne pouvez pas passer d'arguments aux constructors de tableau avec `new[]`. Chaque élément reçoit le constructor par défaut.

### 3.3 Agencement mémoire de `new[]` — le cookie

Lorsque vous faites un `new[]` sur un tableau d'objets ayant des destructors non triviaux, le runtime stocke le nombre d'éléments juste avant votre pointer :

```
       hidden cookie     your pointer
       ─────────────     ────────────
            ▼                ▼
            ┌─────┬─────┬─────┬─────┬─────┬─────┐
            │  N  │ obj │ obj │ obj │ obj │ obj │
            └─────┴─────┴─────┴─────┴─────┴─────┘
```

C'est pourquoi `delete[]` sait combien de destructors il doit exécuter. Mélanger `new[]` avec `delete` (sans crochets) → il ignore le cookie, n'appelle qu'un seul destructor et libère une mauvaise adresse. Undefined behavior, souvent un crash.

```cpp
int *arr = new int[10];
delete arr;              // BUG — devrait être delete[]
```

### 3.4 `new` nothrow

```cpp
#include <new>
int *p = new (std::nothrow) int[1000000000];
if (p == 0) {
    // l'allocation a échoué — gérer proprement
}
```

Le `new` par défaut **lève `std::bad_alloc`** en cas d'échec. `new (std::nothrow)` renvoie un null pointer à la place. La norme 42 l'utilise rarement — les exceptions sont la manière idiomatique en C++ de signaler un échec d'allocation.

### 3.5 Placement `new` — construire dans une mémoire préexistante

```cpp
#include <new>

char buffer[sizeof(MyClass)];
MyClass *p = new (buffer) MyClass(42);     // construit dans 'buffer'

p->~MyClass();                             // le destructor doit être appelé manuellement
                                            // ne PAS faire 'delete p' — buffer n'a pas été alloué via malloc
```

Utilisé par les implémentations de conteneurs (par exemple, le pattern `reserve`+`push_back` de `std::vector`) pour séparer l'allocation de la construction. Avancé ; vous l'éviterez la plupart du temps.

---

## 4. Ce que `new` ne fait PAS

- Il ne met **pas** la mémoire à zéro, sauf si le type possède un constructor qui le fait.
- Il ne vérifie **pas** la validité du pointer ultérieurement — une fois que vous avez un pointer, le runtime l'oublie ; un `delete` inadapté est votre problème.
- Il ne suit **pas** l'ownership. Deux pointers vers le même bloc `new`, deux `delete` — double free, crash.

```cpp
int *a = new int(5);
int *b = a;
delete a;
delete b;             // BUG — double free, undefined behavior
```

---

## 5. Échec du constructor — `new` nettoie sa propre allocation

Si le constructor lève une exception, `new` libère la mémoire :

```cpp
class C { public: C() { throw std::runtime_error("nope"); } };

try {
    C *p = new C();          // mémoire allouée, le ctor throw
                              // 'new' désalloue la mémoire avant de propager
}
catch (...) {
    // p n'a jamais été assigné — et la mémoire a été libérée.
}
```

Cette garantie est le « `new` exception-safe » : vous ne perdez pas de mémoire lors d'un constructor qui lève une exception. (Les sous-objets membres déjà construits sont également détruits dans l'ordre inverse — voir exception safety.)

---

## 6. `new` vs `malloc` — les différences dans un tableau

| | `malloc(sizeof(T))` | `new T()` |
|---|---|---|
| Alloue de la mémoire | oui | oui |
| Appelle le constructor | **non** | **oui** |
| Initialise | laisse des données résiduelles | exécute le ctor ; les types intégrés peuvent rester non initialisés |
| Type-safe | renvoie `void*`, nécessite un cast | renvoie `T*` |
| Mode d'échec | renvoie NULL | lève `std::bad_alloc` (ou renvoie NULL avec `nothrow`) |
| À associer avec | `free` | `delete` |
| Respecte l'alignement | jusqu'à `alignof(max_align_t)` | oui |
| Personnalisable par class | non | oui — overload `operator new` |

Dans les modules CPP de 42 : **utilisez toujours `new`/`delete`**, jamais `malloc`/`free`.

---

## 7. `operator new` personnalisé (par class)

Une class peut définir sa propre allocation :

```cpp
class C {
public:
    static void *operator new(std::size_t size) {
        std::cout << "C::operator new(" << size << ")\n";
        void *p = std::malloc(size);
        if (!p) throw std::bad_alloc();
        return p;
    }
    static void operator delete(void *p) {
        std::cout << "C::operator delete\n";
        std::free(p);
    }
};

C *c = new C();         // appelle C::operator new
delete c;               // appelle C::operator delete
```

Utilisé pour les pools d'objets, les allocators personnalisés, l'optimisation des performances. Hors sujet pour la norme 42, mais utile à reconnaître.

---

## 8. Trucs & astuces

### 8.1 Choix de l'initialiseur

```cpp
int *a = new int;             // non initialisé (types intégrés)
int *b = new int();           // value-initialized à 0  ← à préférer
int *c = new int(42);         // initialisé à 42
```

Pour les types intégrés, `new int()` effectue une initialisation par zéro. `new int` ne le fait pas. C'est un piège vicieux.

### 8.2 Associez toujours la bonne forme

| Allocation     | Désallocation |
|----------------|---------------|
| `new T`        | `delete p` |
| `new T[n]`     | `delete[] p` |
| `new (buf) T`  | `p->~T();` (pas de delete !) |

Une mauvaise association entraîne un undefined behavior.

### 8.3 N'utilisez jamais `new` pour des choses qui n'en ont pas besoin

```cpp
int *p = new int(5);   // à éviter
int  v = 5;             // à faire
```

L'allocation sur le heap est toujours plus lente que sur la stack. Utilisez `new` lorsque vous avez besoin de :
- Une durée de vie de l'objet qui dépasse le scope actuel.
- Polymorphisme (vous avez besoin d'un pointer vers une base class).
- Tailles connues uniquement à l'exécution.

### 8.4 Le RAII encapsule `new`/`delete`

La méthode 42 consiste à placer `new` dans un constructor et `delete` dans le destructor :

```cpp
class Buffer {
    int *_data;
    int  _size;
public:
    Buffer(int n) : _data(new int[n]), _size(n) {}
    ~Buffer()    { delete[] _data; }
private:
    Buffer(const Buffer&);              // désactivé — la copie provoquerait un double-free
    Buffer& operator=(const Buffer&);   // désactivé
};
```

Ou implémentez la forme canonique orthodoxe (OCF) correctement pour permettre la copie. Voir [`MEMORY.md`](../notions/fundamentals/MEMORY.md) et [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

### 8.5 Suivez chaque allocation dans votre destructor

Si votre class alloue avec `new` dans son constructor, comptez les allocations. Votre destructor doit en libérer exactement le même nombre, avec la forme `delete` correspondante.

### 8.6 Utilisez valgrind pour vérifier

```bash
$ valgrind --leak-check=full ./prog
==== LEAK SUMMARY:
====    definitely lost: 0 bytes in 0 blocks
====    indirectly lost: 0 bytes in 0 blocks
```

Si vous voyez « definitely lost », vous avez un `new` sans le `delete` correspondant. Voir [`MEMORY.md`](../notions/fundamentals/MEMORY.md).
### 8.7 `nullptr` est propre à C++11 — pour C++98, utilisez `0` ou `NULL`

```cpp
int *p = 0;          // idiome C++98
int *q = NULL;       // également valide
int *r = nullptr;    // C++11+ — ne compilera pas sous -std=c++98
```

Après un `delete p;`, faire `p = 0;` est une habitude défensive — cela affecte au pointer une valeur invalide connue afin qu'une réutilisation accidentelle ait plus de chances de provoquer un crash visible.

---

## 9. Common errors

| Erreur | Cause | Solution |
|---|---|---|
| Memory leak | `delete` manquant pour un `new` | Associez chaque `new` à un `delete` (de préférence via RAII) |
| `pointer being freed was not allocated` | Mauvaise association `new[]`/`delete`, ou libération d'un stack pointer | Utilisez `delete[]` pour les arrays ; ne faites jamais `delete &localVar` |
| `double free or corruption` | Deux `delete` pour la même allocation | Définissez le pointer à 0 après un delete ; concevez l'ownership clairement |
| `std::bad_alloc` levée | Mémoire insuffisante (ou taille aberrante) | Vérifiez les tailles ; envisagez `new (std::nothrow)` si vous pouvez récupérer de l'erreur |
| Crash à l'intérieur du ctor → leak (s'il n'est pas exception-safe) | Le constructeur alloue des membres manuellement avant son propre throw | Utilisez des objets membres qui possèdent leurs ressources (membres RAII) |

---

## 10. Visual summary

```
              ┌──────────────────────────────────────────┐
              │   T *p = new T(args);                    │
              └─────────────────────┬────────────────────┘
                                    │
                         ┌──────────┴──────────┐
                         ▼                     ▼
                  step 1: alloc         step 2: construct
                  ─────────────         ──────────────────
                  ::operator new        T::T(args) runs in
                  (size = sizeof(T))    that memory.
                  returns aligned       vptr set if needed.
                  raw bytes from heap.  invariants now hold.

              ┌──────────────────────────────────────────┐
              │   forms                                   │
              ├──────────────────────────────────────────┤
              │   new T            ←→  delete p          │
              │   new T[n]         ←→  delete[] p        │
              │   new (buf) T      ←→  p->~T(); (no del) │
              │   new (nothrow) T  ←→  delete p (after   │
              │                        null check)        │
              └──────────────────────────────────────────┘

              every new has exactly one matching delete,
              with the matching form — or you have UB.
```

---

## 11. Practice

1. Pourquoi `new int` laisse-t-il le int non initialisé alors que `new int()` l'initialise à zéro ? *(Default- vs value-initialization. Les parenthèses déclenchent la value-init pour les types built-in.)*
2. Qu'est-ce qui est stocké dans le "cookie" avant un array `new[]` ? *(Le nombre d'éléments, utilisé par `delete[]` pour savoir combien de destructeurs exécuter.)*
3. Pourquoi `delete[]` est-il requis pour les arrays ? *(Les destructeurs de chaque élément doivent s'exécuter ; la forme array lit le cookie pour savoir combien.)*
4. Qu'advient-il de la mémoire si un constructeur throw à l'intérieur de `new` ? *(`new` la désalloue avant de propager l'exception. Aucun leak — en supposant que les membres respectent RAII.)*