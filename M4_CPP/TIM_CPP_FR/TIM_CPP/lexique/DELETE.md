# `delete` — Exécuter le Destructor **Et** Libérer la Mémoire

> **TL;DR.** `delete` est l'inverse de `new` : il exécute le destructor (réduisant les invariants à néant) puis restitue la mémoire à l'allocateur. La forme correspondante (`delete` vs `delete[]`) doit être exacte, sinon vous obtenez un undefined behavior — généralement un crash, parfois une corruption silencieuse.

En lien : [`NEW.md`](NEW.md) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. Le modèle en deux étapes

```cpp
delete p;
```

Ce qui se passe réellement, dans l'ordre :

```
   étape 1: exécute p->~T()                  — le destructor nettoie l'object
   étape 2: ::operator delete(p, sizeof(T)) — renvoie les octets à l'allocateur
```

Pour `new[]` :

```cpp
delete[] arr;
```

```
   étape 1: lit le cookie à arr-N           — trouve le nombre d'éléments N
   étape 2: exécute ~T() pour chacun des N  — dans l'ordre INVERSE (N-1, N-2, ..., 0)
   étape 3: ::operator delete[](arr, …)     — renvoie les octets à l'allocateur
```

Comparez avec le C :

```c
free(p);     // étape 2 uniquement — pas de destructors
```

En C, la heap ne sait pas qu'elle contient des objets associés à une logique de nettoyage. En C++, `delete` le comprend.

---

## 2. La règle de correspondance des formes

| Allocation | Deallocation |
|---|---|
| `new T(...)`            | `delete p`              |
| `new T[n]`              | `delete[] p`            |
| `new (buf) T(...)`      | `p->~T()` uniquement — pas de delete (mémoire ne provenant pas de `new`) |
| `new (std::nothrow) T`  | `delete p`              |
| `malloc(...)`           | `free(...)` — jamais `delete` |

Ne pas respecter la correspondance est un **undefined behavior**. Formes courantes de ce bug :

```cpp
int *arr = new int[10];
delete arr;            // BUG — devrait être delete[]

int *p = new int(5);
delete[] p;            // BUG — devrait être delete

char *m = (char*)std::malloc(10);
delete[] m;            // BUG — devrait être std::free
```

Le compiler ne peut pas toujours intercepter cela. Certains cas, comme `delete[]` sur un object unique, explosent au runtime. D'autres corrompent la heap silencieusement.

---

## 3. Vue hardware/runtime

L'allocateur stocke des métadonnées dans un header caché avant chaque bloc :

```
   ce que new vous renvoie :
                    ▼
   ┌─────────────┬─────────────────────────────────┐
   │  metadata   │    VOTRE OBJECT (taille octets) │
   │ (size etc.) │                                  │
   └─────────────┴─────────────────────────────────┘
```

Pour le `new[]` d'objets avec des destructors non triviaux, un cookie additionnel au début enregistre le décompte :

```
   ce que new[] vous renvoie :
                          ▼
   ┌─────────────┬─────┬─────────────────────────────────┐
   │  alloc meta │  N  │  obj | obj | obj | ... | obj    │
   └─────────────┴─────┴─────────────────────────────────┘
                          ↑
                          arr (le pointer que vous avez obtenu)
```

Quand vous appelez `delete[] arr`, le runtime lit `N` depuis `arr-sizeof(N)`, exécute `~T()` `N` fois dans l'ordre inverse, puis libère le bloc.

Si vous appelez `delete arr` (sans crochets) là-dessus :

- Il tente d'appeler `~T()` exactement une fois sur `*arr`.
- Il appelle ensuite `::operator delete(arr, ...)` — mais le *vrai* header d'allocation se trouve à `arr - cookie - alloc_meta`. L'allocateur reçoit alors un mauvais pointer à libérer → corruption ou crash.

Problème inverse : `delete[]` sur un object unique alloué avec `new` lit "N" depuis une zone mémoire qui n'est pas un cookie → `N` corrompu (valeur poubelle) → tente de détruire N octets aléatoires → crash.

C'est pourquoi la forme doit concorder. Le runtime ne peut pas deviner.

---

## 4. Ce que fait le destructor

```cpp
class Buffer {
    int *_data;
public:
    Buffer(int n) : _data(new int[n]) {}
    ~Buffer() { delete[] _data; }     // libère ce que nous possédions
};
```

