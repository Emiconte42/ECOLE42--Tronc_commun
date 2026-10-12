# File I/O en C++ — Open, Read, Write, Close

Ce guide est écrit pour C++98 (ce qu'utilise 42). Tout se trouve dans le header `<fstream>`, qui inclut indirectement `<iostream>` — mais incluez quand même explicitement les deux.

---

## Les trois stream classes

| Class | Rôle | Voyez-le comme |
|---|---|---|
| `std::ifstream` | **i**nput file stream | Lire un fichier |
| `std::ofstream` | **o**utput file stream | Écrire dans un fichier |
| `std::fstream` | bidirectionnel | Lire **et** écrire dans le même fichier |

Vous choisissez selon vos besoins. Pour l'ex04, vous voulez **un `ifstream` pour le fichier source** et **un `ofstream` pour le fichier de sortie** — pas un `fstream`, car vous ne modifiez pas la source.

Les trois héritent de `std::ios` et se comportent comme `std::cin` / `std::cout` pour la lecture/écriture. Si vous savez lire depuis `cin`, vous savez lire depuis un `ifstream` — mêmes opérateurs, mêmes idiomes.

---

## Ouvrir un fichier

Il existe deux manières équivalentes. **Privilégiez la forme avec constructeur** — c'est plus RAII, et la fermeture est automatique lorsque le stream sort de son scope.

### Forme avec constructeur (recommandée)
```cpp
#include <fstream>

std::ifstream in("input.txt");
std::ofstream out("output.txt");
```

### Forme avec `.open()`
```cpp
std::ifstream in;
in.open("input.txt");     // open later, e.g. inside an if
// ...
in.close();               // optional — destructor will do it
```

### Le piège du `const char*` en C++98

En C++98, les constructeurs de `ifstream` / `ofstream` et `.open()` prennent un **`const char*`**, et non un `std::string`. Si `filename` est un `std::string`, vous devez appeler `.c_str()` :

```cpp
std::string filename = "input.txt";

std::ifstream in(filename);           // ❌ compile error in C++98
std::ifstream in(filename.c_str());   // ✅ correct
```

C'est un piège classique sur l'ex04. Le C++11 a ajouté une overload acceptant un `std::string`, mais vous n'y avez pas accès.

---

## Modes d'ouverture (open modes)

Second argument du constructeur / `open()`. Ce sont des flags dans `std::ios`, combinés avec `|`.

| Flag | Signification |
|---|---|
| `std::ios::in` | Ouvrir en lecture (par défaut pour `ifstream`) |
| `std::ios::out` | Ouvrir en écriture (par défaut pour `ofstream`) |
| `std::ios::app` | **Append** — toutes les écritures vont à la fin du fichier |
| `std::ios::ate` | "at end" — ouvre et positionne le curseur à la fin, mais les écritures peuvent toujours aller n'importe où |
| `std::ios::trunc` | Truncate — efface le contenu du fichier à l'ouverture (par défaut pour `ofstream`) |
| `std::ios::binary` | Mode binaire (pas de conversion de texte — pertinent sous Windows, noop sous Linux) |

```cpp
std::ofstream log("app.log", std::ios::app);           // append to log
std::ifstream bin("data.bin", std::ios::binary);        // binary read
std::ofstream out("x.txt", std::ios::out | std::ios::trunc);  // explicit clear
```

Pour les exercices de 42, vous avez rarement besoin d'autre chose que des valeurs par défaut. Pour l'ex04, les open modes par défaut sont parfaits.

---

## Vérifiez toujours si le fichier est ouvert

**Toute ouverture de fichier peut échouer** (le fichier n'existe pas, pas de permissions, mauvais chemin, etc.). Les streams se convertissent en `bool` — `false` signifie "état invalide". Vérifiez-le **systématiquement**.

Trois approches, de la plus simple à la plus informative. Choisissez selon le niveau de détail d'erreur requis par votre programme.

### Approche A — Minimum viable (simple vérification et sortie)

```cpp
std::ifstream in(filename.c_str());
if (!in) {
    std::cerr << "Error: could not open " << filename << std::endl;
    return 1;
}
```

`!in` renvoie `true` si le stream est dans un mauvais état — ce qui couvre "le fichier n'existe pas", "pas de permission", "le chemin est un dossier", etc. Largement suffisant pour 42.

### Approche B — Légèrement plus explicite (`is_open()`)

```cpp
std::ifstream in(filename.c_str());
if (!in.is_open()) {
    std::cerr << "Error: could not open " << filename << std::endl;
    return 1;
}
```

Même effet en pratique. Certains relecteurs / évaluateurs préfèrent `is_open()` car l'intention est plus claire : *"le fichier est-il ouvert ?"* plutôt que l'idiome plus cryptique `!in`.

### Approche C — Avec la raison au niveau de l'OS (`errno` + `strerror`)

```cpp
#include <cerrno>
#include <cstring>

std::ifstream in(filename.c_str());
if (!in) {
    std::cerr << "Error opening " << filename << ": " << std::strerror(errno) << std::endl;
    return 1;
}
```

Génère des messages comme :
```
Error opening foo.txt: No such file or directory
Error opening /root/secret: Permission denied
```

Utile pour de vrais programmes. Attention : C++98 ne garantit pas (*guarantee*) que `ifstream` renseigne `errno` — c'est implementation-defined. Sous Linux avec g++/clang, cela fonctionne, donc pour 42 c'est bon.

### `!in` vs `!in.is_open()` — quelle est la différence ?

- `!in` → `true` si le stream est dans **n'importe quel** mauvais état (non ouvert, ou ouvert mais EOF, ou erreur de lecture, etc.)
- `!in.is_open()` → vérifie strictement si le file handle est ouvert

**Juste après l'ouverture**, les deux conviennent. Pour **les vérifications d'erreur en cours de lecture**, préférez l'idiome `!in` (ou vérifiez explicitement `fail()`, `eof()`, `bad()` — voir plus bas).

### Écriture — même approche

```cpp
std::ofstream out(output.c_str());
if (!out) {
    std::cerr << "Error: could not create " << output << std::endl;
    return 1;
}
```

`ofstream` peut échouer si le dossier n'existe pas, que le disque est plein, ou si vous n'avez pas la permission d'écrire.

### Se prémunir contre les échecs d'écriture *après l'ouverture*

L'ouverture peut réussir, mais des écritures individuelles (ou le flush final) peuvent échouer plus tard :

```cpp
std::ofstream out(filename.c_str());
if (!out) { /* handle */ return 1; }

out << content;
out.close();         // force flush

if (!out) {
    std::cerr << "Error: write failed for " << filename << std::endl;
    return 1;
}
```

Cela permet d'attraper les erreurs de disque plein qui n'apparaissent qu'au moment du flush. C'est excessif pour l'ex04, mais bon à savoir.

### L'antipattern à éviter

```cpp
std::ifstream in(filename.c_str());
std::string line;
while (std::getline(in, line)) { /* ... */ }   // silently does nothing if file didn't open
```

Pas de vérification explicite → pas de message d'erreur → l'utilisateur se retrouve avec un fichier de sortie vide sans savoir pourquoi. **Vérifiez toujours immédiatement après l'ouverture.**

---

## Lire un fichier — les idiomes principaux

Choisissez celui qui correspond à votre tâche. Ils sont classés du plus courant au moins courant.

### Approche 1 — Lire ligne par ligne (`std::getline`)

La meilleure option quand le fichier est orienté ligne et que vous traitez chaque ligne indépendamment.

```cpp
std::ifstream in("input.txt");
std::string line;
while (std::getline(in, line)) {
    std::cout << line << std::endl;   // process `line`
}
```

- La condition de la boucle `while` est `std::getline(...)` → renvoie le stream, qui se convertit en `bool`. La boucle se termine sur EOF ou en cas d'erreur.
- Le `\n` final est **retiré** de `line`. Si vous le réécrivez, ajoutez-le vous-même.
- Fonctionne avec n'importe quel délimiteur : `std::getline(in, line, ',')` lit façon CSV.

### Approche 2 — Lire mot par mot (`operator>>`)

Idéal quand le fichier est délimité par des espaces (nombres, tokens, etc.).

```cpp
std::ifstream in("numbers.txt");
int n;
while (in >> n) {
    total += n;
}
```

- `operator>>` ignore les espaces initiaux (whitespace) par défaut.
- Fonctionne pour `int`, `double`, `std::string`, etc.
- Échoue (la boucle se termine) sur EOF **ou** en cas d'incohérence de format (ex. : lecture d'un `int` mais présence de `"abc"`).

### Approche 3 — Lire tout le fichier dans un seul string (`rdbuf()`)

La meilleure solution quand vous devez traiter le fichier comme un bloc unique — comme pour l'ex04, où une correspondance peut s'étendre sur plusieurs sauts de ligne.

```cpp
#include <fstream>
#include <sstream>

std::ifstream in(filename.c_str());
std::stringstream ss;
ss << in.rdbuf();              // dump entire file buffer into the stringstream
std::string content = ss.str();
```

**Pourquoi cela fonctionne :** `rdbuf()` renvoie un pointer vers le buffer sous-jacent du stream. L'opérateur `<<` sur un stream accepte un `streambuf*` et le lit jusqu'au bout.

C'est idiomatique en C++98 et c'est **la** méthode pour ingérer (slurp) un fichier entier. Ne réinventez pas la roue avec une boucle octet par octet.

### Approche 4 — Caractère par caractère (`get()`, `peek()`)

Rare à votre niveau, mais utile pour les parseurs.

```cpp
std::ifstream in("input.txt");
char c;
while (in.get(c)) {
    // process c
}
```

- `in.get()` sans argument renvoie un `int` (pour distinguer EOF, qui vaut -1).
- `in.peek()` renvoie le caractère suivant sans le consommer.

### Approche 5 — Lecture binaire de taille fixe (`read()`)

Pour les fichiers binaires.

```cpp
std::ifstream in("data.bin", std::ios::binary);
char buffer[1024];
in.read(buffer, sizeof(buffer));
std::streamsize bytesRead = in.gcount();   // how many bytes were actually read
```

---

## Écrire dans un fichier

Miroir de la lecture. Utilisez `operator<<` tout comme avec `std::cout`.

```cpp
std::ofstream out("output.txt");
if (!out) { /* error */ }

out << "Hello, world" << std::endl;
out << 42 << " " << 3.14 << std::endl;
```

- `std::endl` vide le buffer (**flush**) **et** écrit `\n`. Pour une vitesse maximale, utilisez `"\n"` — cela n'effectue pas de flush, donc les écritures restent bufferisées.
- `operator<<` gère les strings, les nombres, et tout ce que `std::cout` sait traiter.

### La boucle de copie pour l'ex04
```cpp
std::ofstream out(output_filename.c_str());
out << transformed_content;   // assuming you built `transformed_content` as std::string
```

---

## Fermer un fichier

### Vous n'avez généralement pas besoin d'appeler `close()`

Lorsqu'un `ifstream` ou `ofstream` sort de son scope, son destructeur ferme automatiquement le fichier. C'est le principe du **RAII** — resource acquisition is initialization. Reposez-vous dessus.
```cpp
{
    std::ifstream in("input.txt");
    // ... utiliser in ...
}   // `in` est détruit ici, le fichier est fermé automatiquement
```

### Quand appeler `close()` explicitement

- Vous souhaitez **rouvrir** le même stream avec un fichier différent.
- Vous devez **flush et vérifier** que les écritures ont réussi avant la fin du scope :
  ```cpp
  out.close();
  if (!out) {
      std::cerr << "Write failed" << std::endl;
  }
  ```
- Vous êtes en train d'écrire et souhaitez **libérer le handle dès que possible** (un autre processus attend de le lire).

### Ce que fait `close()`
- Flush toutes les écritures bufferisées vers le disque.
- Libère le file handle de l'OS.
- Place le stream dans un état "non ouvert" (mais n'efface pas les flags d'erreur — appelez `in.clear()` si vous souhaitez réutiliser l'objet stream).

---

## Flags d'état des streams

Chaque stream suit son propre état. Quatre flags, quatre vérifications :

| État | `goodbit` | `eofbit` | `failbit` | `badbit` |
|---|---|---|---|---|
| Vérifier avec | `good()` | `eof()` | `fail()` | `bad()` |
| Signification | Tout va bien | Fin de fichier atteinte | La lecture a échoué (format incompatible, lecture partielle) | Erreur catastrophique (matériel, OS) |

```cpp
if (in.eof())   std::cout << "reached EOF";
if (in.fail())  std::cout << "read failed";
if (in.bad())   std::cout << "irrecoverable error";
if (in.good())  std::cout << "all fine";
```

**`operator!()` renvoie `fail() || bad()`.** Donc `if (!in)` signifie "quelque chose s'est mal passé" — généralement ce que vous voulez.

### Réinitialiser les flags
Dès qu'un stream passe en mauvais état (par ex. vous avez essayé de lire un `int` mais vous avez obtenu `"abc"`), **il reste dans cet état** jusqu'à ce que vous appeliez `clear()` :

```cpp
in.clear();              // réinitialise tous les flags à goodbit
in.clear(std::ios::eofbit);   // efface tout sauf eofbit
```

Ceci est important si vous souhaitez faire un `seekg` en arrière et relire après avoir atteint EOF.

---

## Seeking (positionnement à l'intérieur d'un fichier)

| Fonction | Rôle |
|---|---|
| `in.tellg()` | Obtenir la position de lecture actuelle (renvoie un `streampos`) |
| `in.seekg(pos)` | Se positionner à une position absolue |
| `in.seekg(off, std::ios::beg)` | Se positionner par rapport au début |
| `in.seekg(off, std::ios::cur)` | Se positionner par rapport à la position actuelle |
| `in.seekg(off, std::ios::end)` | Se positionner par rapport à la fin |
| `out.tellp()` / `out.seekp(...)` | Identique, pour les streams de sortie (position "put") |

Idiome courant — obtenir la taille d'un fichier :
```cpp
in.seekg(0, std::ios::end);
std::streamsize size = in.tellg();
in.seekg(0, std::ios::beg);   // retour au début pour lire réellement
```

Vous n'en avez généralement pas besoin dans l'ex04 — `rdbuf()` s'en charge pour vous.

---

## Le template pour l'exercice 04 de 42

Assemblage pour "*lire le fichier, transformer, écrire dans fichier.replace*" :

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <s1> <s2>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string s1 = argv[2];
    std::string s2 = argv[3];

    if (s1.empty()) {
        std::cerr << "Error: s1 must not be empty" << std::endl;
        return 1;
    }

    // --- Lire le fichier complet ---
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "Error: cannot open " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();   // optionnel mais explicite

    // --- Transformer (remplacement manuel puisque std::string::replace est interdit) ---
    std::string result;
    size_t i = 0;
    while (i < content.size()) {
        size_t found = content.find(s1, i);
        if (found == std::string::npos) {
            result += content.substr(i);
            break;
        }
        result += content.substr(i, found - i);
        result += s2;
        i = found + s1.size();
    }

    // --- Écrire ---
    std::ofstream out((filename + ".replace").c_str());
    if (!out) {
        std::cerr << "Error: cannot create output file" << std::endl;
        return 1;
    }
    out << result;
    // out est fermé automatiquement ici

    return 0;
}
```

---

## Pièges à retenir

1. **C++98 nécessite `.c_str()`** sur `std::string` lors du passage à `ifstream`/`ofstream`.
2. **Vérifiez toujours le stream après l'ouverture** — ne supposez pas que le fichier existe.
3. **`getline` supprime le `\n`** — si vous réémettez les lignes, rajoutez-le.
4. **Mélanger `>>` et `getline`** est dangereux : `>>` laisse des espaces/sauts de ligne résiduels dans le buffer, et le `getline` suivant lit une ligne vide. Après `cin >> n`, appelez `cin.ignore()` avant `getline`.
5. **`std::endl` flush**, `"\n"` ne le fait pas. Dans des boucles serrées qui écrivent dans un fichier, préférez `"\n"`.
6. **`eof()` n'est activé que lorsque vous essayez de lire au-delà de la fin** — ainsi, `while (!in.eof())` comme condition de boucle est un bug classique. Il effectue une lecture de trop au-delà de la fin avant de vérifier. Utilisez toujours `while (in >> x)` ou `while (std::getline(in, line))` à la place.
7. **Les streams ne throw pas par défaut.** En cas d'échec, ils activent `failbit` et refusent silencieusement les opérations ultérieures. Vérifiez `!in`.
8. **Un `open()` qui échoue laisse le stream dans un état invalide.** Vous pouvez appeler à nouveau `open()` sur le même objet après `clear()` + `close()`, mais repartir sur un objet neuf est plus propre.

---

## Vocabulaire minimal à mémoriser

Vous devriez être capable de reproduire ces éléments de mémoire :

```cpp
// Ouvrir + vérifier
std::ifstream in(filename.c_str());
if (!in) { /* error */ }

// Lire ligne par ligne
std::string line;
while (std::getline(in, line)) { /* ... */ }

// Lire le fichier complet
std::stringstream ss; ss << in.rdbuf();
std::string content = ss.str();

// Écrire
std::ofstream out(name.c_str());
out << "text\n";
```

Si vous maîtrisez ces quatre fragments sur le bout des doigts, vous pouvez gérer n'importe quel exercice d'I/O de fichier du niveau de 42.

---

## Lectures complémentaires

- [`<fstream>` sur cppreference](https://en.cppreference.com/w/cpp/header/fstream) — mettez-le en favori.
- Voir aussi [STRING_FUNCTIONS.md](STRING_FUNCTIONS.md) pour les opérations de `std::string` que vous combinerez avec les file I/O.