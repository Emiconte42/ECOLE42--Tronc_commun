# La mémoire en C++ — Guide complet

Où vivent les objets, comment ils sont alloués, quand ils meurent. Les quatre
storage durations, le protocole `new` / `delete`, la variante
pour les tableaux, et la vision matérielle sous-jacente.

Écrit avec le Mod 01 de 42 comme point d'ancrage, mais s'applique à chaque module
qui possède une ressource. Si vous confondez `new` avec `new[]`, ou créez des leaks
via un destructor non virtuel, la solution commence ici.

> **Zoom sur les mots-clés :** [`NEW`](../../lexique/NEW.md) · [`DELETE`](../../lexique/DELETE.md) · [`STATIC`](../../lexique/STATIC.md) · [`SIZEOF`](../../lexique/SIZEOF.md)

---

## Table des matières

1. [The four storage durations](#1.%20The%20four%20storage%20durations)
2. [Stack — the automatic storage](#2.%20Stack%20%E2%80%94%20the%20automatic%20storage)
3. [Heap — `new` and `delete`](#3.%20Heap%20%E2%80%94%20new%20and%20delete)
4. [Arrays — `new[]` and `delete[]`](#4.%20Arrays%20%E2%80%94%20new%5B%5D%20and%20delete%5B%5D)
5. [Static and thread-local storage](#5.%20Static%20and%20thread-local%20storage)
6. [Object lifetime, in order](#6.%20Object%20lifetime%2C%20in%20order)
7. [RAII — the central idea](#7.%20RAII%20%E2%80%94%20the%20central%20idea)
8. [What happens at the machine level](#8.%20What%20happens%20at%20the%20machine%20level)
9. [`new` vs `malloc`, `delete` vs `free`](#9.%20new%20vs%20malloc%2C%20delete%20vs%20free)
10. [Common bugs and how to spot them](#10.%20Common%20bugs%20and%20how%20to%20spot%20them)
11. [Valgrind — reading the output](#11.%20Valgrind%20%E2%80%94%20reading%20the%20output)
12. [In the CPP modules](#12.%20In%20the%20CPP%20modules)
13. [Rules of thumb](#13.%20Rules%20of%20thumb)

---

## 1. The four storage durations

Chaque objet C++ en possède exactement une :

| Durée | Où il réside | Quand il meurt | Comment l'obtenir |
|---|---|---|---|
| **Automatique** | stack | sortie de scope | `T x;` à l'intérieur d'une fonction |
| **Dynamique** | heap | quand vous le `delete` | `T* p = new T;` |
| **Statique** | `.data` / `.bss` | fin du programme | `static T x;` ou à l'échelle du fichier |
| **Thread** (c++11+) | stockage par thread | fin du thread | `thread_local T x;` — pas en c++98 |

Pour 42 (c++98), vous travaillez avec les trois premières. Le stockage
de thread n'existe pas encore ; `thread_local` a été introduit en c++11.

---

## 2. Stack — the automatic storage

### Déclaration

```cpp
void f() {
    Zombie z("Bob");    // automatique, sur la stack
    // utilisation de z...
}                       // ← ~Zombie() s'exécute ici ; z est détruit
```

Pas de `new`, pas de `delete`. L'objet vit pendant la durée du bloc
dans lequel il a été déclaré.

### Quand l'utiliser

- Valeurs à courte durée de vie, taille connue à la compilation.
- Tout ce qui tient dans le scope d'une fonction.
- Lorsque vous souhaitez que la destruction se produise automatiquement en sortie de scope.

### Le coût

Zéro au-delà de la taille de l'objet. Allouer un objet sur la stack correspond à une
**mise à jour unique de registre** — le compiler soustrait la taille
de l'objet du stack pointer (`rsp` sur x86-64) lors de l'entrée dans
la fonction, et la rajoute lors du retour de fonction.

### La limite

La taille de la stack est limitée (par défaut 8 Mo sur Linux, 1 Mo sur les
threads macOS). Allouer un tableau de 10 Mo sur la stack provoque un crash avec
un segfault. Règle : si c'est plus grand que quelques Ko, mettez-le sur la heap.

### Le piège — lifetime

```cpp
Zombie* bad() {
    Zombie z("Bob");
    return &z;       // ← renvoie l'adresse d'un objet en train de mourir
}                    // z est détruit avant que l'appelant ne lise *result
```

La stack frame contenant `z` disparaît au retour de `bad`. L'appelant
se retrouve avec un pointer vers une zone mémoire désallouée.
Toute lecture/écriture = undefined behaviour.

---

## 3. Heap — `new` and `delete`

### Déclaration

```cpp
Zombie* z = new Zombie("Bob");   // une construction
// utilisation de z...
delete z;                         // une destruction + free
```

### Le contrat

**Chaque `new` doit être associé à exactement un `delete`.**

- Oublier le `delete` → **leak**. La mémoire reste allouée jusqu'à
  la fin du programme. Valgrind rapporte "definitely lost".
- Appeler `delete` deux fois sur le même pointer → **double-free**. Corruption
  de la heap, souvent un crash au second appel ou plus tard lors d'une
  allocation sans rapport.
- `delete` un pointer qui n'a pas été retourné par `new` → **undefined
  behaviour**. De même pour un `delete` sur `NULL` — sauf que `NULL` est
  explicitement sûr (no-op).

### Ce que fait `new`

```cpp
Zombie* z = new Zombie("Bob");
```

...est équivalent à :

```cpp
void* raw = operator new(sizeof(Zombie));   // 1. alloue la mémoire brute
Zombie* z = new (raw) Zombie("Bob");        // 2. construit sur place
```

Deux étapes : **allocation brute** (passe ultimement par `malloc`)
puis **construction sur place** (appelle le ctor). Si le ctor
lève une exception, `operator delete` est automatiquement appelé sur la mémoire
brute — aucun leak.

### Ce que fait `delete`

```cpp
delete z;
```

...est équivalent à :

```cpp
z->~Zombie();                     // 1. exécute le destructor
operator delete(z);               // 2. libère la mémoire brute
```

Deux étapes dans l'ordre inverse : **destructor** (peut déclencher des chaînes
de sous-destructors — par ex. `std::string` libérant son buffer sur la heap),
puis **désallocation brute** (retour à `free`).

### Quand l'utiliser

- L'objet doit survivre à la fonction qui l'a créé.
- La taille ou la quantité n'est connue qu'au runtime.
- Polymorphisme — vous avez besoin d'un `Animal*` qui pointe en réalité vers
  un `Dog`, ce qui signifie que l'objet ne peut pas être un `Animal` sur la
  stack (c'est un `Dog` ; il doit vivre à travers un base pointer).

### Le piège — ownership

Lorsque vous retournez un heap pointer, vous transmettez à l'appelant une
**responsabilité** : il doit le `delete`. Documentez ce contrat
dans le header ou par une convention de nommage (`newFoo()` retourne
conventionnellement un élément que l'appelant doit libérer).

---

## 4. Arrays — `new[]` and `delete[]`

### La syntaxe

```cpp
Zombie* horde = new Zombie[10];   // 10 zombies construits par défaut
// ...
delete[] horde;                    // LES 10 destructors s'exécutent, puis free
```

### Les règles clés

- `new T[N]` invoque le **default constructor** de `T` N fois. Vous
  ne pouvez pas passer d'arguments de constructeur par élément en c++98.
- `new T[N]` correspond à **un seul** appel à `operator new[]` pour `N * sizeof(T) +
  cookie`. Pas N allocations.
- `delete[] p` exécute **l'ensemble des N destructors** dans l'ordre inverse, puis
  désalloue l'ensemble du bloc en un seul appel.
- **`new T[N]` combiné avec `delete p` (sans `[]`) est un UB.** Incohérence
  de métadonnées pour l'allocateur — généralement un crash, parfois une
  corruption silencieuse.
- **`new T` combiné avec `delete[] p` est un UB** également.

### Le cookie

L'allocateur a besoin de connaître N pour que `delete[]` puisse exécuter N destructors.
Implémentation typique : préfixer le bloc de 8 octets stockant N.

```
   returned ptr
        ↓
┌────┬──┬──┬──┬──┬──────────┐
│ N  │T0│T1│T2│..│(padding) │
└────┴──┴──┴──┴──┴──────────┘
 ← 8 →
(cookie)
```

`delete[] ptr` effectue `N = ptr[-8]`, puis détruit `ptr[N-1..0]`,
puis libère `ptr - 8`.

### Pourquoi uniquement le default-ctor

`new T[N]("arg")` n'existe pas. Si `T` n'a pas de default ctor, cette
ligne échoue à la compilation. Vous initialisez les éléments après coup :

```cpp
Zombie* horde = new Zombie[N];
for (int i = 0; i < N; ++i)
    horde[i].setName(chosenName(i));
```

### Quand l'utiliser

Rarement, à 42. `std::vector<T>` couvre la majorité des cas à partir
du Mod 08. L'ex01 du Mod 01 utilise `new[]` précisément pour vous faire
assimiler la distinction array-new / array-delete.

---

## 5. Static and thread-local storage

### Durée statique

```cpp
static int counter = 0;    // portée de fichier, ou à l'intérieur d'une fonction (une seule fois)
```

- Initialisé **une seule fois**, avant le démarrage de `main` (ou à la première
  utilisation s'il est local à une fonction).
- Détruit après le retour de `main`.
- Réside dans le segment `.data` (si initialisé) ou `.bss` (si
  initialisé à zéro) de l'image du programme — faisant littéralement partie du
  binaire sur disque pour `.data`, alloué au chargement pour `.bss`.

### Membres class-static

```cpp
class Account {
    static int _nbAccounts;   // déclaration
};

// Dans le .cpp :
int Account::_nbAccounts = 0;   // définition — alloue le stockage
```

Un seul stockage par class, partagé entre toutes les instances. L'exercice
`Account` du Mod 00 entraîne exactement ce concept.

### Thread-local — c++11+ uniquement

Indisponible en c++98. À ignorer pour 42.

---

## 6. Object lifetime, in order

Pour une variable automatique :

```
Entering scope:
  1. Raw storage allocated on stack (sp decrement).
  2. Constructor runs.
  3. ← Object is alive, usable.
Exiting scope:
  4. Destructor runs.
  5. Raw storage released (sp increment).
```

Pour un objet sur la heap :

```
new:
  1. operator new: ask for raw memory (typically malloc).
  2. Constructor runs.
  3. ← Object is alive, usable.
delete:
  4. Destructor runs.
  5. operator delete: return memory to the allocator (typically free).
```

Pour un tableau :

```
new T[N]:
  1. operator new[]: one big malloc for N*sizeof(T) + cookie.
  2. For i in 0..N-1: construct T at offset i. If ctor i throws,
     destroy 0..i-1 in reverse, then rethrow.
  3. ← N objects alive.
delete[] p:
  4. Read N from cookie.
  5. For i in N-1..0: destroy T at offset i.
  6. operator delete[]: free.
```

---

## 7. RAII — the central idea

**Resource Acquisition Is Initialisation.** Chaque ressource (mémoire
sur la heap, descripteur de fichier, socket, mutex) est **détenue** par un objet, et
**libérée** dans le destructor de cet objet.
```cpp
class FileReader {
    std::ifstream ifs;
public:
    FileReader(std::string const& path) : ifs(path.c_str()) {
        if (!ifs) throw std::runtime_error("open failed");
    }
    ~FileReader() {
        // le destructeur de ifs ferme le fichier automatiquement
    }
};

void f() {
    FileReader r("data.txt");
    // ... use r ...
}   // ← fichier fermé ici, quoi qu'il arrive (même en cas d'exception)
```

Le destructor du membre `ifstream` ferme le fichier. `r` est sur la
stack, donc son destructor est garanti d'être exécuté à la sortie du scope —
retour normal *ou* propagation d'exception. Aucun chemin de fuite.

**Le RAII est la raison pour laquelle vous avez rarement besoin de `try/finally` en C++.** Vous bénéficiez
d'un nettoyage déterministe gratuitement.

Pour la mémoire heap, le RAII signifie : **encapsuler `new`/`delete` dans une class dont
le destructor effectue le `delete`**. `std::string` est l'exemple
d'école — vous n'appelez jamais `free` sur le buffer d'un `std::string`
car `~string()` s'en charge.

---

## 8. Ce qui se passe au niveau machine

### Allocation sur la stack

```cpp
Zombie z("Bob");    // sizeof(Zombie) = 32 bytes, disons
```

Sortie du compiler (x86-64, simplifiée) :

```asm
sub  rsp, 32              ; reserve 32 bytes on the stack
lea  rdi, [rsp]           ; point rdi at the reserved space
mov  rsi, "Bob"           ; arg 2: name
call Zombie::Zombie       ; run ctor on stack slot
```

Au retour de la fonction :

```asm
lea  rdi, [rsp]
call Zombie::~Zombie      ; dtor
add  rsp, 32              ; release stack
ret
```

Overhead total : 2 instructions pour l'allocation (`sub`/`add`). L'objet
réside dans le cache L1 si la stack est chaude (quasiment toujours).

### Allocation sur la heap

```cpp
Zombie* z = new Zombie("Bob");
```

...se compile approximativement en :

```asm
mov  rdi, 32              ; arg: size
call operator new          ; returns ptr in rax
mov  rdi, rax
mov  rsi, "Bob"
call Zombie::Zombie       ; ctor
```

`operator new` finit par appeler `malloc`, qui :

1. Vérifie la free list de l'allocateur pour trouver un slot libre de 32 octets.
2. Succès (hit) → O(1), renvoie le slot.
3. Échec (miss) → demande plus de pages au kernel (`brk` ou `mmap`). Le
   kernel marque les pages virtuelles et les mappe paresseusement (lazy mapping) à la
   RAM physique lors du premier accès.

Coût typique : 10-100 ns sur une free list chaude en L1, des microsecondes sur un
`mmap` frais. L'allocation n'est pas gratuite.

### Désallocation sur la heap

```cpp
delete z;
```

...correspond à :

```asm
mov  rdi, z_ptr
call Zombie::~Zombie
mov  rdi, z_ptr
call operator delete
```

`operator delete` → `free`. La mémoire reste généralement avec
l'allocateur (retourne dans la free list), sans être rendue au
kernel. C'est pourquoi `valgrind` peut afficher une fuite même si la
heap est vide à la sortie du programme — l'image du processus conserve
toujours ces pages.

### `new[]` pour les tableaux

Pour `new Zombie[10]` :

```asm
mov  rdi, 328             ; 10 * 32 + 8 (cookie)
call operator new[]
mov  [rax], 10            ; store N in cookie
lea  rbx, [rax + 8]       ; ptr to first element (skipping cookie)
; loop 10 times: ctor at rbx + 32*i
; return rbx
```

Lors du `delete[]` :

```asm
mov  rax, [rbx - 8]       ; read N
; loop: dtor at rbx + 32*i, i = N-1..0
lea  rdi, [rbx - 8]
call operator delete[]
```

---

## 9. `new` vs `malloc`, `delete` vs `free`

| Fonctionnalité | `malloc`/`free` | `new`/`delete` |
|---|---|---|
| Origine | C | C++ |
| Exécute le ctor | ✗ | ✓ |
| Exécute le dtor | ✗ | ✓ |
| Type-safe | ✗ (retourne `void*`) | ✓ |
| En cas d'OOM | retourne `NULL` | lance `std::bad_alloc` |
| Surchargeable par class | non | oui |
| S'associe avec | `free` uniquement | `delete` (ou `delete[]`) |

**Ne les mélangez jamais.** `malloc` + `delete` = UB. `new` + `free` = UB.

À 42, vous utilisez exclusivement `new`/`delete` (le sujet interdit
`*alloc`/`free`). Cela inclut l'intérieur de `operator new` —
si vous le surchargez, utiliser `malloc` en interne est *autorisé* car
c'est ainsi qu'on l'implémente. Mais du point de vue de l'utilisateur, restez dans
le monde de `new`/`delete`.

---

## 10. Bugs courants et comment les repérer

### Fuite (Leak)

```cpp
Zombie* z = new Zombie("Bob");
// ... forgot delete ...
```

**Symptôme :** `valgrind` rapporte "definitely lost: 32 bytes in 1
blocks".

**Solution :** ajouter le `delete` manquant sur chaque chemin de sortie (return,
exception, break prématuré).

### Double-free

```cpp
delete z;
// ... later ...
delete z;   // UB
```

**Symptôme :** crash lors du second `delete` ou plus tard, parfois avec
`*** Error in '...': double free or corruption ***`.

**Solution :** assigner le pointer à `NULL` après le `delete` (un double-
`delete` inoffensif sur un null pointer est sans danger), ou éviter complètement le delete en double.

### Use-after-free

```cpp
delete z;
z->attack("somewhere");   // UB
```

**Symptôme :** crash aléatoire, ou pire, lecture silencieuse de mémoire libérée
(fonctionne parfois, donne parfois des données corrompues). Valgrind l'intercepte avec
"Invalid read of size N".

**Solution :** ne le faites pas.

### Désaccord entre `new` et `delete[]`

```cpp
Zombie* horde = new Zombie[10];
delete horde;               // UB: should be delete[]
```

**Symptôme :** valgrind : "Mismatched free() / delete / delete[]".

**Solution :** faites-les correspondre. `new` ↔ `delete`, `new[]` ↔ `delete[]`.

### Fuite via un destructor non-virtual

```cpp
Animal* a = new Dog();   // Dog owns a Brain*
delete a;                 // if ~Animal is not virtual: Brain leaks
```

**Symptôme :** valgrind montre une fuite du `Brain`. Si `Brain`
possède lui-même quelque chose, la fuite se propage en cascade.

**Solution :** `virtual ~Animal();`.

---

## 11. Valgrind — lire la sortie

```bash
valgrind --leak-check=full --show-leak-kinds=all ./program
```

Messages clés :

- **`definitely lost`** — un pointer vers de la mémoire allouée a été
  écrasé ou est sorti du scope sans `delete`.
- **`indirectly lost`** — ce qui possède cette ressource est lui-même perdu.
  Corrigez la fuite parente, celle-ci disparaîtra.
- **`possibly lost`** — valgrind n'est pas sûr (par ex. un pointer pointe
  au milieu d'un bloc). C'est généralement de votre faute quand même.
- **`still reachable`** — à la sortie, le pointer était toujours conservé par
  quelque chose, mais il n'y a pas eu de `delete` avant de quitter. Ce n'est pas strictement
  une fuite (l'OS la récupère), mais le sujet la traite comme telle.
- **`Invalid read/write of size N`** — use-after-free ou OOB. Arrêtez
  tout, lisez la stack trace.

Les stack traces de Valgrind pointent vers le site d'**allocation** ou
de **désallocation**, pas vers l'endroit où le bug s'est manifesté. La backtrace
indique le thread où l'opération invalide a eu lieu.

---

## 12. Dans les modules CPP

| Module | Concept de mémoire exercé |
|---|---|
| Mod 00 | Stack (objets locaux, tableau de `Contact` dans le phonebook) |
| Mod 01 ex00 | Stack vs heap explicitement (`newZombie` vs `randomChump`) |
| Mod 01 ex01 | Heap pour les tableaux (`new Zombie[N]` + `delete[]`) |
| Mod 01 ex03 | Membre reference (pas d'allocation, mais décisions proches de la mémoire) |
| Mod 02 | Stack, entièrement — `Fixed` n'utilise pas la heap |
| Mod 03 | Stack, principalement — les classes ne contiennent que `std::string` |
| Mod 04 ex01+ | Heap + virtual dtor + deep copy de pointers polymorphiques |
| Mod 08+ | `std::vector`, `std::list` — sur la heap mais encapsulés en RAII |

Le Mod 04 ex01 est l'endroit où tout cela converge. Réussissez-le bien ici,
et le reste vous semblera évident.

---

## 13. Règles empiriques

1. **Préférez la stack.** Si vous n'avez pas *besoin* de la heap, ne l'utilisez
   pas. Le nettoyage automatique est la fonctionnalité de sécurité la plus robuste.
2. **Un seul propriétaire par ressource.** Chaque allocation sur la heap a exactement
   une class/fonction responsable de son `delete`. Si vous n'êtes
   pas sûr de laquelle, repensez votre conception.
3. **Appliquez le RAII à tout.** Ce qui alloue est la même
   chose qui nettoie — généralement via un destructor.
4. **Associez chaque `new` à un `delete`, chaque `new[]` à un
   `delete[]`.** Sans exception.
5. **Destructor virtuel lors d'une suppression via un pointer de base.**
   Toujours.
6. **Protection contre l'auto-assignation dans `operator=`.** Même quand cela semble
   superflu.
7. **Supprimez l'ancien avant d'assigner le nouveau** à l'intérieur de `operator=`, sinon
   vous créez une fuite.
8. **Valgrind avant chaque push.** L'absence de fuite est un critère strict et éliminatoire,
   pas une option.

---

## Voir aussi

- [`REFERENCE.md`](REFERENCE.md) — les references n'allouent pas mais
  ont une sémantique proche de la propriété (ownership).
- [`ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)
  — l'OCF est le mécanisme RAII pour les classes qui possèdent des ressources.
- [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) — virtual destructor,
  object slicing.
- [`CASTS.md`](../../lexique/CASTS.md) — transtypage (casting) de pointers sur la heap.