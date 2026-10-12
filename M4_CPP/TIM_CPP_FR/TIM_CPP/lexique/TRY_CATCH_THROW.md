# `try` / `catch` / `throw` — Gestion des exceptions

> **TL;DR.** `throw` lève une exception (une valeur transportant des informations d'erreur) ; `try` délimite un bloc où des exceptions peuvent être levées ; `catch` énumère les gestionnaires qui correspondent aux types d'exception. Le runtime remonte la call stack — en détruisant les objets dans le scope (stack unwinding) — jusqu'à trouver un catch correspondant. Explications détaillées dans [`ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md).

Lié : [`ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. La forme

```cpp
try {
    // code that may throw
    if (problem)
        throw std::runtime_error("something broke");
}
catch (const std::runtime_error &e) {
    std::cerr << "runtime error: " << e.what() << '\n';
}
catch (const std::exception &e) {
    std::cerr << "other exception: " << e.what() << '\n';
}
catch (...) {
    std::cerr << "totally unknown error\n";
}
```

Trois éléments :

```
   try     "J'ai peur que quelque chose en dessous de moi lance une exception."
   throw   "Quelque chose ne va pas ; abandonne ce chemin d'exécution avec cette erreur."
   catch   "Si quelque chose en dessous a lancé un type correspondant, je le gère."
```

---

## 2. Le flux de contrôle — stack unwinding

Lorsque `throw` s'exécute :

```
   1. L'objet lancé est construit (c'est une vraie valeur, copiée/déplacée dans un emplacement géré par le runtime).
   2. La fonction courante s'interrompt au point de throw.
   3. Les objets locaux dans le scope sont détruits dans l'ordre inverse — les destructors s'exécutent.
   4. Le contrôle revient à l'appelant, qui effectue aussi son unwinding — les destructors s'y exécutent également.
   5. La remontée de la stack continue jusqu'à trouver un catch correspondant.
   6. Si aucune correspondance n'est trouvée — std::terminate est appelé → fin du programme.
```

```
                 throw point
   ┌───────────────────────────────────────────────┐
   │ caller A    locals: F1 F2                      │
   │  ┌─────────────────────────────────────────┐  │
   │  │ caller B    locals: G1                  │  │
   │  │  ┌────────────────────────────────────┐ │  │
   │  │  │ caller C    locals: H1 H2 H3       │ │  │
   │  │  │   throw expr;       ◄───── here    │ │  │
   │  │  └────────────────────────────────────┘ │  │
   │  │  destructors: ~H3, ~H2, ~H1 (reverse)   │  │
   │  └─────────────────────────────────────────┘  │
   │  destructors: ~G1                              │
   │                                                │
   │  ◄── catch found here? handle.                 │
   │      no? continue → caller A's destructors,    │
   │      then up to its caller, etc.               │
   └───────────────────────────────────────────────┘
```

C'est le **stack unwinding**. Le RAII (ressources détenues par des objets sur la stack avec des destructors) prend tout son sens ici — vos descripteurs de fichiers, verrous de mutex, allocations sont tous nettoyés automatiquement.

---

## 3. Que peut-on lancer ?

**N'importe quoi.** Tout objet copiable.

```cpp
throw 42;                              // an int — please don't
throw "hello";                         // const char * — please don't
throw std::string("oops");             // a string — please don't
throw std::runtime_error("bad input"); // ✓ exception class — yes
```

Convention : lancer des types dérivés de `std::exception`, ce qui garantit la présence d'une méthode `what()` :

```cpp
class std::exception {
public:
    virtual const char* what() const throw();
};
```

La bibliothèque standard fournit :

```
   std::exception
       ├── std::logic_error
       │       ├── std::invalid_argument
       │       ├── std::domain_error
       │       ├── std::length_error
       │       └── std::out_of_range
       ├── std::runtime_error
       │       ├── std::range_error
       │       ├── std::overflow_error
       │       └── std::underflow_error
       └── std::bad_alloc, std::bad_cast, std::bad_typeid, ...
```

Pour les modules de 42, vous ferez généralement :

```cpp
class MyClass {
public:
    class MyException : public std::exception {
    public:
        const char *what() const throw() { return "MyClass error"; }
    };
};

if (bad)
    throw MyClass::MyException();
```

Une classe d'exception imbriquée à l'intérieur de la classe qui la lance. Le pattern standard de la norme 42.

---

## 4. Correspondance des catch

Les blocs catch sont testés dans l'ordre. Le premier vers lequel le type de l'objet lancé est implicitement convertible l'emporte.

```cpp
try { throw std::out_of_range("x"); }
catch (const std::logic_error& e)  { /* matches — out_of_range derives from logic_error */ }
catch (const std::exception&  e)  { /* never reached, even though it would also match */ }
```

Il faut donc **lister les types spécifiques en premier, les types généraux ensuite**. L'ordre a son importance.

### `catch (...)` — attraper n'importe quoi

```cpp
catch (...) {
    // catches anything not caught above; you can't inspect the exception object here
}
```

À utiliser avec parcimonie — généralement comme nettoyage de dernier recours, souvent immédiatement suivi de `throw;` pour relancer l'exception :

```cpp
try { risky(); }
catch (...) {
    cleanup();
    throw;            // re-throw the current exception, preserving its type
}
```

### Catch par reference — presque toujours

```cpp
catch (const std::exception& e)    // ✓ — reference, no slicing
catch (std::exception e)           // ✗ — by value, slices any derived class
```

Si vous attrapez par valeur, les parties dérivées de l'objet d'exception sont tronquées (object slicing) — le même problème que le polymorphisme avec la sémantique de valeur. **Attrapez toujours par const reference** pour les exceptions de type classe.

---

## 5. Vue matériel/runtime — le coût des exceptions

La gestion des exceptions en C++ sur les compilers modernes (gcc/clang) utilise le **table-based unwinding** (aussi appelé zero-cost exceptions sur le chemin normal) :

```
   le compiler émet, aux côtés de chaque fonction :
     • le code machine de la fonction (.text)
     • une table d'unwind (.eh_frame)
       ─ décrit comment retrouver les frame pointers et destructors
         à chaque instruction dans la fonction

   pas de try → aucune instruction ajoutée au corps de la fonction
   pas de throw → aucun coût à l'exécution
   throw → le runtime parcourt .eh_frame, exécute les destructors, transfère au gestionnaire
```

Conséquences clés :

- **Le chemin sans exception a un overhead nul** sur x86_64 moderne.
- **Lancer une exception est coûteux** — le runtime doit chercher dans les tables, exécuter les destructors, configurer le contexte du gestionnaire. Des centaines de cycles au minimum, souvent des microsecondes.
- **La taille du binaire augmente** — les tables de eh ne sont pas gratuites.

N'utilisez pas les exceptions pour contrôler le flux d'exécution. Utilisez-les pour de réelles conditions exceptionnelles.

---

## 6. Spécifications d'exception (C++98 uniquement — dépréciées, puis supprimées)

```cpp
void f() throw();              // C++98: promises NOT to throw
void g() throw(std::exception); // C++98: promises to only throw types derived from std::exception
```

Si une fonction lance un type qu'elle n'a pas déclaré, `std::unexpected` est appelée → appelle généralement `std::terminate`.

En pratique :
- `throw()` est très répandu, en particulier sur les destructors.
- Les spécifications avec liste de types (`throw(X, Y)`) étaient rarement utilisées et ont été supprimées en C++17.

La norme 42 n'impose pas les spécifications d'exception, mais vous les verrez sur `what()` :

```cpp
class MyEx : public std::exception {
public:
    virtual const char* what() const throw() { return "..."; }
};
```

Le `throw()` après `const` déclare que cette méthode `what()` ne lance pas d'exception.

Le C++11 a déprécié `throw()` au profit de `noexcept`. Pas à 42.

---

## 7. Garanties de sécurité des exceptions

Lors de la conception de classes pouvant être impliquées avec du code susceptible de throw :

| Niveau | Promesse |
|---|---|
| **No-throw** | La fonction ne lance jamais d'exception. Obligatoire pour les destructors et `swap`. |
| **Strong** | Soit la fonction réussit, soit l'état du programme reste inchangé (transactionnel). |
| **Basic** | La fonction peut échouer, mais aucune ressource ne fuite et les invariants sont préservés. |
| **None** | Rien n'est garanti. |

Obtenez la garantie "basic" grâce au RAII — gérez les ressources via des objets dont les destructors nettoient automatiquement. Obtenez la garantie "strong" avec l'**idiome copy-and-swap** (construire le nouvel état à part, faire le swap en cas de succès).

```cpp
// strong-exception-safe operator=
MyClass& operator=(MyClass other) {        // copy made; if it throws, original untouched
    swap(*this, other);                     // no-throw swap
    return *this;
}                                           // 'other' destructs, cleaning up old data
```

---

## 8. Les destructors ne doivent pas throw

```cpp
class C {
public:
    ~C() {
        if (problem()) throw std::runtime_error("nope");   // BAD
    }
};
```

Si le destructor s'exécute pendant le stack unwinding (alors qu'une autre exception est déjà en cours de traitement), lancer une *seconde* exception appelle `std::terminate`. Irrécupérable.

Règle : **les destructors ne doivent pas laisser les exceptions se propager.** Soit ils ne lancent rien, soit ils attrapent l'exception en interne :

```cpp
~C() {
    try { cleanup(); }
    catch (...) { /* swallow */ }
}
```

---

## 9. Astuces & bonnes pratiques

### 9.1 Throw par valeur, catch par reference

```cpp
throw MyException();                // construct and throw a temporary
catch (const MyException& e)        // bind by const reference
```

### 9.2 Hériter de `std::exception`

Fournissez toujours un `what()` renvoyant un message explicite. Les outils (debuggers, loggers) s'y attendent.

### 9.3 Utiliser les exceptions pour des conditions **exceptionnelles**
Pour des situations comme « l'utilisateur a entré une mauvaise commande » dans votre annuaire, `if (badInput) printError()` convient parfaitement — pas besoin d'exceptions. Réservez les exceptions aux cas où le code local ne peut véritablement pas continuer et que l'appelant doit en être informé.

### 9.4 Ne faites pas de `catch (...)` sans rien faire

Avaler silencieusement les exceptions masque les bugs. Au minimum, loguez :

```cpp
catch (...) {
    std::cerr << "unknown error caught and ignored\n";
}
```

Ou relancez l'exception.

### 9.5 Relancez avec un simple `throw;`

```cpp
catch (const std::exception& e) {
    log(e.what());
    throw;             // préserve le type et le contenu de l'exception
}
```

`throw e;` provoquerait un slicing si `e` est du type de base — en ne copiant que la partie de base. Un simple `throw;` relance l'original.

### 9.6 Function-try-block — gérer les exceptions de ctor

```cpp
class C {
    int *_data;
public:
    C(int n) try : _data(new int[n]) {
        // corps...
    }
    catch (...) {
        // on peut nettoyer ici, mais impossible de supprimer l'exception ; le throw d'un ctor se propage toujours
    }
};
```

Principalement utilisé pour les throws dans la member-init list. Cas d'usage de niche.

### 9.7 Les blocs `try` ont leur propre portée (scope)

Les variables déclarées à l'intérieur de `try { … }` ne sont pas visibles après le bloc :

```cpp
try {
    int x = 5;
}
std::cout << x;        // ERREUR — x est hors de portée
```

Si vous avez besoin d'une valeur issue d'un bloc try, déclarez-la à l'extérieur.

---

## 10. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `terminate called after throwing an instance of …` | Exception levée mais non attrapée | Ajouter un `catch` correspondant, ou corriger le bug |
| Slicing dans `catch (Base e)` | Attrapé par valeur | Attraper par `const Base&` |
| `std::terminate` pendant le nettoyage | Un destructeur a levé une exception pendant le unwinding | Ne levez pas d'exception depuis les destructeurs |
| Fuite mémoire après une exception | `new` manuel sans wrapper RAII | Utiliser des objets sur le stack / des smart pointers / RAII |
| Ordre des catch inefficace | Catch plus général listé avant un plus spécifique | Réordonner : spécifique d'abord, général en dernier |

---

## 11. Résumé visuel

```
              ┌─────────────────────────────────────────┐
              │   try {                                  │
              │       throw X();   ← raises an exception │
              │   }                                       │
              │   catch (const X& e) { ... }              │
              │   catch (const std::exception& e) { ... } │
              │   catch (...) { ... }                     │
              └────────────────────┬────────────────────┘
                                   │
                                   ▼
              throw causes:
                1. construct exception object
                2. unwind stack — destructors run
                3. find matching catch (specific to general)
                4. transfer control to that catch
                5. no match? std::terminate.

              cost:
                non-throwing path: ~zero on modern x86_64
                throw: hundreds of cycles + destructor work
                binary size: +eh_frame tables

              rules of thumb:
                - throw classes derived from std::exception
                - throw by value, catch by const reference
                - destructors never throw
                - use for exceptional conditions, not control flow
                - RAII makes exception safety almost automatic
```

---

## 12. Pratique

1. Que se passe-t-il si une exception levée ne correspond à aucun `catch` ? *(`std::terminate` est appelé → fin du programme ; aucun autre destructeur ne s'exécute au-delà de ce que le unwinding avait déjà couvert.)*
2. Pourquoi les destructeurs ne doivent-ils pas lever d'exceptions ? *(Si un unwinding est déjà en cours pour une autre exception, deux exceptions simultanées provoquent un `std::terminate`.)*
3. Pourquoi `catch (const std::exception&)` est-il meilleur que `catch (std::exception)` ? *(Le passage par valeur découpe (slice) l'objet d'exception — les parties dérivées (y compris le véritable override de `what()`) sont perdues.)*
4. Quel est le coût à l'exécution de `try { ... }` si aucune exception n'est levée ? *(Pratiquement nul sur les implémentations de unwinding basées sur des tables — gcc/clang sur x86_64.)*