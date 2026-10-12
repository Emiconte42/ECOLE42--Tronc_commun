# `volatile` — "The Compiler May Not Optimize Reads/Writes Away"

> **TL;DR.** `volatile` indique au compiler : *"chaque lecture et écriture sur cette variable doit toucher la mémoire dans l'ordre du code source — ne la mets pas en cache dans un registre, ne réordonne pas, n'élimine pas."* C'est destiné au **matériel mappé en mémoire** et aux **gestionnaires de signaux**, pas aux threads.

Connexe : [`CONST.md`](CONST.md) · [`MUTABLE.md`](MUTABLE.md)

---

## 1. Le problème que `volatile` résout

Le compiler est autorisé à optimiser de cette façon :

```cpp
int status;
while (status == 0) {
    /* spin */
}
```

devient, après optimisation :

```
   load  r1, [status]   ; load once
.L:                     ;
   test  r1, r1         ; check the register over and over
   jz    .L
```

Si `status` est mis à jour par un gestionnaire de signal, une interruption ou un périphérique mappé en mémoire, le compiler ne le sait pas — et la boucle ne se termine jamais.

```
                        ┌─────────────────────────┐
   le compiler pense:   │ status ne change pas    │
                        │ entre les itérations    │
                        └────────────┬────────────┘
                                     │
                                     ▼
                        en cache dans un registre pour toujours → boucle infinie
```

`volatile int status;` force :

```
   .L:
   load  r1, [status]   ; reload from memory every iteration
   test  r1, r1
   jz    .L
```

---

## 2. Ce que `volatile` signifie réellement

Un accès `volatile` est un **effet de bord**, tout comme les I/O. Le compiler :

- Doit effectuer la lecture ou l'écriture au point précis du programme source.
- Ne doit pas combiner plusieurs accès `volatile` en un seul.
- Ne doit pas éliminer les accès dont le résultat est inutilisé.
- Doit conserver les accès `volatile` dans l'ordre source **par rapport aux autres accès `volatile`**.

Il **ne fait pas** :

