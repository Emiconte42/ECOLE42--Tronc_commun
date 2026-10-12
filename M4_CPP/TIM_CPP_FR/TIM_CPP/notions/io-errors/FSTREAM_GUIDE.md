# La bibliothèque `<fstream>` — Guide complet

Le header `<fstream>` fournit des classes de flux de fichiers (file stream) pour lire et écrire dans des fichiers. Il s'appuie sur `<iostream>`.

## Hiérarchie des classes

```
ios_base
   └── ios
         ├── istream ──┐
         │             ├── iostream
         └── ostream ──┘
              │
              ├── ifstream    (input file stream)
              ├── ofstream    (output file stream)
              └── fstream     (input/output file stream)
```

## Les trois classes principales

| Class      | Rôle                           | Mode par défaut        |
|------------|--------------------------------|------------------------|
| `ifstream` | Lire des fichiers              | `ios::in`              |
| `ofstream` | Écrire dans des fichiers       | `ios::out \| ios::trunc` |
| `fstream`  | Lire et écrire dans des fichiers | `ios::in \| ios::out`    |

---

## Constructeurs

```cpp
std::ifstream();                                          // Par défaut
std::ifstream(const char* filename,
              std::ios_base::openmode mode = ios::in);    // Ouverture à la construction
std::ifstream(const std::string& filename,
              std::ios_base::openmode mode = ios::in);    // C++11
```

Le même modèle existe pour `ofstream` et `fstream` (avec leurs propres modes par défaut).

---

## Modes d'ouverture (`std::ios_base::openmode`)

| Flag          | Signification                                 |
| ------------- | --------------------------------------------- |
| `ios::in`     | Ouvrir pour la lecture                        |
| `ios::out`    | Ouvrir pour l'écriture                        |
| `ios::app`    | Append — toutes les écritures se font à la fin du fichier |
| `ios::ate`    | Se positionner à la fin du fichier immédiatement après l'ouverture |
| `ios::trunc`  | Supprime le contenu du fichier s'il existe    |
| `ios::binary` | Ouvrir en mode binaire (aucune conversion de texte) |

À combiner avec un OU binaire (bitwise OR) : `std::ios::in | std::ios::binary`

---

## Fonctions membres

### Ouverture / Fermeture

| Function                                        | Description                                |
|-------------------------------------------------|--------------------------------------------|
| `open(filename, mode)`                          | Ouvrir un fichier sur un stream déjà construit |
| `close()`                                       | Fermer le fichier                          |
| `is_open()`                                     | Retourne `true` si un fichier est actuellement ouvert |

```cpp
std::ifstream file;
file.open("data.txt");
if (file.is_open()) { /* ... */ }
file.close();
```

### Lecture (depuis `istream` / `ifstream`)

| Function                          | Description                                            |
|-----------------------------------|--------------------------------------------------------|
| `operator>>`                      | Extraction formatée (délimitée par les espaces)       |
| `get()`                           | Lire un seul caractère                                 |
| `get(char& c)`                    | Lire un caractère dans `c`                             |
| `get(char* s, streamsize n)`      | Lire jusqu'à `n-1` caractères, s'arrête à `\n`         |
| `getline(char* s, streamsize n)`  | Comme `get`, mais consomme le `\n`                     |
| `read(char* s, streamsize n)`     | Lecture binaire brute de `n` octets                    |
| `readsome(char* s, streamsize n)` | Lire ce qui est immédiatement disponible               |
| `peek()`                          | Regarder le caractère suivant sans l'extraire          |
| `unget()`                         | Remettre le dernier caractère extrait dans le stream   |
| `putback(char c)`                 | Remettre un caractère spécifique dans le stream        |
| `ignore(n, delim)`                | Ignorer jusqu'à `n` caractères, ou jusqu'à ce que `delim` soit atteint |
| `gcount()`                        | Nombre de caractères extraits par la dernière lecture non formatée |

Fonction libre à connaître :
```cpp
std::getline(std::istream& is, std::string& str, char delim = '\n');
```

### Écriture (depuis `ostream` / `ofstream`)

| Function                          | Description                          |
|-----------------------------------|--------------------------------------|
| `operator<<`                      | Insertion formatée                   |
| `put(char c)`                     | Écrire un seul caractère             |
| `write(const char* s, streamsize n)` | Écriture binaire brute de `n` octets |
| `flush()`                         | Forcer le vidage du buffer vers le fichier |

### Positionnement (seek / tell)

| Function          | Description                                      |
|-------------------|--------------------------------------------------|
| `tellg()`         | Obtenir la position de lecture courante          |
| `tellp()`         | Obtenir la position d'écriture courante         |
| `seekg(pos)`      | Définir la position de lecture (absolue)         |
| `seekg(off, dir)` | Définir la position de lecture (relative)        |
| `seekp(pos)`      | Définir la position d'écriture (absolue)        |
| `seekp(off, dir)` | Définir la position d'écriture (relative)       |