```cpp
Buffer *b = new Buffer(100);
delete b;
```

Séquence :

```
   delete b
       │
       ├── Buffer::~Buffer() s'exécute
       │       │
       │       └── delete[] _data
       │              │
       │              └── libère le bloc int[100]
       │
       └── ::operator delete(b)
              │
              └── libère le bloc Buffer
```

Deux `new`, deux `delete` — équilibré. RAII rend cela automatique pour l'utilisateur de `Buffer` : il lui suffit de faire `delete b;` et tout est nettoyé.

---

## 5. `delete` sur un null pointer est sans danger

```cpp
int *p = 0;
delete p;           // sans effet, pas de crash, pas d'UB
delete[] p;         // également sûr
```

Le standard le garantit. Le code défensif tel que :

```cpp
if (p) delete p;
```

est inutile. Un simple `delete p;` fonctionne.

Après la suppression, réinitialisez le pointer à 0 si vous devez réutiliser la variable :

```cpp
delete p;
p = 0;              // optionnel mais défensif — une réutilisation accidentelle sera un déréférencement propre de nullptr
```

---

## 6. Le double-free est un désastre

```cpp
int *p = new int(5);
delete p;
delete p;           // UB — bloc déjà libéré ; métadonnées de l'allocateur corrompues
```

Après `delete p`, le bloc est retourné dans la free list de l'allocateur. Un second `delete` trouve les métadonnées dans un état inattendu et peut :

- Crash immédiatement (meilleur des cas).
- Corrompre la free list (ensuite un `new` ultérieur renverra un mauvais pointer).
- Casser silencieusement une autre allocation quelconque des heures plus tard.

Assignez le pointer à 0 pour éviter les doubles libérations accidentelles, ou concevez l'ownership pour rendre le double-free structurellement impossible.

---

## 7. Le use-after-free est aussi un désastre

```cpp
int *p = new int(5);
delete p;
*p = 10;            // UB — la mémoire peut être réutilisée, peut appartenir à autre chose désormais
```

Après le delete, les octets sont toujours là *pendant une microseconde* — le mapping de l'espace d'adressage est inchangé. Ainsi, un use-after-free peut "fonctionner" dans de minuscules tests et corrompre la mémoire en production. Invalidez toujours les pointers après un delete.

---

## 8. Sécurité des exceptions dans les destructors

Un destructor ne doit **pas lever d'exception**. S'il le fait, et qu'une autre exception est déjà en cours de propagation (par exemple, le destructor était en train de s'exécuter pendant le stack unwinding), le programme appelle `std::terminate`.

```cpp
class C {
public:
    ~C() {
        if (problem()) throw std::runtime_error("bad");    // Ne faites PAS cela
    }
};
```

À la place, logguez et étouffez l'erreur, ou signalez-la par un canal différent.

---

## 9. `operator delete` personnalisé

Miroir de `operator new` :

```cpp
class C {
public:
    static void operator delete(void *p, std::size_t size) {
        std::cout << "C::operator delete(" << size << ")\n";
        std::free(p);
    }
};
```

Si vous fournissez un `operator new` personnalisé, vous devez fournir un `operator delete` correspondant. Sinon, le `delete` par défaut (qui appelle `::operator delete`) ne s'associera pas correctement avec l'allocateur de votre `new` surchargé.

---

## 10. Astuces & conseils

### 10.1 Mettre le pointer à 0 après un delete (dans les classes à longue durée de vie)

```cpp
class Owner {
    Resource *_r;
public:
    ~Owner() { delete _r; _r = 0; }   // optionnel, mais défensif
};
```

La class est en cours de destruction, donc `_r = 0` est techniquement superflu. L'intérêt est le suivant : si un destructor n'est pas trivial et pourrait être ré-entrant (gestionnaires de signaux, flux inhabituel), mettre à null évite les accidents.

### 10.2 La "forme canonique orthodoxe" (rule of three) — voir OCF

Si votre class possède un destructor non trivial (c'est-à-dire qu'elle `delete` des éléments), vous avez presque certainement besoin d'un copy constructor et d'un copy assignment personnalisés également. Autrement, les copies par défaut partageront le pointer, et les deux objets tenteront de le supprimer.

```cpp
// MAUVAIS : non-respect de la rule-of-three
class Buffer {
    int *_data;
public:
    Buffer()  : _data(new int[100]) {}
    ~Buffer() { delete[] _data; }
};

void demo() {
    Buffer a;
    Buffer b = a;       // le copy ctor par défaut copie le pointer
}                       // a est détruit → libère _data
                        // b est détruit → tente de libérer le même _data → crash
```

