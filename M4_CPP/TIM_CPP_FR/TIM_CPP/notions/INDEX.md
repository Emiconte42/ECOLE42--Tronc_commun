# 🧠 NOTIONS — Concepts par Thème

La vue d'ensemble : chaque fichier explique un concept en profondeur — modèle mental, diagrammes, exemples concrets, pièges à éviter. Les mots-clés ont droit à leur zoom dédié dans [`../lexique/`](../lexique/INDEX.md) ; la lecture guidée par les exercices se trouve dans [`../projets/`](../projets/INDEX.md).

> 🌟 **Commencez ici :** [`PHILOSOPHY.md`](PHILOSOPHY.md) — les 5 piliers du C++ et l'état d'esprit pour écrire du code correct. À relire avant chaque nouveau module.

---

## 🧱 `fundamentals/` — la couche de base

| Fichier | Sujet |
|---|---|
| [`BASICS.md`](fundamentals/BASICS.md) | Syntaxe de base, unité de compilation, namespaces — la transition du C vers le C++. |
| [`CONTRACTS.md`](fundamentals/CONTRACTS.md) | Le modèle mental derrière `const`, `private`, `explicit` : des promesses garanties par le compiler. |
| [`REFERENCE.md`](fundamentals/REFERENCE.md) | References vs pointers, sémantique des lvalues. |
| [`MEMORY.md`](fundamentals/MEMORY.md) | Stack vs heap, `new`/`delete`, RAII, durée de vie, valgrind. |
| [`ALLOCATION_FAILURE.md`](fundamentals/ALLOCATION_FAILURE.md) | Quand `new` échoue — `bad_alloc` vs NULL, rouages internes d'`operator new`. |
| [`STRING.md`](fundamentals/STRING.md) | Exploration approfondie de `std::string` — SSO vs COW, croissance, la passerelle `char*`. |
| [`CMATH.md`](fundamentals/CMATH.md) | `<cmath>` — puissances, racines, trigonométrie, arrondis ; C++98 vs C++11. |

## 🧬 `oop/` — programmation orientée objet

| Fichier | Sujet |
|---|---|
| [`ORTHODOX_CANONICAL_FORM.md`](oop/ORTHODOX_CANONICAL_FORM.md) | Les quatre fonctions membres obligatoires + ctor par défaut. L'indispensable de 42. |
| [`GETTERS_SETTERS.md`](oop/GETTERS_SETTERS.md) | Modèles d'encapsulation. |
| [`OPERATOR_OVERLOADING.md`](oop/OPERATOR_OVERLOADING.md) | Opérateurs membres vs fonctions libres, règle de symétrie. |
| [`INHERITANCE.md`](oop/INHERITANCE.md) | Héritage public/protected/private, héritage en diamant. |
| [`POLYMORPHISM.md`](oop/POLYMORPHISM.md) | Virtual, vtables, fonctions virtuelles pures, classes abstraites. |

## ⚙️ `advanced/` — les concepts avancés

| Fichier | Sujet |
|---|---|
| [`TEMPLATES.md`](advanced/TEMPLATES.md) | Function + class templates, spécialisation. |
| [`STL.md`](advanced/STL.md) | Containers + iterators + algorithms — la vue d'ensemble. |
| [`ALGORITHMS.md`](advanced/ALGORITHMS.md) | Référence exhaustive de `<algorithm>`/`<numeric>`, annotée pour C++98. |
| [`MEMBER_FUNCTION_POINTERS.md`](advanced/MEMBER_FUNCTION_POINTERS.md) | `.*` / `->*`, tables de dispatch, le pattern Harl. |

*Les quatre casts ont été déplacés dans le lexique : [`lexique/CASTS.md`](../lexique/CASTS.md).*

## 📦 `containers/` — une page par container STL

Modèle mental + diagramme ASCII + API complète + complexité + invalidation d'iterators, pour chacun. **Hub : [`containers/INDEX.md`](containers/INDEX.md)** — `vector`, `list`, `deque`, `stack`, `queue`, `priority_queue`, `set`, `multiset`, `map`, `multimap`.

## 🌊 `io-errors/` — flux & gestion des erreurs

| Fichier | Sujet |
|---|---|
| [`FSTREAM_GUIDE.md`](io-errors/FSTREAM_GUIDE.md) | `ifstream`/`ofstream`/`stringstream`. |
| [`OPEN.md`](io-errors/OPEN.md) | Flags et modes d'ouverture de fichiers. |
| [`STRING_FUNCTIONS.md`](io-errors/STRING_FUNCTIONS.md) | Aide-mémoire de l'API `std::string` (zoom approfondi : [`fundamentals/STRING.md`](fundamentals/STRING.md)). |
| [`ERROR_MANAGEMENT.md`](io-errors/ERROR_MANAGEMENT.md) | Exceptions, `try`/`catch`, garanties de sécurité. |

## 🛠️ `tooling/` — build & édition de liens

| Fichier | Sujet |
|---|---|
| [`MAKEFILE_CPP.md`](tooling/MAKEFILE_CPP.md) | Recettes de Makefile C++, `-Wall -Wextra -Werror -std=c++98`. |
| [`LIBRARIES.md`](tooling/LIBRARIES.md) | Bibliothèques statiques vs partagées, ordre d'édition de liens. |

---

## 🧭 Ordres de lecture suggérés

- **Début de la piscine :** `PHILOSOPHY` → `fundamentals/BASICS` → `fundamentals/MEMORY` → `oop/ORTHODOX_CANONICAL_FORM`
- **Si l'héritage pose problème :** `oop/INHERITANCE` → `oop/POLYMORPHISM` → [`lexique/VIRTUAL.md`](../lexique/VIRTUAL.md)
- **À l'attaque du Mod08 / des containers :** `advanced/STL` → `containers/INDEX` → `advanced/ALGORITHMS`