Valeurs de direction :
- `std::ios::beg` — depuis le début
- `std::ios::cur` — depuis la position actuelle
- `std::ios::end` — depuis la fin

### Flags d'état

| Function       | Description                                         |
|----------------|-----------------------------------------------------|
| `good()`       | Aucune erreur                                       |
| `eof()`        | Fin de fichier atteinte                             |
| `fail()`       | Erreur logique (ex. mauvais format) ou erreur d'E/S |
| `bad()`        | Erreur d'E/S critique                               |
| `rdstate()`    | Obtenir les flags d'état bruts                      |
| `setstate(s)`  | Définir les flags d'état                             |
| `clear(s = goodbit)` | Effacer / réinitialiser les flags d'état       |
| `operator!`    | Équivalent à `fail()`                               |
| `operator bool`| Équivalent à `!fail()`                              |

### Buffer / Divers

| Function          | Description                               |
|-------------------|-------------------------------------------|
| `rdbuf()`         | Obtenir/définir le `filebuf` sous-jacent   |
| `swap(other)`     | Échanger deux streams (C++11)             |
| `tie()`           | Obtenir/définir le stream lié (ex. flusher `cout` avant de lire sur `cin`) |

---

## Cas d'usage typiques

### Lire un fichier ligne par ligne
```cpp
std::ifstream file("input.txt");
if (!file)
    return 1;

std::string line;
while (std::getline(file, line))
    std::cout << line << '\n';
```

### Écrire dans un fichier
```cpp
std::ofstream out("output.txt");
if (!out)
    return 1;

out << "Hello " << 42 << '\n';
```

### Lecture/écriture (modification sur place)
```cpp
std::fstream f("data.bin", std::ios::in | std::ios::out | std::ios::binary);
```

### E/S binaire
```cpp
std::ofstream out("data.bin", std::ios::binary);
int n = 42;
out.write(reinterpret_cast<const char*>(&n), sizeof(n));

std::ifstream in("data.bin", std::ios::binary);
int m;
in.read(reinterpret_cast<char*>(&m), sizeof(m));
```

### Obtenir la taille d'un fichier
```cpp
std::ifstream f("file.txt", std::ios::ate | std::ios::binary);
std::streamsize size = f.tellg();
f.seekg(0, std::ios::beg);
```

---

## Créer des fichiers qui n'existent pas encore

### Avec `ofstream` (la méthode la plus simple)

`ofstream` avec `ios::out` (la valeur par défaut) crée le fichier s'il n'existe pas, et le tronque s'il existe déjà :

```cpp
std::ofstream out("newfile.txt");   // créé si absent, tronqué si présent
if (!out)
    return 1;                        // vérification : le répertoire peut ne pas exister, ou permissions insuffisantes
out << "content\n";
```

### Ajouter sans tronquer (Append)

Utilisez `ios::app` pour créer le fichier s'il est absent et écrire à la fin s'il existe déjà :

```cpp
std::ofstream out("log.txt", std::ios::app);
out << "new line\n";
```

### Avec `fstream` (lecture + écriture sur un nouveau fichier)

`fstream` avec `ios::in | ios::out` ne créera **pas** le fichier — l'opération échoue s'il n'existe pas.
Pour le créer s'il est absent puis lire/écrire, combinez avec `ios::trunc` ou ouvrez-le d'abord avec `ofstream` :

```cpp
// Option 1 : créer avec ofstream, réouvrir avec fstream
{
    std::ofstream create("data.txt");   // crée le fichier
}
std::fstream f("data.txt", std::ios::in | std::ios::out);

// Option 2 : trunc crée le fichier (mais écrase tout contenu existant)
std::fstream f("data.txt", std::ios::in | std::ios::out | std::ios::trunc);
```

### Vérifier si un fichier existe avant de le créer

```cpp
std::ifstream probe("file.txt");
if (!probe)
    std::ofstream("file.txt");   // le crée vide
```

### Ce qui peut échouer même si le chemin semble correct

- Le **répertoire n'existe pas** — `fstream` ne créera pas les répertoires intermédiaires.
- **Pas de permission d'écriture** sur le répertoire.
- Le chemin est un répertoire, pas un fichier.

Vérifiez toujours `is_open()` ou `operator bool` après l'ouverture.

---

## Pièges courants

- **Vérifiez toujours le stream** après l'ouverture (`if (!file) ...`) — le constructeur ne lèvera pas d'exception en cas d'échec.
- **`eof()` n'est activé qu'après** une tentative de lecture échouée au-delà de la fin, pas lorsque le dernier octet est lu. Ne bouclez pas avec `while (!file.eof())`.
- **Les destructeurs ferment le fichier** automatiquement (RAII), un appel explicite à `close()` est donc généralement inutile.
- **Mélanger `>>` et `getline`** laisse un `\n` dans le buffer — utilisez `ignore()` entre les deux.
- **`ofstream` tronque par défaut** — utilisez `ios::app` pour ajouter du contenu à la fin.