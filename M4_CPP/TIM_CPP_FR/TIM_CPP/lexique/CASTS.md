# Les quatre casts de C++ — Guide complet

> **Zoom sur les mots-clés :** [`TYPEID`](TYPEID.md) · [`CONST`](CONST.md) · [`EXPLICIT`](EXPLICIT.md)

Le C hérite d'un cast unique et brutal : `(int)x`. Il fera *tout* ce que vous lui demandez — retirer le const, réinterpréter les bits, effectuer un downcast d'un pointer — sans aucun moyen pour le lecteur ou le compiler de savoir *laquelle* de ces opérations vous vouliez effectuer.

Le C++ le remplace par **quatre casts explicites**, chacun restreignant ce que vous pouvez faire. La règle à 42 est : **ne jamais utiliser les casts de style C dans le code C++.** Choisissez toujours le bon cast nommé.

```
C:       (T)value                         // un seul cast, fait tout, n'indique rien
C++:     static_cast<T>(value)            // conversions sûres, vérifiées à la compilation
         dynamic_cast<T>(value)           // downcast vérifié à l'exécution dans des hiérarchies polymorphiques
         const_cast<T>(value)             // ajoute/retire const ou volatile
         reinterpret_cast<T>(value)       // réinterprétation bit à bit
```

---

## Table des matières

