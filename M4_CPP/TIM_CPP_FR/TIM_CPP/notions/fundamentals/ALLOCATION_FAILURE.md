# Quand `new` échoue — `bad_alloc`, `operator new`, et les rouages internes

Le compagnon de [`MEMORY.md`](MEMORY.md). Cette note-là couvre la manière dont l'allocation fonctionne — stack vs heap, le protocole `new`/`delete`, la durée de vie, valgrind. Celle-ci couvre la partie qui surprend les programmeurs C : **ce qui se passe quand l'allocation échoue**, et **ce qu'est réellement `new` sous le capot**.

Ce qu'il faut retenir en une ligne : en C, vous vérifiez `if (!ptr)` parce que `malloc` renvoie `NULL`. En C++, `new` **ne renvoie jamais `NULL`** — il **lance une exception (`throws`)**. Cette vérification est donc du dead code, et l'ensemble du modèle d'erreur est différent.

> **Focus sur les mots-clés :** [`NEW`](../../lexique/NEW.md) · [`DELETE`](../../lexique/DELETE.md) · [`TRY/CATCH/THROW`](../../lexique/TRY_CATCH_THROW.md)
> **Voir aussi :** [`ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md) (exceptions), [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) (dtor virtuel, pourquoi `delete` est crucial)

---

## Table des matières

1. [`new` throws, it never returns NULL](#1.%20new%20throws%2C%20it%20never%20returns%20NULL)
2. [Why C and C++ differ](#2.%20Why%20C%20and%20C%2B%2B%20differ)
3. [No `try`/`catch` → `terminate()` → `abort()`](#3.%20No%20try%2Fcatch%20%E2%86%92%20terminate%28%29%20%E2%86%92%20abort%28%29)
4. [`bad_alloc` abort vs segfault](#4.%20bad_alloc%20abort%20vs%20segfault)
5. [Catching it, and the `nothrow` escape hatch](#5.%20Catching%20it%2C%20and%20the%20nothrow%20escape%20hatch)
6. [Simulating a failure (and the traps)](#6.%20Simulating%20a%20failure%20%28and%20the%20traps%29)
7. [Under the hood: the new-expression vs `operator new`](#7.%20Under%20the%20hood%3A%20the%20new-expression%20vs%20operator%20new)
8. [Overriding `operator new`](#8.%20Overriding%20operator%20new)
9. [42 guidance](#9.%2042%20guidance)

---

## 1. `new` throws, il ne renvoie jamais NULL

```cpp
Animal *p = new Animal();
if (!p)            // ❌ DEAD CODE — cette branche ne peut jamais être vraie
    ...;
```

Quand `new` **réussit**, `p` est valide. Quand il **échoue**, il lance une exception `std::bad_alloc` *avant* que l'assignation n'ait lieu — l'exécution n'atteint donc jamais la ligne suivante avec un `p` invalide en main. Il n'y a aucun état intermédiaire où `p` pointerait vers quelque chose de corrompu. Le test `if (!p)` est un pur réflexe de C ; supprimez-le.

**Ce que contient `p` en cas d'échec dépend de la variante :**

| Forme | En cas d'échec, `p`… | `if (!p)` est-il valide ? |
|---|---|---|
| `new Animal()` | n'est jamais assigné — l'exécution saute au `catch` le plus proche | non |
| `new (std::nothrow) Animal()` | contient `NULL` | oui |
| `malloc` du C | contient `NULL` | oui |

---

## 2. Pourquoi le C et le C++ diffèrent

Le C et le C++ sont apparentés, mais `new` et `malloc` sont **deux outils différents avec deux protocoles d'échec différents** — et les deux coexistent en C++ (oui, `malloc` renvoie toujours `NULL` en C++ ; cela n'a jamais changé).

La raison fondamentale pour laquelle `new` ne peut pas simplement renvoyer `NULL` réside dans les **constructeurs** :

```c
void *p = malloc(size);     // C : UNE tâche — trouver des octets. NULL = "échec".
```
```cpp
Animal *p = new Animal();   // C++ : (1) allouer  ET  (2) exécuter le ctor
```

`new` alloue **et construit**. Un constructeur n'a **pas de valeur de retour** — il ne peut pas faire un `return NULL` pour signaler un échec. Le C++ a déjà résolu la question "comment un constructeur signale-t-il un échec ?" grâce aux **exceptions** (un ctor qui échoue throw). Comme `new` inclut la construction, l'ensemble de l'opération utilise ce même canal. Ainsi :

- **C / `malloc`** — minimaliste, aucune machinerie au runtime, erreurs via la valeur de retour. "Faites confiance au programmeur." Votre réflexe du `if (!ptr)`.
- **C++ / `new`** — ajoute la construction → doit utiliser des exceptions, car les codes de retour ne peuvent pas s'échapper d'un constructeur.

La rigueur demeure, elle change simplement de cible : en C++, la "protection" ne consiste pas à vérifier les NULL, mais à **associer chaque `new` à un `delete`** (voir [`MEMORY.md`](MEMORY.md#3.%20Heap%20%E2%80%94%20new%20and%20delete)).

---

## 3. Pas de `try`/`catch` → `terminate()` → `abort()`

Un `bad_alloc` non intercepté ne corrompt rien et ne gèle pas le programme — il **se propage le long de la call stack** à la recherche d'un gestionnaire, et si aucun n'existe :

```
new throws bad_alloc
  → pas de gestionnaire dans cette fonction → unwind vers le haut
  → pas de gestionnaire dans main()        → unwind au-delà de main
  → rien au-dessus de main                 → std::terminate()
  → terminate() appelle abort()            → SIGABRT (6), code de sortie 134
```

Sortie réelle observée sur macOS / Apple Silicon (`-std=c++98`) :

```
libc++abi: terminating due to uncaught exception of type std::bad_alloc: std::bad_alloc
exit code: 134                 # 128 + 6 (SIGABRT)
```

**Subtilité du stack unwinding :** à mesure que l'exception remonte, les destructors des objets automatiques (sur la stack) dans les portées abandonnées sont exécutés correctement. Mais **les objets sur le heap que vous avez déjà alloués avec `new` et pas encore libérés avec `delete` fuient** — les lignes `delete` sont ignorées :

```cpp
Animal *a = new Dog();   // réussit
Animal *b = new Cat();   // throw ici
delete a;                // ← IGNORÉ — a fuite pendant l'unwinding
delete b;
```

(Pour un programme qui *plante*, cela n'a pas d'importance — l'OS récupère tout à la sortie. Cela a de l'importance dans du code à longue durée de vie, comme un serveur.)
Remarque : le fait que la stack soit déroulée ou non pour une exception *non interceptée* dépend de l'implémentation (implementation-defined).

---

## 4. Crash `bad_alloc` vs segfault

Les deux arrêtent le programme, mais ce sont deux types de fin diamétralement opposés.

| | Crash `std::bad_alloc` | Segfault |
|---|---|---|
| Signal / sortie | `SIGABRT` (6), sortie **134** | `SIGSEGV` (11), sortie **139** |
| Déclenché par | **votre programme**, volontairement | l'**OS/matériel**, contre votre volonté |
| Type de comportement | **défini** — chemin d'échec planifié | **indéfini (undefined)** — accès mémoire illégal |
| Message | explicite & spécifique : `std::bad_alloc` | générique : `Segmentation fault` |
| Interceptable ? | **oui** — `try`/`catch` s'en remet | **non** — c'est un signal, pas une exception |
| Analogie en C | (le C n'a aucun équivalent) | le crash classique en C (déréférencement de NULL, mauvais free) |

**Modèle mental :** l'arrêt par `bad_alloc` est une *alarme incendie* — le système de sécurité détecte une anomalie et s'arrête de manière ordonnée (et vous pouvez installer un `catch` pour la gérer). Un segfault, c'est le *sol qui s'effondre* — aucun avertissement, aucun gestionnaire, vous avez touché à une zone mémoire interdite et le matériel vous a stoppé net. Le premier est le *bon* échec (identifié, défini, récupérable) ; le second est le symptôme d'un undefined behavior dans vos pointers.

---

## 5. L'intercepter, et la porte de sortie `nothrow`

Si vous souhaitez le gérer plutôt que de terminer :

```cpp
try {
    Animal *p = new Animal();
    // ...
    delete p;
}
catch (const std::bad_alloc &e) {   // nécessite #include <new>
    std::cerr << "allocation failed: " << e.what() << std::endl;
}
```

Une fois intercepté, le programme récupère la main et sort avec le code 0.

Si vous voulez réellement un retour `NULL` en cas d'échec à la manière du C, optez explicitement pour la forme **`nothrow`** — le seul cas où `if (!p)` après un `new` est correct :

```cpp
Animal *p = new (std::nothrow) Animal();
if (!p)            // ✓ valide ici — nothrow fait en sorte que new renvoie NULL
    ...;
```

---

## 6. Simuler un échec (et les pièges)

Pour forcer un `bad_alloc`, demandez plus de mémoire qu'il n'est possible d'en fournir — mais deux pièges rendent la chose délicate :

### Piège A — `()` vs `[]`

```cpp
new char(n);    // un char UNIQUE, initialisé à la valeur n  → 1 octet, réussit toujours
new char[n];    // un TABLEAU de n chars                     → n octets, peut échouer
```

Les parenthèses allouent un seul objet et utilisent `n` comme sa *valeur* ; les crochets utilisent `n` comme un *décompte*. Pour demander une allocation gigantesque, vous devez utiliser `[]`.

### Piège B — la taille doit être une valeur déterminée au runtime

```cpp
new char[static_cast<std::size_t>(-1)];   // ❌ Erreur de COMPILATION : "array is too large"
```

Une taille constante à la compilation est simplifiée et rejetée par le compiler avant même que le programme ne s'exécute — ce n'est pas l'échec au runtime que vous recherchez. Masquez la taille derrière une valeur évaluée au runtime :

```cpp
char *boom = new char[static_cast<std::size_t>(-1) - argc];   // ✓ throw au runtime
```

`static_cast<std::size_t>(-1)` est la manière propre en C++98 d'écrire `SIZE_MAX` (`-1` passe en wrap-around vers tous les bits à 1).

---

## 7. Sous le capot : la new-expression vs `operator new`

La manière dont `new` est codé se divise en deux parties :

**Partie 1 — `operator new` : une véritable fonction, remplaçable.** Il s'agit de la fonction d'allocation appelée par `new`. Sa déclaration réside dans `<new>` (marquée *"replaceable"*), et sa définition par défaut correspond essentiellement à ceci :

```cpp
void *operator new(std::size_t size) throw(std::bad_alloc) {  // Spécification C++98
    void *p = malloc(size);
    while (!p) {
        new_handler h = get_new_handler();   // hook optionnel enregistré
        if (!h) throw std::bad_alloc();       // abandon → throw
        h();                                  // sinon on le laisse libérer de la mémoire...
        p = malloc(size);                     // ...et on réessaie
    }
    return p;
}
```

Ainsi, l'`operator new` par défaut **appelle bel et bien `malloc`** et **fait bel et bien un `throw std::bad_alloc`** — ce qui confirme tout ce qui a été expliqué plus haut. La boucle avec le `new_handler` est un hook que vous pouvez installer pour tenter de libérer de la mémoire avant d'abandonner.

**Partie 2 — la new-expression : une règle du langage.** `new Animal()` en lui-même est une règle de grammaire générée par le compiler, et non une fonction que vous pouvez lire :

```
new Animal()  se développe en :
  1. operator new(sizeof(Animal))     → mémoire brute (raw storage)  ← Partie 1 (une fonction)
  2. exécution de Animal::Animal() dessus → construction             ← règle du langage
  3. produit un Animal* typé                                         ← règle du langage
```
C'est la séparation allocation-puis-construction : vous pouvez *remplacer*
l'étape d'allocation, mais l'étape de construction est intégrée au langage.
`delete` fait exactement l'inverse : destruction, puis `operator delete`
(→ `free`).

> Le `throw(std::bad_alloc)` du C++98 est une *spécification d'exception dynamique*
> — dépréciée en C++11, supprimée plus tard, mais
> `-std=c++98` l'attend toujours. L'omettre déclenche
> `-Wmissing-exception-spec`.

---

## 8. Surcharger `operator new`

Puisque `operator new` est une vraie fonction, vous pouvez la substituer.
Deux approches distinctes :

- **Remplacer le global** — il n'y en a qu'un seul dans le programme ;
  définissez le vôtre et le linker l'utilisera partout.
- **`operator new` par class** — un membre `static` ; seule cette class
  (et ses dérivées) l'utilise. Les autres conservent le comportement par défaut.

```cpp
class Animal {
    public:
        static void *operator new(std::size_t size) {
            std::cout << "Animal new: " << size << " bytes" << std::endl;
            return ::operator new(size);     // delegate to the global one
        }
        static void operator delete(void *p) {
            ::operator delete(p);
        }
};
```

(`operator new`/`delete` sont implicitement `static` — il n'y a
pas encore d'objet au moment de l'allocation.) Les overloads avec des
paramètres *supplémentaires* coexistent avec la version par défaut : `nothrow` et le placement-`new`
(`new (ptr) T`) sont exactement cela.

**Pourquoi le faire :** suivi des allocations / compteurs de fuites,
memory pools (blocs pré-découpés pour des objets alloués des millions de
fois), mémoire spéciale (alignement, mémoire partagée/GPU). Une paire
globale personnalisée `operator new`/`operator delete` est l'astuce classique
pour un **compteur de fuites maison** — utile ici puisque valgrind ne
tourne pas sur Apple Silicon.

---

## 9. Directives pour 42

- **Ne faites pas de `if (!ptr)` après un `new`.** C'est du code mort. (Valide
  uniquement avec `new (std::nothrow)`.)
- **N'englobez pas `new` dans un `try`/`catch` dans les Mod00–04.** `bad_alloc`
  ne se déclenche que sur un véritable OOM, ce qui n'arrive jamais pour des programmes d'apprentissage.
  Le laisser se propager (et appeler `terminate`) est le comportement idiomatique par défaut —
  en accord avec la règle de CLAUDE.md « pas de gestion d'erreur pour les
  cas impossibles ».
- **Associez toujours chaque `new` à un `delete`** (et `new[]` à
  `delete[]`). C'est la véritable « protection » en C++ — voir
  [`MEMORY.md`](MEMORY.md).
- **Les expériences d'échec ne sont pas des livrables.** Un test de `bad_alloc`
  est parfait pour *comprendre* ; retirez-le avant le rendu — un
  correcteur ne devrait pas tomber sur une allocation géante qui traîne.

---

## Voir aussi

- [`MEMORY.md`](MEMORY.md) — les mécanismes : stack/heap, `new`/
  `delete`, `new[]`, lifetime, RAII, valgrind. **À lire en premier.**
- [`ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md) —
  exceptions, `try`/`catch`, stack unwinding en général.
- [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) — destructeur virtuel :
  pourquoi `delete` via un base pointer en a besoin, object slicing.