- De synchronisation entre threads.
- De barrière mémoire (memory barrier) sur du matériel multicœur (`x86`, `ARM`, etc. ont chacun leurs propres règles d'ordonnancement).
- En sorte que les accès soient atomiques.

---

## 3. Où `volatile` est approprié

### 3.1 I/O mappées en mémoire (embarqué)

```cpp
volatile uint32_t *uart_status = (uint32_t*)0x4000'1000;
volatile uint32_t *uart_data   = (uint32_t*)0x4000'1004;

void send(char c) {
    while ((*uart_status & TX_READY) == 0) {}   // chaque lecture DOIT toucher le périphérique
    *uart_data = c;
}
```

```
        CPU                         périphérique
         │                               │
   load [uart_status]  ──── bus ────►   le registre reflète l'état actuel
         │                               │
   test bit, loop                        │
         │                               │
   store [uart_data]  ───── bus ────►   octet mis en mémoire tampon, transmis
```

Sans `volatile`, le compiler peut :
- Lire `uart_status` une seule fois et supposer qu'il ne change jamais → attente active (busy-wait) infinie.
- Supprimer l'écriture dans `uart_data` s'il ne voit aucune utilisation ultérieure de la variable.

### 3.2 Gestionnaires de signaux

```cpp
volatile sig_atomic_t stop = 0;

void handler(int) { stop = 1; }

int main() {
    signal(SIGINT, handler);
    while (!stop) { /* travail */ }
    return 0;
}
```

`sig_atomic_t` est le seul type dont le standard C++ garantit qu'un gestionnaire de signaux peut le lire/écrire de manière portable. `volatile` garantit que la boucle principale relit `stop` depuis la mémoire.

### 3.3 Variables locales avec `setjmp` / `longjmp`

Une variable locale qui est écrite entre `setjmp` et `longjmp` et lue après `longjmp` doit être `volatile`, sinon la valeur est indéfinie.

---

## 4. Où `volatile` N'EST PAS une solution

### 4.1 Multithreading

`volatile` ne fournit **aucune** synchronisation sur un CPU multicœur moderne.

```cpp
// INCORRECT
volatile bool ready = false;
volatile int  data  = 0;

// thread A
data = 42;
ready = true;

// thread B
while (!ready) {}
use(data);   // peut encore voir data == 0
```

Le CPU et le compiler peuvent réordonner `data = 42` et `ready = true` l'un par rapport à l'autre du point de vue de B. `volatile` n'empêche pas le réordonnancement à travers les lectures/écritures non-volatile.

```
       cœur A               cohérence des caches              cœur B
   ┌──────────────┐                                    ┌──────────────┐
   │ data = 42    │                                    │ load ready   │
   │ ready = true │                                    │ load data    │
   └───────┬──────┘                                    └──────────────┘
           │
   les écritures peuvent devenir visibles
   pour le cœur B dans n'importe quel ordre
   sur la plupart des architectures
```

Le bon outil est `std::atomic<T>` (C++11+) ou un mutex. C++98 n'a ni l'un ni l'autre de façon portable — utilise `pthread_mutex_t`.

### 4.2 Marquer du code non-thread-safe comme "thread-safe"

Ajouter `volatile` ne fait rien pour l'atomicité, les conditions de course (race conditions) ou l'ordre de visibilité entre les threads. C'est un anti-pattern de pensée magique (cargo-cult).

---

## 5. Syntaxe — mêmes règles que `const`

Lis le type de droite à gauche :

```cpp
volatile int  x;             // un int volatile
int volatile  y;             // identique
volatile int *p;             // pointer vers un int volatile
int *volatile q;             // pointer volatile vers un int (standard)
const volatile int *r;       // pointer vers un int const volatile
                             // (ex: un registre d'état matériel
                             //  que tu dois relire mais ne peux pas écrire)
```

`const volatile` est une combinaison réelle et utile — parfaite pour les registres matériels en lecture seule qui changent sans l'intervention de ton code.

---

## 6. Fonctions membres `volatile`

Le miroir de [`const`](CONST.md) — promet que la fonction peut être appelée sur des objets `volatile` :

```cpp
class Sensor {
    int _value;
public:
    int read() volatile { return _value; }     // peut être appelée sur un Sensor volatile
};

volatile Sensor s;
s.read();         // OK
```

Presque jamais utile dans le code de niveau 42 ; courant dans le code de drivers.

---

## 7. Point de vue matériel — ce que `volatile` produit réellement à la compilation

`volatile` n'est **pas une instruction de barrière mémoire**. Le compiler émet toujours une simple instruction load/store ; il ne peut simplement pas l'éliminer ou la réordonner par rapport à d'autres accès `volatile`.

```
    int n = *p;          int n = *vp;        (vp est volatile int*)
    int m = *p;          int m = *vp;
    
    optimizer:           optimizer:
    un load              deux loads — obligatoire
```

Sur x86, cela s'aligne avec le modèle de mémoire à cohérence de cache : l'instruction load *verra* une valeur qui a été précédemment écrite par un autre cœur, mais l'ordonnancement entre des emplacements non liés n'est pas garanti sans instructions de barrière (`mfence`, `lfence`, `sfence`).

---

## 8. Trucs & astuces

### 8.1 Drivers — encapsuler les registres dans une struct

```cpp
struct UartRegs {
    volatile uint32_t status;
    volatile uint32_t data;
    volatile uint32_t control;
};

UartRegs *uart = reinterpret_cast<UartRegs*>(0x4000'1000);
uart->data = 'A';
```

Plus propre que des pointers nus, et `volatile` se propage à chaque accès aux membres.

### 8.2 N'utilise pas `volatile` pour "je veux que la valeur soit relue pour le debugging"

Compile avec `-O0` à la place. `volatile` est livré dans le code de production ; "ça permet à mon breakpoint de fonctionner" n'est pas une raison valable.

### 8.3 `volatile` et `const_cast`

Tu peux faire `const_cast<volatile T*>(p)` et `const_cast<T*>(vp)` — mêmes règles, qualificateur différent. Ne l'utilise pas à la légère ; tu supprimes un contrat.

### 8.4 Les atomics le remplacent pour les threads — complètement

```cpp
// C++11+, le bon outil:
std::atomic<bool> ready(false);
std::atomic<int>  data(0);
```

Les atomics t'offrent à la fois la garantie "ne pas éliminer lors des optimisations" **et** l'ordonnancement mémoire. `volatile` ne fournit que la première.

---

## 9. La réalité de 42

Pour les modules CPP de 42, tu n'as presque jamais besoin de `volatile`. Il apparaît dans :

- Les projets d'embarqué (hors 42 CPP).
- L'interaction avec des gestionnaires de signaux au niveau de l'OS (hors 42 CPP).
- Du code cargo-culté que tu devrais réécrire sans lui.

Mais tu le **verras** dans les questions d'entretien et les codebases de niveau senior — sois prêt à expliquer *exactement* ce qu'il fait et ce qu'il ne fait pas.

---

## 10. Résumé visuel

```
                ┌─────────────────────────────────────┐
                │   "chaque accès touche la mémoire"  │
                │     (le compiler ne peut éliminer   │
                │        les lectures/écritures)      │
                └────────────────────┬────────────────┘
                                     │
              ┌──────────────────────┼──────────────────────┐
              ▼                      ▼                      ▼
      I/O mappées en        gestionnaires de        variables locales
         mémoire                 signaux             setjmp/longjmp
     (UART, GPIO,          (flag sig_atomic_t)     (préservées à travers
   registres DMA)                                      un long jump)
              │                      │                      │
              └─────── tous des patterns single-thread ─────┘
                                     │
                                     ▼
                        PAS un outil de synchronisation.
                          Utilise std::atomic / mutex
                       pour la visibilité cross-thread.
```

---

## 11. Pratique

1. Est-ce que `volatile int x; x = 1; x = 2; x = 3;` produira trois stores dans l'assembly ? *(Oui — le compiler doit émettre chaque effet de bord.)*
2. Pourquoi une boucle de spin-wait sur un flag non-volatile risque-t-elle de bloquer avec `-O2` ? *(Le compiler extrait le load en dehors de la boucle, mettant en cache la valeur initiale dans un registre pour toujours.)*
3. Pourquoi `volatile std::atomic<int>` est-il redondant sur x86_64 ? *(L'atomic fournit déjà toutes les garanties de `volatile`, plus d'autres encore — ordonnancement, atomicité. `volatile` n'ajoute aucune sémantique utile par-dessus.)*