1. [Pourquoi les casts C posent problème](#1.%20Why%20C%20casts%20are%20a%20problem)
2. [`static_cast`](#2.%20static_cast)
3. [`dynamic_cast`](#3.%20dynamic_cast)
4. [`const_cast`](#4.%20const_cast)
5. [`reinterpret_cast`](#5.%20reinterpret_cast)
6. [Comparaison côte à côte](#6.%20Side-by-side%20comparison)
7. [Quel cast choisir](#7.%20Which%20cast%20to%20reach%20for)
8. [RTTI — `typeid` et `dynamic_cast`](#8.%20RTTI%20%E2%80%94%20typeid%20and%20dynamic_cast)
9. [Dans le CPP06](#9.%20In%20CPP06)
10. [Pièges courants](#10.%20Gotchas)

---

## 1. Pourquoi les casts C posent problème

Un cast C enchaîne silencieusement jusqu'à trois conversions différentes jusqu'à ce que l'une fonctionne :

```c
const int *p = ...;
int *q = (int *)p;          // retire le const silencieusement
double *d = (double *)q;    // réinterprète les bits silencieusement — même syntaxe
```

Le lecteur voit `(T)x` et ne peut pas savoir ce qui s'est réellement produit. Le compiler ne peut émettre aucun avertissement, car le cast lui ordonne de se taire. C++ sépare ces intentions afin que le compiler et le relecteur puissent les identifier clairement.

---

## 2. `static_cast`

**Objectif :** conversions explicites mais **sûres et vérifiées au moment de la compilation** entre types liés.

```cpp
static_cast<T>(expr)
```

### À utiliser pour :

- Les conversions numériques pouvant entraîner une perte de précision (le compiler émettrait un avertissement sans cela) :
  ```cpp
  double d = 3.7;
  int    i = static_cast<int>(d);        // 3 — la troncature est explicite
  ```
- La conversion entre types de pointer liés dans une hiérarchie non polymorphique :
  ```cpp
  Base *b = new Derived();
  Derived *d = static_cast<Derived*>(b); // vous promettez que b pointe réellement vers un Derived
  ```
- Reconvertir un `void*` en pointer typé :
  ```cpp
  void *raw = ...;
  int  *p   = static_cast<int*>(raw);
  ```
- Enum ↔ int, appels de constructor explicites, conversions définies par l'utilisateur.

### Ne compilera PAS :

- L'ajout ou le retrait de `const` (utilisez `const_cast`).
- La réinterprétation de pointer non liés comme `int*` → `std::string*` (utilisez `reinterpret_cast` — et remettez vos choix de vie en question).
- Le downcasting **à travers une hiérarchie non liée** : le compiler ne peut pas vérifier la hiérarchie pour vous.

### Comment la machine le perçoit

Pour les conversions numériques, il génère une véritable instruction de conversion (`cvttsd2si` pour `double→int` sur x86). Pour les ajustements de pointer au sein d'une hiérarchie de class, il peut générer **une addition d'offset au moment de la compilation** (pour l'héritage multiple). C'est tout — aucune vérification à l'exécution.

---

## 3. `dynamic_cast`

**Objectif :** effectuer en toute sécurité le downcast d'un pointer ou d'une reference de base vers une derived class, **avec une vérification à l'exécution**.

```cpp
dynamic_cast<Derived*>(base_ptr)    // renvoie NULL si le cast est invalide
dynamic_cast<Derived&>(base_ref)    // lance std::bad_cast si invalide
```

### Prérequis

La class de base **doit être polymorphique** — elle doit posséder au moins une fonction `virtual`. Dans le cas contraire, il n'y a pas de vtable, et `dynamic_cast` n'a aucun élément sur lequel s'appuyer pour vérifier.

```cpp
class Animal { public: virtual ~Animal() {} };       // possède une vtable — OK
class Dog : public Animal {};
class Cat : public Animal {};

Animal *a = new Dog();
Dog *d = dynamic_cast<Dog*>(a);        // d == pointer original
Cat *c = dynamic_cast<Cat*>(a);        // c == NULL — l'exécution a indiqué "pas un Cat"
```

### Comment la machine le perçoit

`dynamic_cast` parcourt la structure **type_info** de l'objet à travers la vtable :

```
a
│
├─► vptr ──► vtable ──► type_info "Dog"
│
└─► runtime: "est-ce que Dog hérite-ou-est-égal à la cible ? oui ⇒ renvoyer le pointer ajusté"
                                                            "non ⇒ renvoyer NULL"
```

Ce parcours à l'exécution explique pourquoi `dynamic_cast` est plus lent que `static_cast`. Pour les chemins critiques en performance, préférez repenser la conception plutôt que d'utiliser un downcast.

### À utiliser pour :

- Les rares cas où vous disposez d'un `Base*` et devez exécuter une action qui n'a de sens que pour une class `Derived` précise.
- Le code de bibliothèque qui *doit* vérifier le type réel (sérialiseurs, visitors).

### Règle empirique

Si vous pensez à utiliser `dynamic_cast`, demandez-vous d'abord : « puis-je plutôt ajouter une fonction virtual à `Base`, afin que la sous-classe effectue elle-même le traitement approprié ? » La plupart du temps, la réponse est oui. `dynamic_cast` est un dernier recours.

---

## 4. `const_cast`

**Objectif :** ajouter ou retirer `const` (ou `volatile`) d'un type. C'est le **seul** cast capable de faire cela.

```cpp
const int x = 5;
int *p = const_cast<int*>(&x);       // retire const du type de pointer
*p = 10;                              // COMPORTEMENT INDÉFINI si x était à l'origine const
```

### Le seul usage légitime

Interagir avec des API C ou héritées (legacy) qui acceptent un `char*` alors qu'un `const char*` serait approprié :

```cpp
void legacy_print(char *s);          // API C, ne modifie pas s mais n'est pas déclarée const

const std::string msg = "hello";
legacy_print(const_cast<char*>(msg.c_str()));    // sûr — l'API ne modifie vraiment rien
```

**Modifier un objet véritablement const via `const_cast` est un comportement indéfini (Undefined Behavior).** Le cast vous permet uniquement d'*exprimer* le changement de type ; il ne rend pas l'espace mémoire accessible en écriture.

### Comment la machine le perçoit

`const_cast` ne génère **aucune instruction**. `const` est une promesse faite au moment de la compilation ; le cast supprime simplement cette promesse du système de types.

### Règle empirique

Si vous commencez à écrire un `const_cast`, vous avez presque certainement un problème de conception. La bonne solution consiste généralement à modifier la fonction nécessitant la variable non-const pour qu'elle accepte directement un paramètre non-const dès le départ.

---

## 5. `reinterpret_cast`

**Objectif :** réinterpréter le **motif binaire (bit pattern)** d'une valeur comme étant d'un type différent. Aucune conversion n'a lieu ; le compiler accepte simplement d'examiner la même mémoire à travers un type différent.

```cpp
int          n = 0x41424344;
const char  *s = reinterpret_cast<const char*>(&n);   // inspecter les octets
// s[0..3] sont les octets de n — dépend de l'endianness
```

### À utiliser pour (rare) :

- Le type punning bas niveau entre des types de pointer non liés.
- Convertir un pointer en entier (`uintptr_t`) pour de la journalisation (logging) ou du hachage.
- Le code matériel ou de sérialisation manipulant des dispositions d'octets fixes.

### Ce qu'il ne fait PAS :

- Il ne modifie **pas** les bits.
- Il n'ajoute et ne retire **pas** le `const` (il faudrait aussi utiliser `const_cast`).
- Il ne vérifie **rien** — un mauvais usage est un UB.

### Comment la machine le perçoit

`reinterpret_cast` ne génère **aucune instruction**. La valeur renvoyée est constituée exactement des mêmes bits que l'entrée. Vous avez simplement indiqué au compiler : « traite désormais ces bits comme étant de type T ».

### Règle empirique

Si vous envisagez d'utiliser `reinterpret_cast` dans un module CPP ordinaire, arrêtez-vous. Vous avez presque certainement besoin de `static_cast` (pour les nombres) ou de `dynamic_cast` (pour une hiérarchie). `reinterpret_cast` est comme utiliser un tournevis pour enfoncer des clous — techniquement possible, mais généralement une mauvaise idée.

---

## 6. Comparaison côte à côte

| Cast                | Vérifié ?        | Coût à l'exécution | Modifie les bits ? | Usage typique                        |
|---------------------|------------------|--------------------|-------------------|--------------------------------------|
| `static_cast`       | Compilation      | Zéro (ou conv.)    | Peut-être (num.)  | Conversions sûres de types liés      |
| `dynamic_cast`      | **Exécution**    | Parcours vtable    | Non               | Downcast dans une hiérarchie polymorphique |
| `const_cast`        | Aucun            | Zéro               | Non               | Retirer `const` pour des API legacy  |
| `reinterpret_cast`  | Aucun            | Zéro               | Non               | Réinterprétation de motif binaire    |
| C-style `(T)expr`   | Aucun (tout enchaînement) | Variable | Variable          | **Jamais en C++**                    |

---

## 7. Quel cast choisir

Posez-vous ces questions dans l'ordre :

```
1. Suis-je uniquement en train d'ajouter/retirer const ? → const_cast
2. Vais-je d'un Base* vers un Derived*
   dans une hiérarchie polymorphique, sans être
   sûr à 100 % qu'il s'agit bien d'un Derived ?       → dynamic_cast
3. S'agit-il d'une conversion classique que le
   compiler accepterait via un appel de fonction ?     → static_cast
4. Suis-je en train de réinterpréter la mémoire brute ?  → reinterpret_cast
5. Toute autre situation.                              → vous faites probablement fausse route, prenez du recul
```

---

## 8. RTTI — `typeid` et `dynamic_cast`

Le **RTTI** (Run-Time Type Information) est le mécanisme sous-jacent à `dynamic_cast`. Vous pouvez l'interroger directement :

```cpp
#include <typeinfo>

Animal *a = new Dog();

if (typeid(*a) == typeid(Dog))
    std::cout << "it's a Dog\n";

std::cout << typeid(*a).name() << '\n';   // nom décoré (mangled) dépendant de l'implémentation
```
Deux règles :
- `typeid(*p)` sur un objet polymorphe renvoie le type **dynamique** (réel).
- `typeid(p)` sur un pointer renvoie le type **statique** (déclaré) — presque jamais ce que vous voulez.

Le RTTI a un coût : il ajoute un objet `type_info` à la vtable de chaque class. Certains projets désactivent le RTTI avec `-fno-rtti`. À 42, le RTTI est supposé activé.

---

## 9. Dans CPP06

Le module est structuré autour des casts :

- **ex00 — Serialization.** Convertir une `std::string` en `int`, puis en `float`, puis en `double`. Décider entre `static_cast` (numérique) et un parsing simple.
- **ex01 — Serialization round-trip.** Prendre un pointer, le convertir en un entier non signé, puis faire l'inverse. C'est le cas d'école pour `reinterpret_cast` :
  ```cpp
  uintptr_t serialize(Data *p)   { return reinterpret_cast<uintptr_t>(p); }
  Data*     deserialize(uintptr_t raw) { return reinterpret_cast<Data*>(raw); }
  ```
- **ex02 — Real type.** Étant donné un `Base*`, déterminer vers quel `Derived` il pointe réellement. C'est l'exercice dédié à `dynamic_cast` :
  ```cpp
  if (dynamic_cast<A*>(base)) std::cout << "A\n";
  else if (dynamic_cast<B*>(base)) std::cout << "B\n";
  // ou utiliser des references + try/catch pour std::bad_cast
  ```

Le module est un tour d'horizon — à la fin, vous aurez manipulé chaque cast selon son cas d'usage prévu.

---

## 10. Pièges courants

- **Ne faites jamais de C-cast en C++.** Cela contourne les distinctions établies ci-dessus. Le correcteur à 42 ne le ratera pas.
- **`dynamic_cast` nécessite une base polymorphe.** Pas de fonction virtual → erreur de compiler. Au minimum, ajoutez un destructor `virtual`.
- **`dynamic_cast` sur des references throw**, sur des pointers il renvoie NULL. Choisissez selon que « l'échec » est acceptable ou non.
- **Modifier un objet réellement const via `const_cast` est un UB.** Ne retirez le const (cast away const) que sur des éléments qui ne sont pas réellement const en mémoire.
- **`reinterpret_cast` entre des types sans rapport puis déréférencer est presque toujours un UB** (strict aliasing). L'exception sûre : lire des octets sous forme de `char*` / `unsigned char*`.
- **`static_cast` ne vérifie pas les hiérarchies de class au runtime.** `static_cast<Dog*>(someAnimalPtr)` sur un objet qui est en réalité un `Cat` compile sans problème et plante à l'exécution. Utilisez `dynamic_cast` en cas de doute.
- **L'arithmétique de pointer après un cast respecte la taille du *nouveau* type.** `reinterpret_cast<char*>(int_ptr) + 1` avance d'un octet, pas de 4.