Voir [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

### 10.3 RAII rend les delete manuels rares

À l'intérieur d'un destructor de class, `delete` est parfaitement à sa place. En dehors du code d'une class, un `delete` manuel est un code smell — la plupart du temps, il devrait se trouver à l'intérieur du destructor d'une class possédant la ressource.

### 10.4 Conventions d'ownership

- *Raw pointer* : "Je ne fais que consulter, je ne possède pas ceci." Ne pas supprimer.
- *Pointer en tant que membre avec `delete` dans le destructor* : "Je possède ceci ; rien d'autre ne doit le supprimer."

En C++11+, les smart pointers (`std::unique_ptr`, `std::shared_ptr`) rendent l'ownership explicite. Ce n'est pas le cas en C++98 à 42 — vous l'exprimez via le nommage et les conventions.

### 10.5 N'utilisez pas `delete this` à la légère

Légal mais risqué :

```cpp
void C::release() {
    if (--_refs == 0) delete this;
}
```

Après `delete this`, la fonction ne doit plus toucher à `*this` et l'appelant ne doit plus utiliser le pointer. Utilisé dans certaines conceptions basées sur le comptage de références ; presque jamais approprié dans les modules 42.

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| Crash sur `delete` | Incompatibilité de forme (`delete` vs `delete[]`) | Associer correctement : `new T[]` ↔ `delete[]`, `new T` ↔ `delete` |
| `double free or corruption` | Même pointer supprimé deux fois | Mettre à null après delete ; clarifier l'ownership |
| Heap-use-after-free (asan) | Pointer utilisé après `delete` | Ne pas réutiliser ; ou mettre à null et vérifier |
| Fuite de mémoire (memory leak) | `delete` manquant | Tracer l'ownership ; pour chaque `new`, identifier le `delete` |
| `terminate called recursively` | Un destructor a levé une exception pendant le stack unwinding | Ne levez pas d'exceptions depuis les destructors |
| Double-delete lors d'une exception | Nettoyage manuel + membre RAII tentant de nettoyer la même chose | Choisir l'un des deux — laissez RAII en assumer l'ownership |
---

## 12. Résumé visuel

```
              ┌──────────────────────────────────────────┐
              │   delete p;                              │
              └─────────────────────┬────────────────────┘
                                    │
                         ┌──────────┴──────────┐
                         ▼                     ▼
                  step 1: destruct      step 2: deallocate
                  ────────────────      ──────────────────
                  p->~T() runs.         ::operator delete(p)
                  members destructed.   returns bytes to the
                  base destructors      allocator's free list.
                  run after derived
                  in derived hierarchies.

              ┌──────────────────────────────────────────┐
              │   delete[] arr;                          │
              └─────────────────────┬────────────────────┘
                                    │
                                    ▼
                  read cookie → N. run ~T() N times in reverse.
                  ::operator delete[](arr) returns the block.

              must match new with delete:
                 new T            → delete p
                 new T[n]         → delete[] arr
                 placement new    → p->~T()  (no delete)
                 malloc           → free     (never delete)

              after delete, the pointer is dangling.
              don't read, don't write, don't delete again.
```

---

## 13. Pratique

1. Pourquoi `delete arr;` après `int *arr = new int[10];` est-il un undefined behavior même s'il peut ne pas crash ? *(`delete[]` consulte un cookie situé avant le pointer ; sans lui, l'allocateur reçoit un mauvais pointer / un mauvais décompte de destruction → une corruption qui peut ne pas se manifester immédiatement.)*
2. Pourquoi `delete 0;` (null pointer) est-il sûr ? *(Le standard garantit que `delete` sur un null pointer est un no-op.)*
3. Pourquoi un destructeur qui throw provoque-t-il `terminate` ? *(Si le unwinding est déjà en cours pour une autre exception, le runtime ne peut pas gérer deux exceptions simultanées et se termine.)*
4. Après `delete p; p = 0;`, pourquoi la seconde instruction est-elle défensive mais pas strictement requise ? *(Strictement parlant, vous « ne devriez pas réutiliser » `p` quoi qu'il en soit. Le mettre à 0 garantit que toute réutilisation accidentelle échoue bruyamment au lieu de corrompre silencieusement la mémoire.)*