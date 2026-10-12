# `std::string` — Comment fonctionne réellement ce type

> **TL;DR.** Un `std::string` est un `basic_string<char>` : un objet qui **possède un buffer de `char` redimensionnable et terminé par un caractère nul**. Considérez-le comme un `vector<char>` garantissant un `'\0'` final pour que `c_str()` soit gratuit. Il gère sa propre mémoire — vous ne faites jamais de `new`/`delete` sur le buffer, et copier une chaîne effectue une copie profonde (deep copy) de son contenu. Sur votre machine (Apple Silicon → **libc++**), il utilise la **small-string optimization** : les chaînes courtes résident *à l'intérieur* de l'objet, sans aucune allocation sur le heap.

Articles liés : [`STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md) (aide-mémoire exhaustif de l'API) · [`VECTOR.md`](../containers/VECTOR.md) · [`MEMORY.md`](MEMORY.md) · [`REFERENCE.md`](REFERENCE.md) · [`BASICS.md`](BASICS.md)

Header : `#include <string>`

> Cette note est le guide explicatif du *fonctionnement interne*. Pour la **liste exhaustive des méthodes** (chaque overload de `find`/`insert`/`replace`/`compare`), consultez [`STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md). Ici, nous abordons le modèle mental, le layout mémoire, la croissance et les pièges propres au C++98.

---

## Table of Contents

1. [Modèle mental — ce qu'est réellement une chaîne](#1.%20Mental%20model%20%E2%80%94%20what%20a%20string%20really%20is)
2. [`std::string` vs `char*` — le pont avec le C](#2.%20std%3A%3Astring%20vs%20char%20%E2%80%94%20the%20C%20bridge)
3. [Le buffer : SSO vs COW](#3.%20The%20buffer%3A%20SSO%20vs%20COW)
4. [Comment elle grandit](#4.%20How%20it%20grows)
5. [Le terminateur nul, `c_str()` et `data()`](#5.%20The%20null%20terminator%2C%20c_str%28%29%20and%20data%28%29)
6. [Sémantique de valeur — les copies sont profondes](#6.%20Value%20semantics%20%E2%80%94%20copies%20are%20deep)
7. [Aide-mémoire sur la complexité](#7.%20Complexity%20cheat%20sheet)
8. [Invalidation — quand les pointeurs et itérateurs deviennent obsolètes](#8.%20Invalidation%20%E2%80%94%20when%20pointers%20and%20iterators%20go%20stale)
9. [Conseils d'efficacité](#9.%20Efficiency%20tips)
10. [Mises en garde sur C++98](#10.%20C%2B%2B98%20caveats)
11. [Pièges courants](#11.%20Gotchas)

---

## 1. Modèle mental — ce qu'est réellement une chaîne

`std::string` n'est ni un mot-clé ni un type primitif. Il s'agit d'une class ordinaire de la bibliothèque standard — un `typedef` :

```cpp
namespace std {
    typedef basic_string<char> string;   // grossomodo
}
```

Conceptuellement, l'objet contient trois éléments :

```
   ┌─────────────────────────────────────┐
   │  std::string                         │
   │    pointer  ──────────►  heap buffer │   "hello\0"
   │    size      = 5                     │
   │    capacity  = 22                    │
   └─────────────────────────────────────┘
```

- **size** — combien de `char` sont logiquement stockés (ce que renvoient `size()`/`length()`).
- **capacity** — combien de `char` peuvent être accueillis avant de devoir réallouer (`capacity()`).
- **pointer** — l'endroit où résident les octets.

C'est *exactement* le modèle du `vector<T>` de [`VECTOR.md`](../containers/VECTOR.md), spécialisé pour le type `char`, avec une promesse supplémentaire : **il y a toujours un `'\0'` juste après le dernier caractère**, ce qui permet de passer le buffer à une fonction C en O(1).

Tout l'intérêt : vous disposez d'un buffer de texte extensible qui **se nettoie tout seul**. Lorsque la chaîne sort du scope, son destructeur libère le buffer. C'est le principe du RAII (voir [`MEMORY.md`](MEMORY.md)) — la raison pour laquelle vous ne perdez jamais de mémoire avec un `std::string`, contrairement à un `char*` géré à la main en C.

---

## 2. `std::string` vs `char*` — le pont avec le C

Vous connaissez déjà la manière de faire en C. Comparez les deux approches :

| Tâche | C (`char*`) | C++ (`std::string`) |
|---|---|---|
| Déclarer + initialiser | `char *s = strdup("hi");` | `std::string s = "hi";` |
| Longueur | `strlen(s)` — scan en O(n) | `s.size()` — O(1), elle est stockée |
| Concaténer | `strcat` dans un buffer assez grand | `s += " there";` — grandit automatiquement |
| Comparer | `strcmp(a, b) == 0` | `a == b` |
| Copier | `malloc` + `strcpy`, penser à libérer | `std::string b = a;` — deep copy, libéré automatiquement |
| Libérer | `free(s);` (et jamais de double-free) | *rien* — le destructeur s'en charge |
| Sous-chaîne | arithmétique de pointer manuelle + terminateur nul | `s.substr(pos, len)` |

Deux aspects cessent d'être *votre problème* pour devenir *le problème de la bibliothèque* : la **propriété de la mémoire (memory ownership)** et le **suivi de la longueur**. C'est là toute la proposition de valeur. La contrepartie est qu'un `std::string` est un objet plus lourd (24 octets ici, contre un pointer de 8 octets) et peut posséder une allocation sur le heap.

Vous pouvez toujours revenir au C avec `s.c_str()` lors de l'appel à une API C (`open`, `execve`, etc. — pertinent dans webserv).

---

## 3. Le buffer : SSO vs COW

Voici la subtilité qui induit souvent en erreur : **le standard C++ ne précise pas la manière dont `std::string` doit stocker ses octets.** Deux stratégies historiques existent, et *celle que vous obtenez dépend de votre standard library, pas de `-std=c++98`.*

### Small-String Optimization (SSO) — ce que fait libc++ (votre machine)

L'objet string réserve un petit buffer inline *à l'intérieur de lui-même*. Les chaînes courtes y sont stockées, avec **zéro allocation sur le heap**. Ce n'est que lorsque le texte dépasse la taille de ce buffer inline qu'une allocation sur le heap a lieu.

Mesuré sur votre configuration (`c++ -std=c++98`, Apple Clang + libc++) :

```
sizeof(std::string) == 24 bytes
empty string capacity() == 22      ← 22 chars fit inline, no malloc
```

```
   Chaîne courte "hello" — réside ENTIÈREMENT dans l'objet de 24 octets :

   ┌───────────────────────────────────────────────┐
   │ h e l l o \0 . . . . . . . . . . . . . . . [sz]│   aucun heap, aucun new
   └───────────────────────────────────────────────┘
              (buffer inline, ~22 chars utilisables)

   Chaîne longue (>22 chars) — l'objet contient un pointeur vers le heap :

   ┌──────────────────────────┐
   │ ptr ─► ┌──────────────────────────────────┐   │
   │ size   │ this is a long string ...... \0   │   │  alloué sur le heap
   │ cap    └──────────────────────────────────┘   │
   └──────────────────────────┘
```

Conséquence : construire de petites chaînes (cas typique dans les modules C++) ne coûte souvent **absolument aucune allocation** — un atout majeur pour les performances, ce qui signifie également que valgrind n'affichera aucun bloc mémoire sur le heap pour celles-ci.

### Copy-On-Write (COW) — l'ancienne approche de libstdc++

Les anciennes versions de GCC/libstdc++ (ABI antérieure à C++11) utilisaient des chaînes avec compteur de références (reference-counted) et copy-on-write : copier une chaîne ne faisait qu'incrémenter un compteur de références et partager le buffer ; la copie profonde n'intervenait de manière différée qu'à la première écriture. Vous pouvez lire des mentions de ce comportement dans de vieux tutoriels. **libc++ n'a jamais fonctionné ainsi**, et libstdc++ moderne l'a abandonné. Vous n'avez pas ce comportement — mais en connaître le nom permet de comprendre pourquoi certains anciens conseils ("copier des strings est gratuit !") ne sont plus d'actualité.

> **Point didactique pour la soutenance :** si un évaluateur demande "est-ce que `std::string b = a;` alloue de la mémoire ?", la réponse honnête est *"cela dépend de l'implémentation et de la longueur — sur libc++, une chaîne courte est copiée inline sans allocation ; une chaîne longue effectue une deep copy du buffer sur le heap."* Cette nuance montre précisément que vous comprenez le fonctionnement réel du type plutôt que d'avoir simplement retenu un slogan.

---

## 4. Comment elle grandit

Lorsque vous effectuez un append au-delà de la `capacity()`, la chaîne suit le même mécanisme qu'un `vector` :

1. allouer un buffer plus grand,
2. copier les chars existants,
3. libérer l'ancien buffer,
4. mettre à jour pointer/size/capacity.

Croissance mesurée sur votre machine (en insérant des caractères un par un avec affichage de la capacité à chaque réallocation) :

```
22 → 47 → 95 → 191 → 383 → ...
```

Il s'agit d'une **croissance géométrique (~×2)**, arrondie afin que l'allocation s'aligne sur une frontière de 16 octets (libc++ rapporte `capacity = allocated_bytes - 1`, réservant un octet pour le `'\0'`). C'est cette croissance géométrique qui rend `push_back`/`+=` **amorti en O(1)** : la plupart des ajouts se résument à une écriture peu coûteuse, et la réallocation coûteuse occasionnelle est suffisamment rare pour s'équilibrer en moyenne.

Si vous connaissez la taille finale, indiquez-la à l'avance à la chaîne pour éviter le coût des réallocations :

```cpp
std::string s;
s.reserve(1024);        // une seule allocation, pas de réallocations ultérieures
for (int i = 0; i < 1000; ++i)
    s += 'x';           // ne réalloue jamais
```

`reserve(n)` ajuste la capacité à au moins `n` sans modifier la `size()`. Très pratique dans webserv lorsque vous accumulez le corps d'une réponse de longueur connue.

---

## 5. Le terminateur nul, `c_str()` et `data()`

Un `std::string` maintient un `'\0'` juste après son dernier caractère afin de pouvoir fournir un C-string valide aux anciennes API en O(1) :

```cpp
std::string path = "index.html";
int fd = open(path.c_str(), O_RDONLY);   // c_str() → const char*
```

Deux points d'accès, tous deux de type `const char*` en C++98 :

- **`c_str()`** — garanti terminé par un caractère nul. À utiliser pour les API C.
- **`data()`** — pointer vers les octets internes. En C++98, il n'était *pas* garanti qu'il se termine par un caractère nul (cette garantie a été introduite avec C++11) ; préférez `c_str()` lorsque vous avez besoin du `'\0'`.

Règle critique : **le pointer renvoyé n'est valide que jusqu'à la prochaine opération non-const sur la chaîne.** Tout `+=`, `insert`, `resize`, etc. peut réallouer la mémoire et laisser votre pointer dangling :

```cpp
const char *p = s.c_str();
s += "more";               // peut réallouer — p est maintenant DANGLING
std::cout << p;            // comportement indéfini (undefined behaviour)
```

Ne mettez jamais `c_str()` en cache ; appelez-le directement à l'endroit où vous en avez besoin.

Notez également : un `std::string` **peut contenir des octets `'\0'` intégrés** (il stocke une taille explicite, contrairement à une chaîne C). Cependant, `c_str()` semblera tronqué dès le premier `'\0'` aux yeux d'une fonction C. Gardez cela en tête si vous stockez des données binaires dans un string.
---

## 6. Sémantique de valeur — les copies sont profondes

`std::string` se comporte comme une valeur, pas comme une référence/handle :

```cpp
std::string a = "hello";
std::string b = a;     // b est une copie INDÉPENDANTE
b[0] = 'H';
std::cout << a;        // "hello"  — a reste inchangé
```

C'est l'opposé d'un `char*` en C, où `b = a` crée un alias vers le même buffer. Comme les copies sont profondes, **passer des strings par valeur copie l'intégralité du buffer** — passez donc par `const&` lorsque vous n'avez pas besoin d'une copie (voir [`REFERENCE.md`](REFERENCE.md)) :

```cpp
void log(const std::string& msg);   // pas de copie
void log(std::string msg);          // copie le buffer à chaque appel
```

C'est la raison pour laquelle chaque fonction dans les modules C++ qui ne fait que *lire* une string prend une `const std::string&`.

---

## 7. Tableau récapitulatif de complexité

| Opération | Complexité | Notes |
|---|---|---|
| `size()`, `length()`, `empty()` | O(1) | stocké, non scanné |
| `operator[]`, `at()` | O(1) | accès direct (random access) |
| `c_str()`, `data()` | O(1) | déjà terminé par un caractère nul |
| `push_back`, `+=`, `append` | O(1) amorti | O(n) lors de la réallocation |
| `insert`, `erase` (au milieu) | O(n) | décale la fin (tail) |
| `find`, `rfind`, `find_first_of` | O(n·m) | recherche naïve de sous-chaîne |
| `substr` | O(len) | alloue une nouvelle string |
| `compare`, `==`, `<` | O(n) | lexicographique |
| constructeur de copie / assignation | O(n) | copie profonde (O(1) si SSO courte) |
| `swap` | O(1) | échange les structures internes |

---

## 8. Invalidation — quand les pointeurs et les itérateurs deviennent obsolètes

Toute opération qui **modifie la taille ou la capacité** peut réallouer le buffer, ce qui invalide :

- les pointeurs/références issus de `operator[]`, `at()`, `front()`, `back()`,
- les pointeurs issus de `c_str()` / `data()`,
- tous les itérateurs.

```cpp
std::string s = "abc";
char& first = s[0];
s += "xyz...long enough to reallocate...";
first = 'Z';               // référence DANGLING — UB
```

Règle empirique : **considérez toute référence/pointeur/itérateur vers une string comme invalide après avoir muté cette string.** Même discipline que pour `vector` ([`VECTOR.md`](../containers/VECTOR.md) §8).

---

## 9. Conseils d'efficacité

1. **Passez par `const std::string&`** pour éviter les copies profondes (§6).
2. **Utilisez `reserve()` lorsque la taille finale est connue** pour éviter les réallocations répétées (§4).
3. **Préférez `+=` / `append` au chaînage de `operator+`** dans les boucles — `a + b + c` construit des temporaires ; `s += a; s += b;` réutilise un seul buffer.
4. **Utilisez `empty()` plutôt que `size() == 0`** — plus clair, et toujours en O(1).
5. **Comparez avec `==`, pas avec `strcmp(a.c_str(), b.c_str())`** — l'opérateur est direct et connaît les tailles.
6. **`swap` pour réduire l'espace alloué** : en C++98, il n'y a pas de `shrink_to_fit` ; `std::string(s).swap(s)` force une copie ajustée à la taille exacte (rarement nécessaire).

---

## 10. Pièges et restrictions de C++98

Le sujet 42 est **strictement en C++98**. Ces fonctionnalités pratiques courantes sont issues de **C++11+ et interdites** :

| Non disponible en C++98 | Faites ceci à la place |
|---|---|
| `std::to_string(n)` | `std::ostringstream oss; oss << n; oss.str();` |
| `std::stoi(s)` / `stol` / `stod` | `std::istringstream iss(s); iss >> n;` |
| `s.front()` / `s.back()` | `s[0]` / `s[s.size() - 1]` |
| `s.pop_back()` | `s.erase(s.size() - 1)` |
| range-`for` (`for (char c : s)`) | boucle classique avec itérateur ou index |
| `data()` garanti terminé par un caractère nul | utilisez `c_str()` |
| `s += {…}` / initialisation par accolades | simple `+=` / appels de constructeur |

La conversion nombre-vers-string et inversement passe par `<sstream>` — voir [`FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md).

---

## 11. Pièges à éviter

- **`operator[]` ne vérifie pas les limites** — être hors limites provoque un UB. Utilisez `at()` (qui lève `std::out_of_range`) lorsque l'index peut être invalide.
- **`find` renvoie un `size_t`, et "non trouvé" vaut `std::string::npos`** — comparez toujours à `std::string::npos`, jamais à `-1`.
- **`size()` est non signé.** `s.size() - 1` sur une string vide donne un très grand nombre, pas `-1`. Protégez avec `if (!s.empty())` avant de soustraire.
- **`substr(pos)` lève une exception si `pos > size()`** — mais `pos == size()` est valide et renvoie `""`.
- **Deux littéraux de chaîne ne peuvent pas être concaténés avec `+` :** `"a" + "b"` correspond à de l'arithmétique de pointeurs et ne compilera pas. Au moins un opérande doit être une `std::string` : `std::string("a") + "b"`.
- **Ne mettez pas `c_str()` en cache** au-delà d'une mutation — le pointeur devient dangling (§5).
- **Une copie est profonde et peut allouer.** Passer par valeur dans un chemin critique (hot path) copie silencieusement le buffer ; utilisez `const&`.

---

### Voir aussi

- [`STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md) — chaque méthode, regroupée et avec ses signatures.
- [`FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md) — `stringstream` pour convertir nombre↔string en C++98.
- [`VECTOR.md`](../containers/VECTOR.md) — même modèle de buffer/croissance, généralisé.
- [`REFERENCE.md`](REFERENCE.md) — pourquoi vous devez passer les strings par `const&`.