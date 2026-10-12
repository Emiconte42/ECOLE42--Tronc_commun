# `std::string` — Référence complète

`std::string` se trouve dans `<string>` et est un `typedef` pour `std::basic_string<char>`. Tout ce qui suit est en C++98, sauf indication contraire **(C++11)**.

## Modèle mental rapide

Un `std::string` possède un buffer de `char` alloué sur le heap et trois valeurs : **size** (`char` actuellement stockés), **capacity** (octets disponibles avant réallocation), et le pointer. Il garantit que le buffer reste terminé par un caractère nul afin que `c_str()` fonctionne en O(1).

---

## Construction & Affectation

| Fonction | Objectif |
|---|---|
| `string()` | Chaîne vide |
| `string(const string& s)` | Copie |
| `string(const string& s, size_t pos, size_t len = npos)` | Copie de sous-chaîne depuis `s[pos..pos+len]` |
| `string(const char* s)` | Depuis une C-string (lit jusqu'à `\0`) |
| `string(const char* s, size_t n)` | Les `n` premiers `char` de `s` (aucun `\0` requis) |
| `string(size_t n, char c)` | `n` copies de `c` |
| `string(InputIt first, InputIt last)` | Depuis une plage d'iterator |
| `operator=(const string&)` / `(const char*)` / `(char)` | Affectation |
| `assign(...)` | Mêmes overloads que les constructeurs ; remplace le contenu |

```cpp
std::string a;                  // ""
std::string b("hello");         // "hello"
std::string c(5, 'x');          // "xxxxx"
std::string d(b, 1, 3);         // "ell"
a.assign("world");              // a = "world"
```

---

## Accès aux éléments

| Fonction | Retourne | Remarques |
|---|---|---|
| `operator[](size_t i)` | `char&` | **Pas de vérification des bornes** ; UB si `i > size()` |
| `at(size_t i)` | `char&` | Lance `std::out_of_range` |
| `front()` **(C++11)** | `char&` | Premier `char` ; UB si vide |
| `back()` **(C++11)** | `char&` | Dernier `char` ; UB si vide |
| `data()` | `const char*` | Pointer vers le buffer interne (terminé par un caractère nul depuis C++11) |
| `c_str()` | `const char*` | Vue C-string terminée par un caractère nul |

```cpp
std::string s = "abc";
s[0];          // 'a'
s.at(10);      // lance une exception
s.c_str();     // "abc\0..."
```

---

## Iterators

| Fonction | Retourne |
|---|---|
| `begin()` / `end()` | `iterator` |
| `rbegin()` / `rend()` | `reverse_iterator` |
| `cbegin()` / `cend()` **(C++11)** | `const_iterator` |
| `crbegin()` / `crend()` **(C++11)** | `const_reverse_iterator` |

```cpp
for (std::string::iterator it = s.begin(); it != s.end(); ++it)
    std::cout << *it;
```

---

## Capacité

| Fonction | Retourne | Objectif |
|---|---|---|
| `size()` / `length()` | `size_t` | Nombre de `char` (identiques, les deux existent par compatibilité) |
| `empty()` | `bool` | `size() == 0` |
| `max_size()` | `size_t` | Taille maximale théorique (généralement énorme) |
| `capacity()` | `size_t` | Capacité actuellement allouée |
| `reserve(size_t n)` | `void` | Assure que capacity ≥ `n` (évite les réallocations) |
| `resize(size_t n)` | `void` | Définit la taille à `n` ; remplit avec `\0` ou un caractère donné |
| `resize(size_t n, char c)` | `void` | Identique mais remplit avec `c` |
| `shrink_to_fit()` **(C++11)** | `void` | Demande une réduction de la capacité |
| `clear()` | `void` | Définit la taille à 0 (ne libère pas la capacité) |

---

## Modificateurs

### Append

| Fonction | Objectif |
|---|---|
| `operator+=` | Accepte `string`, `char*`, `char` |
| `append(const string&)` | Concatène |
| `append(const string&, size_t pos, size_t len)` | Ajoute une sous-chaîne |
| `append(const char*)` / `append(const char*, size_t n)` | Depuis une C-string |
| `append(size_t n, char c)` | `n` copies de `c` |
| `append(InputIt first, InputIt last)` | Ajout d'une plage d'iterator |
| `push_back(char c)` | Ajoute un seul `char` |

### Insert

| Fonction | Objectif |
|---|---|
| `insert(size_t pos, const string& s)` | Insère `s` à `pos` |
| `insert(size_t pos, const char* s)` | Depuis une C-string |
| `insert(size_t pos, size_t n, char c)` | `n` copies de `c` |
| `insert(iterator p, char c)` | Insère au niveau de l'iterator |
| `insert(iterator p, size_t n, char c)` | Plusieurs `char` au niveau de l'iterator |
| `insert(iterator p, InputIt first, InputIt last)` | Plage au niveau de l'iterator |

### Erase

| Fonction | Objectif |
|---|---|
| `erase(size_t pos = 0, size_t len = npos)` | Supprime `[pos, pos+len)` |
| `erase(iterator p)` | Supprime le `char` à la position `p` |
| `erase(iterator first, iterator last)` | Supprime la plage |
| `pop_back()` **(C++11)** | Supprime le dernier `char` |

### Replace

| Fonction | Objectif |
|---|---|
| `replace(size_t pos, size_t len, const string& s)` | Remplace `[pos, pos+len)` par `s` |
| `replace(size_t pos, size_t len, const char* s, size_t n)` | Variante avec C-string |
| `replace(size_t pos, size_t len, size_t n, char c)` | Variante de remplissage |
| `replace(iterator i1, iterator i2, ...)` | Overloads avec iterator pour tout ce qui précède |

### Autre

| Fonction | Objectif |
|---|---|
| `swap(string& other)` | Swap en O(1) |
| `copy(char* dst, size_t len, size_t pos = 0)` | Copie dans un buffer `char*` ; **n'ajoute pas** de caractère nul de fin |

```cpp
std::string s = "hello";
s += " world";              // "hello world"
s.insert(5, ",");           // "hello, world"
s.erase(5, 1);              // "hello world"
s.replace(6, 5, "there");   // "hello there"
s.push_back('!');           // "hello there!"
```

---

## Opérations sur les chaînes

### Recherche

Toutes retournent un `size_t` — soit la position de correspondance, soit `std::string::npos` en cas d'échec.

| Fonction | Trouve |
|---|---|
| `find(const string& s, size_t pos = 0)` | Première occurrence de `s` à ou après `pos` |
| `find(const char* s, size_t pos = 0)` | Identique, C-string |
| `find(char c, size_t pos = 0)` | Première occurrence de `c` |
| `rfind(...)` | Dernière occurrence à ou avant `pos` |
| `find_first_of(const string& chars, size_t pos = 0)` | Premier `char` présent dans `chars` |
| `find_last_of(...)` | Dernier `char` présent dans `chars` |
| `find_first_not_of(...)` | Premier `char` **non** présent dans `chars` |
| `find_last_not_of(...)` | Dernier `char` **non** présent dans `chars` |

```cpp
std::string s = "hello world";
s.find("world");              // 6
s.find('z');                  // std::string::npos
s.find_first_of("aeiou");     // 1 ('e')
s.find_last_not_of(" ");      // 10 ('d')

if (s.find("xyz") == std::string::npos) { /* non trouvé */ }
```

### Extraction

| Fonction | Retourne |
|---|---|
| `substr(size_t pos = 0, size_t len = npos)` | Nouvelle copie `string` de `[pos, pos+len)` |

```cpp
std::string("hello").substr(1, 3);  // "ell"
```

### Comparaison

`compare(...)` retourne un `int` : `0` si égal, négatif si `*this` < argument, positif sinon.

| Fonction | |
|---|---|
| `compare(const string& s)` | Compare des chaînes entières |
| `compare(size_t pos, size_t len, const string& s)` | Compare une sous-chaîne avec `s` |
| `compare(size_t pos, size_t len, const string& s, size_t subpos, size_t sublen)` | Sous-chaîne contre sous-chaîne |
| `compare(const char* s)` | Contre une C-string |
| `compare(size_t pos, size_t len, const char* s, size_t n)` | Sous-chaîne contre les `n` premiers `char` d'une C-string |

Habituellement, les opérateurs `==`, `!=`, `<`, `>`, `<=`, `>=` sont préférés pour l'égalité ou l'ordre.

---

## Fonctions non-membres

| Fonction | Objectif |
|---|---|
| `operator+` | Concaténation (retourne un nouveau `string`) |
| `operator==`, `!=`, `<`, `>`, `<=`, `>=` | Comparaison lexicographique |
| `operator<<` | Écriture dans un `ostream` |
| `operator>>` | Lecture d'un token délimité par des espaces depuis un `istream` |
| `std::getline(istream& is, string& str, char delim = '\n')` | Lit une ligne entière (ou jusqu'à `delim`) |
| `std::swap(string&, string&)` | Swap en fonction libre |

### Conversions C++11

| Fonction | Objectif |
|---|---|
| `std::to_string(int/long/double/...)` | Nombre → string |
| `std::stoi`, `std::stol`, `std::stoll` | String → entier |
| `std::stof`, `std::stod`, `std::stold` | String → float |
| `std::stoul`, `std::stoull` | String → non signé |

```cpp
std::string line;
std::getline(std::cin, line);                // lit toute la ligne
std::string combined = "x = " + std::to_string(42);
```

---

## Constantes

| Constante | Valeur |
|---|---|
| `std::string::npos` | `static_cast<size_t>(-1)` — la sentinelle "non trouvé" / "jusqu'à la fin" |

---

## Pièges

- **`operator[]` ne vérifie pas les bornes** — utilisez `at()` si vous avez besoin de sécurité.
- **`c_str()` n'est valide que jusqu'à ce que la chaîne soit modifiée** — ne conservez pas le pointer sur le long terme.
- **`size()` retourne un `size_t` (non signé).** Une soustraction peut causer un underflow : `s.size() - 10` lorsque `s.size() < 10` donne un très grand nombre. Faites un cast vers un type signé ou vérifiez au préalable.
- **`find` retourne un `size_t` ; `-1` correspond à `npos`.** Comparez toujours à `std::string::npos`, jamais à `-1`.
- **`substr(pos)` avec `pos > size()` lance `std::out_of_range`.** `pos == size()` est autorisé (retourne une chaîne vide).
- **Les littéraux de chaîne sont des `const char[]`, pas des `std::string`.** Concaténer deux littéraux (`"a" + "b"`) correspond à de l'arithmétique de pointer et échoue à la compilation. Au moins l'un des opérandes doit être un `std::string`.