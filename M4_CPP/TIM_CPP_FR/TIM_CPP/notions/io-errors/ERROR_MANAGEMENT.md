# Gestion des erreurs en C++ — Guide complet

> **Zoom sur le mot-clé :** [`TRY/CATCH/THROW`](../../lexique/TRY_CATCH_THROW.md)

Un guide complet pour bien gérer les échecs en C++ — codes de retour, état des streams et tout le mécanisme d'exceptions — incluant une référence de chaque class d'exception standard que vous rencontrerez.

## Table des matières

1. [Les trois stratégies](#1.%20The%20Three%20Strategies)
2. [Codes de retour](#2.%20Return%20Codes)
3. [Vérifications de l'état des streams](#3.%20Stream%20State%20Checks)
4. [Exceptions — Les bases](#4.%20Exceptions%20%E2%80%94%20The%20Basics)
5. [Ce qui se passe réellement lors d'un `throw`](#5.%20What%20Actually%20Happens%20When%20You%20throw)
6. [La hiérarchie des exceptions standard](#6.%20The%20Standard%20Exception%20Hierarchy)
7. [Chaque class d'exception standard expliquée](#7.%20Every%20Standard%20Exception%20Class%2C%20Explained)
8. [Attraper (Catching) — Bonnes pratiques](#8.%20Catching%20%E2%80%94%20Best%20Practices)
9. [Relancer une exception (Rethrowing)](#9.%20Rethrowing)
10. [Classes d'exception personnalisées](#10.%20Custom%20Exception%20Classes)
11. [Exceptions dans les constructeurs](#11.%20Exceptions%20in%20Constructors)
12. [Exceptions dans les destructeurs — Zone de danger](#12.%20Exceptions%20in%20Destructors%20%E2%80%94%20Danger%20Zone)
13. [Spécifications `throw` (C++98) et `noexcept` (moderne)](#13.%20throw%20Specifications%20%28C%2B%2B98%29%20and%20noexcept%20%28modern%29)
14. [Garanties de sécurité des exceptions](#14.%20Exception%20Safety%20Guarantees)
15. [Performance — Le coût réel](#15.%20Performance%20%E2%80%94%20The%20Real%20Cost)
16. [Quand utiliser les exceptions (et quand ne pas le faire)](#16.%20When%20to%20Use%20Exceptions%20%28and%20When%20Not%20To%29)
17. [Pièges courants](#17.%20Common%20Pitfalls)
18. [Aide-mémoire de décision](#18.%20Decision%20Cheat%20Sheet)
19. [Utilitaires du projet template](#19.%20Template%20Project%20Utilities)
20. [En résumé](#20.%20TL%3BDR)

---

## 1. The Three Strategies

Le C++ dispose de trois manières de signaler qu'un problème est survenu, en gros de la plus simple à la plus puissante :

| Stratégie | Quand l'utiliser |
|---|---|
| **Codes de retour** | Petits programmes, validation d'arguments, wrappers simples autour des appels système. |
| **Flags d'état des streams** | Chaque fois que vous lisez/écrivez avec `<iostream>` ou `<fstream>`. |
| **Exceptions** | Erreurs profondes dans la call stack, échecs de constructeur, frontières de bibliothèques. |

Elles se combinent. Un vrai programme utilise les trois — les streams signalent leurs propres échecs, votre code les encapsule dans des codes de retour au niveau des frontières minces, et les exceptions propagent les erreurs à travers plusieurs niveaux d'un coup.

---

## 2. Return Codes

Le pattern classique façon C : la fonction retourne une valeur qui indique à l'appelant si elle a réussi.

```cpp
#define OK  0
#define ERR 1

int open_file(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file)
    {
        std::cerr << "Error: cannot open " << path << std::endl;
        return (ERR);
    }
    // ...
    return (OK);
}

int main()
{
    if (open_file("data.txt") != OK)
        return (ERR);
}
```

**Forces :** aucun mécanisme lourd, aucun coût à l'exécution, facile à appréhender.
**Faiblesse :** les appelants peuvent oublier de vérifier — les échecs silencieux sont le bug classique.

---

## 3. Stream State Checks

Les streams (`std::ifstream`, `std::ofstream`, `std::cin`) maintiennent leurs propres flags qui vous indiquent si la dernière opération a réussi.

```cpp
std::ifstream file("data.txt");

// Check 1: did it open?
if (!file)                         // same as file.fail()
    return (err_msg("Cannot open file"));

// Check 2: read loop (stops on EOF or error)
std::string line;
while (std::getline(file, line))
    std::cout << line << std::endl;

// Check 3: WHY did the loop stop?
if (file.bad())                    // serious I/O error (disk failure, etc.)
    return (err_msg("Read error"));
// if file.eof() — normal end of file, not an error
```

### Les quatre bits d'état

| Flag      | Signification                        | Grave ?     |
|-----------|--------------------------------------|-------------|
| `good()`  | Tout va bien                         | Non         |
| `eof()`   | Fin de fichier atteinte              | Non         |
| `fail()`  | Erreur logique (mauvais format, ouverture impossible) | Oui |
| `bad()`   | Stream corrompu (erreur matériel/OS) | Très        |

`fail()` implique que `good()` est faux. `bad()` implique que `fail()` est vrai. `eof()` est indépendant — une lecture peut réussir et positionner `eof()` parce qu'elle se trouve avoir atteint la fin.

### Activation explicite des exceptions pour les streams

Si vous préférez des E/S basées sur les exceptions, les streams peuvent lancer une exception sur des bits de flags spécifiques :

```cpp
std::ifstream file;
file.exceptions(std::ios::failbit | std::ios::badbit);
file.open("data.txt");    // now throws std::ios_base::failure on failure
```

La plupart du code à 42 utilise de simples vérifications de flags ; ceci est simplement bon à savoir.

---

## 4. Exceptions — The Basics

Une **exception** est un objet qui représente une erreur et se propage automatiquement vers le haut de la call stack jusqu'à ce que quelqu'un l'attrape (catch).

### Lancer une exception (Throwing)

```cpp
#include <stdexcept>

void divide(int a, int b)
{
    if (b == 0)
        throw std::invalid_argument("Division by zero");
    std::cout << a / b << std::endl;
}
```

### Attraper une exception (Catching)

```cpp
int main()
{
    try
    {
        divide(10, 0);
    }
    catch (const std::exception& e)      // catches any standard exception
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }
}
```

### La structure de base

```cpp
try
{
    risky();                              // code that might throw
}
catch (const SpecificType& e)             // first match wins
{
    // ran only if `risky()` threw something compatible with SpecificType
}
catch (const std::exception& e)           // fallback
{
    // any std::exception descendant not caught above
}
// execution resumes here normally, whichever branch ran (if any)
```

Trois règles à mémoriser :

- Un bloc `try` **doit** être suivi d'au moins un `catch`.
- Les `catch` sont testés **de haut en bas** ; le premier type correspondant l'emporte.
- Si aucun `catch` ne correspond, l'exception continue de se propager vers le haut de la call stack.

---

## 5. What Actually Happens When You `throw`

### Étape 1 — l'objet exception est copié dans un stockage sûr

L'objet lancé est copié dans une zone mémoire masquée gérée par le runtime — **pas** sur la stack, car la stack est sur le point d'être détruite. C'est pourquoi il est sans danger de lancer une variable locale : le runtime l'a déjà dupliquée.

```cpp
void foo() {
    std::runtime_error err("boom");
    throw err;         // `err` is copied into exception storage
}                      // `err` itself is destroyed here — the copy survives
```

### Étape 2 — stack unwinding (déroulement de la pile)

Le runtime remonte **vers le haut** de la call stack, frame par frame, à la recherche d'un `catch` correspondant. Pour chaque frame qu'il quitte, il **exécute les destructeurs** de chaque objet local dans cette frame, dans l'ordre inverse de leur construction, avant de détruire la frame.

C'est le mécanisme qui fait fonctionner le **RAII** :

```cpp
void bar() {
    std::ifstream file("data.txt");   // resource acquired
    std::vector<int> buf(1000);       // another resource

    mayThrow();                       // if this throws...
    // ...on the way out, buf's destructor runs, then file's destructor runs
    // → file is flushed & closed, memory is freed, automatically
}
```

C'est la **raison fondamentale de préférer les exceptions à `exit()`** : `exit()` saute le unwinding, donc aucun de ces destructeurs n'est exécuté.

### Étape 3 — le catch s'exécute (ou `std::terminate`)

Lorsque le unwinding trouve un `catch` correspondant, il s'arrête, lie l'exception au paramètre du catch et exécute le gestionnaire (handler).

Si l'exception atteint `main()` sans être attrapée, le runtime appelle `std::terminate()`, qui par défaut appelle `std::abort()` — votre programme se termine brutalement, généralement sans exécuter les destructeurs des objets statiques restants. Ayez **toujours** un `catch (const std::exception&)` de premier niveau dans `main` en guise de filet de sécurité.

---

## 6. The Standard Exception Hierarchy

Toutes les exceptions standard dérivent de `std::exception`. Il existe deux grandes familles en dessous — `std::logic_error` (erreurs du programmeur) et `std::runtime_error` (choses qui échouent à l'exécution) — ainsi que plusieurs enfants directs pour des situations spécifiques.

```
std::exception                       <exception>      base — has .what()
│
├── std::bad_alloc                   <new>            'new' failed
├── std::bad_cast                    <typeinfo>       dynamic_cast of a reference failed
├── std::bad_typeid                  <typeinfo>       typeid on null polymorphic pointer
├── std::bad_exception               <exception>      dynamic-spec violation (C++98 quirk)
├── std::ios_base::failure           <ios>            stream error (when opted in)
│
├── std::logic_error                 <stdexcept>      "this shouldn't happen if the code is correct"
│   ├── std::invalid_argument                         bad argument
│   ├── std::domain_error                             value outside mathematical domain
│   ├── std::length_error                             container/string past max size
│   └── std::out_of_range                             index past valid range
│
└── std::runtime_error               <stdexcept>      "can happen even when the code is correct"
    ├── std::range_error                              result outside representable range
    ├── std::overflow_error                           arithmetic overflow
    └── std::underflow_error                          arithmetic underflow
```
### La ligne de démarcation mentale

> **`logic_error`** = "cela ne devrait pas arriver si le code est correct" — passer un mauvais argument, accéder hors limites, violer un invariant.
> **`runtime_error`** = "cela peut arriver même si le code est correct" — un fichier est absent, le réseau est en panne, une conversion a produit une valeur non représentable.

Cette distinction est la raison même pour laquelle il existe deux familles. Si vous n'arrivez pas à décider de laquelle dériver pour une exception personnalisée, demandez-vous : "un programme parfait pourrait-il tout de même rencontrer ce problème ?" Si oui → `runtime_error`. Si non → `logic_error`.

---

## 7. Chaque classe d'exception standard, expliquée

Chaque entrée comprend : le header dans lequel elle réside, quand la standard library la lance, et quand vous pourriez la lancer vous-même.

### `std::exception` — la racine

- **Header :** `<exception>`
- **Rôle :** base class pour tout ce qui est standard. Jamais lancée directement par la bibliothèque ; habituellement attrapée comme le type de base "filet de sécurité".
- **Interface :** un membre virtuel important — `virtual const char* what() const throw();` — renvoie un message lisible par un humain.

```cpp
try { /* ... */ }
catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
}
```

---

### `std::bad_alloc` — échec d'allocation

- **Header :** `<new>`
- **Lancée quand :** `new` échoue à allouer de la mémoire, ou toute opération de conteneur nécessitant de la mémoire ne peut pas l'obtenir.
- **L'attraper :** vaut rarement la peine d'être gérée spécifiquement — sur les OS modernes, manquer de mémoire signifie généralement que le processus va mourir sous peu de toute façon.

```cpp
try {
    int *huge = new int[std::size_t(-1) / 2];   // absurdly big
}
catch (const std::bad_alloc& e) {
    std::cerr << "out of memory: " << e.what() << '\n';
}
```

La forme `new (std::nothrow) T` évite le throw et retourne `NULL` à la place.

---

### `std::bad_cast` — échec de downcast de reference

- **Header :** `<typeinfo>`
- **Lancée quand :** `dynamic_cast` convertit une **reference** vers un type que l'objet n'est pas réellement. (Les versions pointer de `dynamic_cast` retournent `NULL` au lieu de lancer une exception.)

```cpp
class Base { public: virtual ~Base() {} };
class Derived : public Base {};
class Other   : public Base {};

Base *b = new Other;
try {
    Derived& d = dynamic_cast<Derived&>(*b);    // throws
}
catch (const std::bad_cast& e) {
    std::cerr << "wrong type: " << e.what() << '\n';
}
```

Vous rencontrerez cela dans CPP06.

---

### `std::bad_typeid` — `typeid` sur un pointer polymorphique nul

- **Header :** `<typeinfo>`
- **Lancée quand :** vous appliquez `typeid` à une expression qui déréférence un pointer nul d'un type polymorphique.

```cpp
Base *b = NULL;
try {
    std::cout << typeid(*b).name();   // throws
}
catch (const std::bad_typeid& e) { /* ... */ }
```

Obscur — vous n'écrirez presque jamais de code qui déclenche cela.

---

### `std::bad_exception` — violation de dynamic-spec en C++98

- **Header :** `<exception>`
- **Lancée quand :** une fonction avec une spécification d'exception dynamique (`void f() throw(A, B);`) lance quelque chose hors de la spécification, ET que la spécification inclut `std::bad_exception` — le runtime la substitue alors.
- **En pratique :** une curiosité de C++98. Les spécifications dynamiques ont été dépréciées en C++11 et supprimées en C++17.

---

### `std::ios_base::failure` — erreurs de stream (sur activation)

- **Header :** `<ios>`
- **Lancée quand :** vous avez activé les exceptions de stream via `stream.exceptions(...)` et qu'un bit de flag correspondant est activé.

```cpp
std::ifstream f;
f.exceptions(std::ios::failbit | std::ios::badbit);
try {
    f.open("missing.txt");
}
catch (const std::ios_base::failure& e) {
    std::cerr << "I/O error: " << e.what() << '\n';
}
```

Par défaut, les streams ne lancent **pas** d'exceptions — ils positionnent silencieusement des flags (voir §3).

---

### `std::logic_error` — la base de "l'erreur du programmeur"

- **Header :** `<stdexcept>`
- **Rôle :** base de la famille des erreurs de logique ; jamais lancée directement par la bibliothèque.
- **La lancer vous-même quand :** une précondition a été violée — mauvais argument de l'appelant, invariant rompu. Préférez l'une de ses classes filles plus spécifiques si applicable.

---

### `std::invalid_argument` — un argument incorrect a atteint une fonction

- **Header :** `<stdexcept>`
- **Lancée par :** le constructeur de `std::bitset` sur une mauvaise entrée ; `std::stoi`/`std::stol` (C++11+) quand la chaîne ne peut pas être parsée.
- **La lancer vous-même quand :** une fonction a reçu une valeur qui viole son contrat.

```cpp
void setAge(int age) {
    if (age < 0)
        throw std::invalid_argument("age must be non-negative");
}
```

---

### `std::domain_error` — entrée en dehors du domaine mathématique

- **Header :** `<stdexcept>`
- **Lancée par :** rien dans la standard library C++98 (réservée pour le code utilisateur).
- **La lancer vous-même quand :** une fonction mathématique est appelée hors de son domaine de validité.

```cpp
double safeSqrt(double x) {
    if (x < 0.0)
        throw std::domain_error("sqrt of negative");
    return std::sqrt(x);
}
```

---

### `std::length_error` — la taille dépasserait le maximum

- **Header :** `<stdexcept>`
- **Lancée par :** `std::string` et `std::vector` lorsqu'une opération dépasserait `max_size()`.
- **La lancer vous-même quand :** la taille demandée pour votre propre type similaire à un conteneur ne peut pas être supportée.

```cpp
try {
    std::string s;
    s.resize(s.max_size() + 1);     // throws
}
catch (const std::length_error& e) { /* ... */ }
```

---

### `std::out_of_range` — index au-delà de la plage valide

- **Header :** `<stdexcept>`
- **Lancée par :** `vector::at(i)`, `deque::at(i)`, `string::at(i)`, `bitset::operator[]` (avec vérification), `string::substr(pos)` quand `pos > size()`, `map::at(key)` (C++11+).
- **La lancer vous-même quand :** un appelant passe un index ou une clé qui n'est pas valide.

```cpp
std::vector<int> v(3);
try {
    v.at(10) = 5;                   // throws
}
catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
}
```

L'une des exceptions les plus courantes dans les exercices de CPP de 42 — en particulier dans CPP05 et CPP07.

---

### `std::runtime_error` — la base de "l'échec à l'exécution"

- **Header :** `<stdexcept>`
- **Rôle :** base de la famille des erreurs d'exécution ; jamais lancée directement par la bibliothèque.
- **La lancer vous-même quand :** quelque chose a échoué alors que cela *pouvait* échouer dans un programme correct — I/O externe, parsing, acquisition de ressource. C'est la base de référence pour vos exceptions personnalisées.

```cpp
class FileNotFound : public std::runtime_error {
public:
    FileNotFound(const std::string& path)
        : std::runtime_error("file not found: " + path) {}
};
```

---

### `std::range_error` — un calcul a produit un résultat hors limites

- **Header :** `<stdexcept>`
- **Lancée par :** `std::wstring_convert` (C++11+), et le code utilisateur.
- **La lancer vous-même quand :** la sortie d'une fonction ne peut pas être représentée.

Moins courante que `overflow_error`/`underflow_error` mais sémantiquement plus large — utilisez-la lorsque le problème est "le *résultat* ne rentre pas", indépendamment de la direction arithmétique.

---

### `std::overflow_error` — dépassement arithmétique par le haut (overflow)

- **Header :** `<stdexcept>`
- **Lancée par :** `std::bitset::to_ulong()` lorsque la valeur ne rentre pas.
- **La lancer vous-même quand :** une conversion ou un calcul arithmétique produirait une valeur trop grande.

---

### `std::underflow_error` — dépassement arithmétique par le bas (underflow)

- **Header :** `<stdexcept>`
- **Lancée par :** rien dans la standard library de C++98.
- **La lancer vous-même quand :** une valeur est tombée sous la plage représentable.

---

### Tableau de référence rapide

| Class | Header | Lancée par la stdlib quand | Throw typique de l'utilisateur |
|---|---|---|---|
| `exception` | `<exception>` | jamais (base uniquement) | ne pas faire — dérivez à la place |
| `bad_alloc` | `<new>` | `new` échoue | laisser propager généralement |
| `bad_cast` | `<typeinfo>` | `dynamic_cast<T&>` échoue | rarement |
| `bad_typeid` | `<typeinfo>` | `typeid(*null_ptr)` | rarement |
| `bad_exception` | `<exception>` | violation de dynamic-spec | jamais — bizarrerie de C++98 |
| `ios_base::failure` | `<ios>` | flag de stream déclenché, si activé | wrappers de stream |
| `logic_error` | `<stdexcept>` | jamais (base) | base de précondition |
| `invalid_argument` | `<stdexcept>` | mauvaise init de `bitset` ; échec de parsing `stoi` | validation d'arguments |
| `domain_error` | `<stdexcept>` | jamais | fonction mathématique hors domaine |
| `length_error` | `<stdexcept>` | `string`/`vector` dépassent `max_size()` | conteneurs limités en taille |
| `out_of_range` | `<stdexcept>` | `.at()`, `.substr(pos>size)` | validation d'index/clé |
| `runtime_error` | `<stdexcept>` | jamais (base) | base d'échec à l'exécution |
| `range_error` | `<stdexcept>` | `wstring_convert` | le résultat ne rentre pas |
| `overflow_error` | `<stdexcept>` | `bitset::to_ulong` | overflow arithmétique |
| `underflow_error` | `<stdexcept>` | jamais | underflow arithmétique |

### Une note sur les ajouts post-C++98

C++11 a ajouté `std::system_error`, `std::nested_exception`, `std::exception_ptr`, et quelques autres. Aucun d'entre eux ne s'applique à votre travail à 42 — `-std=c++98` ne sait pas qu'ils existent. Listés ici uniquement pour que vous ne soyez pas confus lorsque vous les rencontrerez dans du code du monde réel.

---

## 8. Attraper les exceptions — Bonnes pratiques

### Attrapez par reference. Toujours.

```cpp
catch (std::exception e)          // MAUVAIS — découpe (slicing) les exceptions dérivées
catch (std::exception* e)         // MAUVAIS — pointer vers quoi ? problèmes de durée de vie
catch (const std::exception& e)   // BON
```
**Pourquoi pas par valeur ?**
**Le slicing.** Si vous lancez une `std::runtime_error` (dérivée de `std::exception`) et que vous l'attrapez par valeur sous la forme d'une `std::exception`, la copie est une simple `std::exception` — la partie dérivée est perdue, y compris les champs supplémentaires ou une chaîne `what()` stockée dans la derived class.

**Pourquoi pas par pointeur ?**
La propriété (ownership) devient ambiguë : est-ce que celui qui attrape l'exception doit la `delete` ? Est-ce que le thrower l'a allouée sur le heap ou a-t-il lancé l'adresse d'une variable locale (qui n'existe plus) ? Lancer et attraper par reference contourne toute la question — le runtime est propriétaire de l'objet exception.

**Pourquoi `const` ?**
Une convention. Dans la quasi-totalité des cas, vous ne faites que lire `e.what()`. Ne retirez le `const` que si vous avez besoin de modifier l'exception avant de la relancer (rethrow).

### L'ordre compte — du plus spécifique au plus général

Les blocs catch sont testés **de haut en bas, sans overload resolution**. Le premier type compatible l'emporte. Ordonnez du plus spécifique au plus général :

```cpp
try { /* ... */ }
catch (const std::out_of_range& e)  { /* most specific */ }
catch (const std::logic_error& e)   { /* less specific */ }
catch (const std::exception& e)     { /* most general  */ }
catch (...)                         { /* anything else */ }
```

Si vous inversez l'ordre et placez `std::exception` en premier, il avalera tout ce qui se trouve en dessous de lui et le compiler vous avertira que les handlers suivants sont inatteignables.

### Le catch-all `catch (...)`

Intercepte **n'importe quelle** exception, y compris celles qui ne sont pas standard (`throw 42;`, `throw "oops";`, ou des types utilisateur qui ne dérivent pas de `std::exception`).

```cpp
catch (...)
{
    std::cerr << "unknown error\n";
    throw;   // optional: rethrow after cleanup
}
```

Utilisez-le avec modération. Il est utile dans deux cas :

- Tout en haut de `main()` comme filet de sécurité ultime (après avoir attrapé `std::exception`, par souci d'exhaustivité).
- À une frontière de langage (API C, destructeur) où **aucune exception ne doit s'échapper**.

Partout ailleurs, il masque des bugs — vous n'apprenez rien sur ce qui a mal tourné.

---

## 9. Relancer une exception (Rethrowing)

### Simple rethrow — `throw;`

À l'intérieur d'un `catch`, `throw;` (sans argument) relance le **même** objet exception. Utilisez-le pour consigner un log, nettoyer des ressources, et laisser une couche supérieure prendre la décision finale :

```cpp
catch (const std::exception& e)
{
    std::cerr << "DB layer noticed: " << e.what() << '\n';
    throw;   // let the next layer up deal with it
}
```

### Wrapping — lancer une exception différente

Si la couche supérieure ne devrait pas avoir à connaître la cause de bas niveau, encapsulez-la (wrapping) :

```cpp
catch (const std::ios_base::failure& e)
{
    throw ConfigLoadError("bad config file: " + std::string(e.what()));
}
```

Une règle empirique : relancez (rethrow) lorsque la couche du dessus est mieux positionnée pour décider ; encapsulez (wrap) lorsque vous souhaitez masquer le vocabulaire de la couche inférieure.

---

## 10. Classes d'exceptions personnalisées

Pour des erreurs explicites, dérivez de `std::exception` (ou de l'un de ses enfants) et faites un override de `what()`.

### La voie la plus simple — dériver de `std::runtime_error`

```cpp
#include <stdexcept>

class FileError : public std::runtime_error
{
public:
    FileError(const std::string& path)
        : std::runtime_error("Cannot open: " + path) {}
};

// Usage:
throw FileError("/tmp/missing.txt");
// Catchable as: FileError, std::runtime_error, or std::exception
```

C'est le pattern le plus courant — le constructeur de `std::runtime_error` prend une `std::string` et la stocke ; `what()` renvoie cette chaîne. Vous héritez de tout ce dont vous avez besoin.

### La méthode façon 42 — dériver de `std::exception`

CPP05 vous entraîne spécifiquement à créer des classes d'exceptions en dérivant directement de `std::exception` et en faisant vous-même l'override de `what()` :

```cpp
class GradeTooHighException : public std::exception {
public:
    virtual const char* what() const throw() {
        return "Grade is too high";
    }
};
```

Deux éléments à noter dans cette signature :
- `virtual` — elle est héritée comme virtuelle depuis `std::exception` ; la redéclarer est une bonne habitude.
- `throw()` — une **exception specification C++98** vide signifiant « cette fonction elle-même ne lance rien ». Vous devez la répéter lors de l'override, sinon le compiler émettra une erreur concernant une spécification non concordante.

### Quand dériver de quelle base

| Parent | Cas d'usage |
|---|---|
| `std::exception` | Vous voulez un contrôle total ; vous renvoyez un message fixe. |
| `std::runtime_error` | Vous souhaitez attacher facilement un message `std::string`. Échec à l'exécution (runtime). |
| `std::logic_error` | Même praticité que runtime_error, mais pour des erreurs de programmation. |
| `std::invalid_argument` / `std::out_of_range` / ... | Vous voulez à la fois le message *et* le type sémantique. |

---

## 11. Exceptions dans les constructeurs

Les constructeurs n'ont **pas de valeur de retour**. Si l'initialisation échoue, les exceptions sont le *seul* moyen propre de le signaler.

Lorsqu'un constructeur lance une exception :

- L'objet est considéré comme **n'ayant jamais été construit**.
- Son destructeur n'est **pas appelé**.
- Mais les destructeurs de tous les **sous-objets membres qui ont déjà été construits** **sont** appelés (dans l'ordre inverse).

```cpp
class Connection {
    Socket s;       // constructed first
    Buffer b;       // constructed second
public:
    Connection() : s(...), b(...) {
        if (!s.alive()) throw std::runtime_error("dead socket");
    }
    ~Connection() { /* ... */ }
};
```

Si le constructeur de `b` lance une exception, `s` est détruit (il avait déjà été construit), mais `~Connection` ne s'exécute **pas** — car la `Connection` n'a jamais pleinement vu le jour.

C'est pourquoi les exceptions + RAII s'associent si bien : une construction ratée ne laisse aucun zombie à moitié construit derrière elle, et aucune ressource ne fuite.

---

## 12. Exceptions dans les destructeurs — Zone de danger

**Règle empirique : les destructeurs ne doivent jamais lancer d'exceptions.**

Si un destructeur lance une exception alors qu'une autre exception est déjà en cours de propagation (c'est-à-dire pendant le stack unwinding), le runtime appelle immédiatement `std::terminate()`. Pas de seconde chance.

```cpp
class Bad {
public:
    ~Bad() {
        throw std::runtime_error("nope");   // if this runs during unwinding → terminate
    }
};
```

Si un destructeur effectue un nettoyage risqué (flusher un socket réseau, par exemple), encapsulez-le pour que rien ne s'en échappe :

```cpp
~MyClass() {
    try { flush(); }
    catch (...) { /* log but do not rethrow */ }
}
```

---

## 13. Spécifications `throw` (C++98) et `noexcept` (moderne)

Comme vous travaillez en C++98 (`-std=c++98`), vous verrez :

```cpp
void foo() throw();                // C++98 — promises no exceptions
void bar() throw(std::exception);  // C++98 — only std::exception may escape
```

Ce sont les **dynamic exception specifications** — dépréciées en C++11 et supprimées en C++17. En C++ moderne, leur remplacement est :

```cpp
void foo() noexcept;               // C++11+
```

Pour les modules C++98 de 42, vous devez principalement savoir qu'elles existent — et plus particulièrement que **`throw()` après la signature d'une fonction membre est la norme lors de l'override de `std::exception::what()`** :

```cpp
virtual const char* what() const throw();
```

Vous l'écrivez pour respecter la spécification de la classe de base ; l'omettre provoque une erreur de compilation sur les toolchains strictes.

---

## 14. Garanties de sécurité des exceptions (Exception Safety Guarantees)

Lorsque vous écrivez une fonction susceptible de lancer une exception, elle fournit l'une des quatre garanties suivantes :

| Niveau        | Signification                                                              |
|---------------|----------------------------------------------------------------------------|
| **No-throw**  | Ne lance jamais d'exception. (Destructeurs, `swap`, move ops doivent viser cela.) |
| **Strong**    | Si elle échoue, l'état du programme reste inchangé (transactionnel).       |
| **Basic**     | Si elle échoue, aucune fuite ni rupture d'invariants, mais l'état peut être altéré. |
| **None**      | Si elle échoue, tout peut arriver. À éviter absolument.                   |

Les conteneurs de la STL offrent pour la plupart la garantie **strong** ou **basic**. Savoir quelle garantie votre code fournit constitue une part importante de l'écriture de code C++ exception-safe.

La technique classique pour assurer une garantie forte est l'**idiome copy-and-swap** : effectuez toutes les opérations sur une copie locale, et ne validez les modifications (via un `swap` no-throw) qu'une fois que tout s'est déroulé sans encombre.

---

## 15. Performances — Le coût réel

Deux visions du coût :

- **Coût nul (zero-cost) tant que rien n'est lancé.** Les compilers modernes utilisent un unwinding basé sur des tables : aucune vérification au runtime lors de chaque appel ; le coût n'est payé que lorsqu'une exception est réellement déclenchée.
- **Très coûteux dès qu'une exception est lancée.** Le stack unwinding parcourt les frames, lit les tables d'unwind, exécute les destructeurs — des ordres de grandeur plus lents qu'un simple `return`.

**Ce qu'il faut retenir :** les exceptions sont réservées aux chemins d'exécution *exceptionnels*. Ne les utilisez pas comme flux de contrôle pour la logique normale (par exemple, un élément « non trouvé » lors d'une recherche dans une map). Utilisez-les pour des conditions qui doivent interrompre l'opération en cours.

---

## 16. Quand utiliser les exceptions (et quand s'en abstenir)

### Utilisez-les lorsque :

- **Les erreurs surviennent profondément dans la call stack.** Une fonction située 8 niveaux plus bas détecte un fichier corrompu. Faire remonter des codes d'erreur à travers 8 fonctions génère du bruit inutile. Un `throw`, un `catch` dans le `main` — c'est réglé.
- **Un constructeur échoue.** La seule façon propre pour `Sed O_Content(path, a, b)` de signaler que « le chemin n'existe pas » est de throw une exception.
- **Vous voulez un nettoyage garanti en cas d'échec.** Associées au RAII, les exceptions garantissent que les gestionnaires locaux de `ifstream`/`ofstream`/`vector`/mutex libèrent leurs ressources même lorsque les choses tournent mal.
- **Vous traversez la frontière d'une bibliothèque qui lance des exceptions.** `std::stoi` lance `std::invalid_argument` ; `vector::at` lance `std::out_of_range`. Vous avez besoin d'un `catch`, sans quoi le programme s'arrête brutalement.
- **Dans le `main`, comme filet de sécurité ultime.**
```cpp
int main(int argc, char** argv) {
    try {
        return run(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "fatal: unknown error\n";
        return 1;
    }
}
```

### Ne les utilisez PAS :

- **Pour les résultats attendus.** "L'utilisateur a saisi une mauvaise commande" n'est pas exceptionnel — c'est une saisie courante. Gérez cela avec un code de retour ou une fonction de validation.
- **Dans les boucles serrées / chemins critiques (hot paths).** Lancer une exception est lent. N'utilisez pas les exceptions pour le contrôle de flux dans du code sensible aux performances.
- **À travers une frontière d'API C.** Le C ne comprend pas les exceptions C++. Si vous exposez une fonction au C, enveloppez le corps dans `try { ... } catch (...) { return -1; }`.
- **Dans les destructors.** Voir §12.
- **Dans les gestionnaires de signaux (signal handlers).** Comportement indéfini (undefined behavior).

---

## 17. Pièges courants

### Ne bouclez pas sur `eof()`

```cpp
// MAUVAIS — traite la dernière ligne deux fois
while (!file.eof())
{
    std::getline(file, line);
    process(line);              // s'exécute une fois de plus après la vraie dernière ligne
}

// BON — s'arrête dès que la lecture échoue
while (std::getline(file, line))
    process(line);
```

### N'attrapez pas tout silencieusement

```cpp
// MAUVAIS — masque les bugs
try { doStuff(); }
catch (...) { }                 // étouffé, vous ne saurez jamais ce qui s'est cassé

// BON — au moins logger
try { doStuff(); }
catch (const std::exception& e)
{
    std::cerr << "Error: " << e.what() << std::endl;
    return (1);
}
```

### N'ignorez pas les valeurs de retour

```cpp
// MAUVAIS
open_file("data.txt");          // et si cela échouait ?

// BON
if (open_file("data.txt") != OK)
    return (ERR);
```

### Ne lancez pas un pointer ou un littéral

```cpp
throw "oops";      // lance un const char* — valide mais pénible à catch
throw 42;          // lance un int — à attraper avec catch(int)
throw new Err();   // lance un pointer — qui le delete ? À NE PAS FAIRE
```

Lancez toujours un **objet** dérivant de `std::exception`.

### Souvenez-vous que `catch` ne boucle pas

Un `catch` s'exécute **une seule fois**. Si le handler lui-même lance une exception, vous êtes de nouveau en mode unwinding à partir de ce point. Il n'y a pas de nouvelle tentative automatique.

### Attraper ce que vous ne pouvez pas gérer

Si un catch ne peut pas récupérer la situation de manière significative, ne l'attrapez pas — laissez-le se propager vers une couche qui le peut. Les catch vides ou de simple transmission sont généralement un mauvais signe (code smell).

### Supposer que les destructors ont été exécutés après `exit()` ou `abort()`

Ce n'est **pas** le cas (pour les objets alloués sur la stack). Seuls les exceptions et `return` effectuent l'unwinding de la stack. `exit()` exécute uniquement les gestionnaires `atexit` et les destructors des objets *static* ; `abort()` saute absolument tout.

---

## 18. Fiche d'aide à la décision

| Situation                              | Utilisation                   |
|----------------------------------------|-------------------------------|
| Valider `argc/argv` dans `main()`      | Code de retour                |
| Ouvrir un fichier                      | Vérification du stream + code de retour |
| Erreur profonde dans la call stack     | Exception                     |
| Constructor pouvant échouer            | Exception (pas de valeur de retour) |
| Vérifier les opérations de lecture/écriture | Flags d'état du stream   |
| Appel de bibliothèque (`.at()`, `stoi`, `dynamic_cast<T&>`) | Exception (vous devez catch) |
| Erreur irrécupérable (doit quitter maintenant) | `err_exit()` ou exception |
| Nettoyage de destructor pouvant échouer | `try`/`catch`, étouffer l'exception |

---

## 19. Utilitaires du template de projet

Le template de projet fournit trois fonctions utilitaires dans `include/utils/errors.hpp` :

```cpp
err_msg("something went wrong");     // écrit sur stderr, retourne ERR (1)
warn_msg("this is suspicious");      // écrit sur stderr, continue
err_exit("cannot recover");          // écrit sur stderr, quitte immédiatement
```

Utilisez `err_msg` pour un return sur une seule ligne :

```cpp
if (argc != 3)
    return (err_msg("Usage: ./program arg1 arg2"));
```

Rappelez-vous : `err_exit` appelle `exit()` et **saute par conséquent les destructors des objets sur la stack**. Si un quelconque objet sur la stack possède une ressource, préférez lancer une exception.

---

## 20. TL;DR

- Trois outils : les **codes de retour** pour les chemins simples, les **flags de stream** pour les E/S, les **exceptions** pour les erreurs profondes ou dans les constructors.
- Lancez des objets dérivant de `std::exception`. Préférez la classe fille spécifique appropriée (`out_of_range`, `invalid_argument`, `runtime_error`, ...).
- Retenez la distinction : **`logic_error`** = bug du programmeur, **`runtime_error`** = échec externe.
- Attrapez toujours par `const&`.
- Ordonnez les catch du plus spécifique au plus général.
- Les destructors ne doivent pas throw.
- Ayez toujours un `catch (const std::exception&)` de premier niveau dans le `main`.
- Associez les exceptions au **RAII** — c'est de là que provient leur réelle puissance.
- `exit()` saute l'unwinding ; les exceptions et `return` ne le font pas.