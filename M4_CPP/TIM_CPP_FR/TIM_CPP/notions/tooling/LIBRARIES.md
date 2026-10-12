# `<iostream>` et `<iomanip>` — Référence de la bibliothèque

> **Focus sur les mots-clés :** [`BOOL`](../../lexique/BOOL.md) (pour `std::boolalpha`) · [`OPERATOR`](../../lexique/OPERATOR.md) (pour `<<`/`>>`)

Ce document résume la principale API destinée aux utilisateurs issue des headers standards C++ `<iostream>` et `<iomanip>`, accompagnée d'exemples pratiques pour chaque fonctionnalité.

Remarque :
- La bibliothèque standard C++ évolue selon les versions. Ce fichier se concentre sur l'API couramment disponible (du C++98 au C++ moderne).
- Certaines fonctions sont des fonctions membres des classes de flux (`std::istream`, `std::ostream`, `std::ios_base`, etc.).
- Le terme "manipulateur" désigne un utilitaire employé avec `<<` ou `>>` pour modifier le formatage ou le comportement.

---

## 1. `<iostream>`

`<iostream>` fournit les classes de flux, la gestion de l'état des flux et les objets de flux standard.

### 1.1 Objets de flux standard

| Objet        | Description                               |
|--------------|-------------------------------------------|
| `std::cin`   | Flux d'entrée standard (`istream`)        |
| `std::cout`  | Flux de sortie standard (`ostream`)       |
| `std::cerr`  | Flux d'erreur standard, non bufférisé     |
| `std::clog`  | Flux d'erreur/journalisation standard, bufférisé |

Variantes pour caractères larges : `std::wcin`, `std::wcout`, `std::wcerr`, `std::wclog`

```cpp
#include <iostream>

int main() {
    int n;
    std::cout << "Enter a number: ";   // print to stdout
    std::cin >> n;                     // read from stdin
    std::cerr << "Error: bad value\n"; // unbuffered, goes to stderr immediately
    std::clog << "Log: got " << n << '\n'; // buffered log to stderr
}
```

---

### 1.2 Opérations fondamentales d'extraction / insertion

- `operator>>(...)` sur `istream` : extraction formatée (lit des nombres, des mots, etc.)
- `operator<<(...)` sur `ostream` : insertion formatée (affiche des valeurs)
- `std::getline(istream&, std::string&)` — lit une ligne complète, s'arrête à `'\n'`
- `std::getline(istream&, std::string&, char delim)` — s'arrête à `delim`

```cpp
#include <iostream>
#include <string>

int main() {
    std::string word, line;

    // >> skips leading whitespace, reads one word
    std::cin >> word;           // input "  hello world" -> word = "hello"

    std::cin.ignore();          // discard leftover '\n' before getline

    // getline reads the entire line including spaces
    std::getline(std::cin, line);  // input "foo bar baz" -> line = "foo bar baz"

    // getline with custom delimiter: reads until ';'
    std::getline(std::cin, line, ';'); // input "abc;def" -> line = "abc"
}
```

---

### 1.3 Vérifications de l'état du flux avec `std::ios`

Ces fonctions indiquent si un flux est dans un état utilisable.

| Fonction      | Renvoie `true` lorsque...                     |
|---------------|-----------------------------------------------|
| `good()`      | Aucun drapeau d'erreur n'est positionné       |
| `eof()`       | La fin de fichier (EOF) a été atteinte        |
| `fail()`      | Un échec d'analyse/saisie récupérable est survenu |
| `bad()`       | Une erreur de flux grave (irrécupérable) est survenue |
| `rdstate()`   | Renvoie le masque de bits `iostate` brut      |

```cpp
#include <iostream>

int main() {
    int x;
    std::cin >> x;

    if (std::cin.good())  std::cout << "All fine\n";
    if (std::cin.fail())  std::cout << "Parse failed (e.g. typed 'abc' for int)\n";
    if (std::cin.eof())   std::cout << "Reached end of input\n";
    if (std::cin.bad())   std::cout << "Stream buffer is broken\n";

    // Check all bits at once
    std::ios::iostate state = std::cin.rdstate();
    if (state & std::ios::failbit) std::cout << "failbit is set\n";
}
```

---

### 1.4 Modifications de l'état avec `std::ios`

- `clear()` — réinitialise tous les drapeaux d'erreur (le flux redevient good)
- `clear(iostate)` — affecte un état spécifique au flux
- `setstate(iostate)` — ajoute des drapeaux sans effacer les autres
- `exceptions(iostate)` — configure le flux pour lever des exceptions sur certains drapeaux

```cpp
#include <iostream>

int main() {
    int x;
    std::cin >> x;                   // fails if user types "abc"

    if (std::cin.fail()) {
        std::cin.clear();            // clear failbit so the stream is usable again
        std::cin.ignore(1000, '\n'); // discard the bad input line
        std::cin >> x;              // try again
    }

    // Throw an exception when failbit or badbit is triggered
    std::cin.exceptions(std::ios::failbit | std::ios::badbit);
    try {
        std::cin >> x;
    } catch (const std::ios::failure& e) {
        std::cerr << "Stream exception: " << e.what() << '\n';
    }
}
```

---

### 1.5 Drapeaux de formatage (`flags`, `setf`, `unsetf`)

- `flags()` — récupère tous les drapeaux de formatage sous forme de masque de bits
- `flags(fmtflags)` — remplace tous les drapeaux
- `setf(fmtflags)` — active des drapeaux spécifiques
- `setf(fmtflags, fmtflags mask)` — définit un drapeau au sein d'un groupe (ex. adjustfield)
- `unsetf(fmtflags)` — désactive des drapeaux spécifiques

```cpp
#include <iostream>

int main() {
    // setf / unsetf directly
    std::cout.setf(std::ios::hex, std::ios::basefield);  // switch to hex
    std::cout << 255 << '\n';   // ff

    std::cout.unsetf(std::ios::hex);
    std::cout.setf(std::ios::dec, std::ios::basefield);  // back to decimal
    std::cout << 255 << '\n';   // 255

    // flags() snapshot and restore
    std::ios::fmtflags saved = std::cout.flags();        // save
    std::cout.setf(std::ios::showpos | std::ios::uppercase);
    std::cout << 42 << '\n';    // +42
    std::cout.flags(saved);     // restore
    std::cout << 42 << '\n';    // 42
}
```

---

### 1.6 Largeur, précision, remplissage

- `width(n)` — largeur de champ minimale pour la *prochaine* sortie formatée (se réinitialise après utilisation)
- `precision(n)` — chiffres significatifs (par défaut) ou chiffres après la virgule (avec `fixed`/`scientific`)
- `fill(c)` — caractère utilisé pour le remplissage lorsque le contenu est plus court que la largeur

```cpp
#include <iostream>

int main() {
    // width resets after one use
    std::cout.width(8);
    std::cout << 42 << '\n';        //       42  (6 spaces + 42)
    std::cout << 42 << '\n';        // 42        (width is gone)

    // fill persists until changed
    std::cout.fill('0');
    std::cout.width(6);
    std::cout << 42 << '\n';        // 000042

    // precision
    std::cout.precision(4);
    std::cout << 3.14159 << '\n';   // 3.142 (4 significant digits)

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(2);
    std::cout << 3.14159 << '\n';   // 3.14 (2 decimal places)
}
```

---

### 1.7 Paramètres régionaux (locale) et liaisons (ties)

- `getloc()` — renvoie la locale actuelle
- `imbue(locale)` — modifie la locale (affecte le formatage numérique/monétaire/temporel)
- `tie()` — renvoie un pointer vers le flux lié (par défaut `cout` est lié à `cin`)
- `tie(ostream*)` — lie un flux afin qu'il soit vidé (flush) avant toute lecture sur ce flux

```cpp
#include <iostream>
#include <locale>

int main() {
    // Imbue French locale for number formatting (e.g. thousands separator)
    std::cout.imbue(std::locale("fr_FR.UTF-8"));
    std::cout << 1000000.5 << '\n';   // 1 000 000,5 (locale-dependent)

    // tie: cout is flushed automatically before cin reads
    // This ensures prompts appear before blocking on input
    std::ostream* old_tie = std::cin.tie();  // old_tie == &std::cout
    std::cin.tie(nullptr);                   // remove the tie (less safe, faster)
    std::cin.tie(old_tie);                   // restore
}
```

---

### 1.8 Buffer et synchronisation

- `rdbuf()` — récupère le `streambuf` sous-jacent
- `rdbuf(streambuf*)` — redirige le flux vers un buffer différent
- `std::ios_base::sync_with_stdio(bool)` — synchronise les E/S C et C++ (par défaut : true)

```cpp
#include <iostream>
#include <sstream>

int main() {
    // Redirect cout to a string buffer
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    std::cout << "captured output";
    std::cout.rdbuf(old);           // restore cout
    std::cout << oss.str() << '\n'; // "captured output"

    // Disable sync for faster I/O (do this before any I/O)
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
}
```

---

### 1.9 `std::istream` — saisie non formatée

#### Surcharges de `get()`

Lit des caractères sans ignorer les espaces blancs (contrairement à `>>`).

```cpp
#include <iostream>

int main() {
    // Read one char as int (returns EOF on end)
    int ch = std::cin.get();            // e.g. reads 'A', returns 65

    // Read one char into a variable
    char c;
    std::cin.get(c);                    // c = next char including spaces/newlines

    // Read up to n-1 chars into buffer, stops at newline (keeps it in stream)
    char buf[20];
    std::cin.get(buf, 20);              // reads max 19 chars

    // Same but with custom delimiter
    std::cin.get(buf, 20, ';');         // stops at ';' or 19 chars
}
```

#### `getline()` sur istream

```cpp
char buf[50];
std::cin.getline(buf, 50);       // reads line, discards '\n'
std::cin.getline(buf, 50, ';');  // reads until ';', discards it
```

#### `read()` et `readsome()`

```cpp
#include <iostream>
#include <fstream>

int main() {
    std::ifstream f("data.bin", std::ios::binary);

    char data[16];
    f.read(data, 16);                    // read exactly 16 bytes
    std::streamsize got = f.gcount();    // how many were actually read

    // readsome: reads only what's immediately available (no blocking)
    char tmp[8];
    std::streamsize n = std::cin.readsome(tmp, 8);
}
```
#### `gcount()`

```cpp
std::cin.read(buf, 100);
std::cout << "Read " << std::cin.gcount() << " bytes\n";
```

---

### 1.10 `std::istream` — anticipation et réinjection

- `peek()` — observer le caractère suivant sans l'extraire
- `unget()` — réinjecter le dernier caractère lu
- `putback(char)` — réinjecter un caractère spécifique

```cpp
#include <iostream>

int main() {
    // peek : inspecter ce qui arrive sans le consommer
    if (std::cin.peek() == '-') {
        char sign;
        std::cin >> sign;   // consomme le '-'
    }

    // unget : réinjecter ce qui a été lu en dernier
    char c;
    std::cin.get(c);
    if (c == '\n') std::cin.unget(); // réinjecte le saut de ligne

    // putback : réinjecter un caractère spécifique (peut différer de ce qui a été lu)
    std::cin.get(c);
    std::cin.putback('X'); // le prochain get() retournera 'X'
}
```

---

### 1.11 `std::istream` — saut et positionnement

- `ignore()` — ignorer un caractère
- `ignore(n)` — ignorer jusqu'à n caractères
- `ignore(n, delim)` — ignorer jusqu'à n caractères ou jusqu'à delim (inclus)
- `seekg(pos)` — se déplacer vers une position absolue
- `seekg(off, dir)` — se déplacer relativement à `beg`, `cur` ou `end`
- `tellg()` — position de lecture actuelle

```cpp
#include <iostream>
#include <sstream>

int main() {
    // ignore : sauter le '\n' restant après >>
    int x;
    std::cin >> x;
    std::cin.ignore(1000, '\n'); // ignorer le reste de la ligne

    // seekg / tellg (fonctionne sur les file/string streams, pas cin)
    std::istringstream ss("hello world");
    std::streampos pos = ss.tellg();  // pos = 0
    ss.seekg(6);                      // sauter à la position 6
    std::string word;
    ss >> word;                       // word = "world"
    ss.seekg(0, std::ios::beg);       // retour au début
    ss >> word;                       // word = "hello"
}
```

---

### 1.12 `std::ostream` — sortie non formatée et positionnement

- `put(char)` — écrire un caractère unique
- `write(ptr, n)` — écrire n octets bruts
- `flush()` — vider le buffer de sortie
- `seekp(pos)` / `seekp(off, dir)` — modifier la position d'écriture
- `tellp()` — position d'écriture actuelle

```cpp
#include <iostream>
#include <fstream>

int main() {
    // put : caractère unique
    std::cout.put('A');          // affiche A (sans saut de ligne)
    std::cout.put('\n');

    // write : bloc brut (idéal pour les sorties binaires ou de longueur fixe)
    const char msg[] = "hello";
    std::cout.write(msg, 5);     // affiche "hello"
    std::cout.put('\n');

    // flush : forcer la sortie bufférisée vers le terminal/fichier
    std::cout << "progress..." << std::flush; // immédiatement visible

    // seekp / tellp sur les file streams
    std::fstream f("out.bin", std::ios::in | std::ios::out | std::ios::binary);
    f.seekp(10);                 // déplacer la tête d'écriture à l'octet 10
    f.write("XX", 2);           // écraser les octets 10-11
    std::streampos p = f.tellp(); // p = 12
}
```

---

### 1.13 Groupes de flags de formatage `ios_base`

**Adjustfield** (alignement) :
```cpp
std::cout << std::left  << std::setw(10) << "hi" << "|\n"; // "hi        |"
std::cout << std::right << std::setw(10) << "hi" << "|\n"; // "        hi|"
std::cout << std::internal << std::setw(10) << -42 << '\n'; // "-       42"
```

**Basefield** (base entière) :
```cpp
std::cout << std::dec << 255 << '\n'; // 255
std::cout << std::oct << 255 << '\n'; // 377
std::cout << std::hex << 255 << '\n'; // ff
```

**Floatfield** (style à virgule flottante) :
```cpp
double v = 123456.789;
std::cout << std::defaultfloat << v << '\n';              // 123457
std::cout << std::fixed       << std::setprecision(2) << v << '\n'; // 123456.79
std::cout << std::scientific  << std::setprecision(2) << v << '\n'; // 1.23e+05
std::cout << std::hexfloat    << v << '\n';               // 0x1.e240cap+16
```

**Autres flags courants** :
```cpp
std::cout << std::boolalpha  << true  << '\n'; // true  (not 1)
std::cout << std::noboolalpha << true << '\n'; // 1
std::cout << std::showbase   << std::hex << 255 << '\n'; // 0xff
std::cout << std::showpos    << 42   << '\n';  // +42
std::cout << std::showpoint  << 1.0  << '\n';  // 1.00000
std::cout << std::uppercase  << std::hex << 255 << '\n'; // FF
```

---

### 1.14 Manipulateurs de flux de base

- `std::endl` — insérer `'\n'` et vider le buffer
- `std::ends` — insérer `'\0'` (utile dans `ostringstream`)
- `std::flush` — vider le buffer sans saut de ligne
- `std::ws` — ignorer les espaces de tête en entrée

```cpp
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::cout << "line 1" << std::endl; // saut de ligne + flush immédiat
    std::cout << "buffered" << std::flush; // pas de saut de ligne, juste flush

    std::ostringstream oss;
    oss << "hello" << std::ends;        // oss contient "hello\0"

    // std::ws : ignorer les espaces avant la lecture
    std::istringstream ss("   42");
    int n;
    ss >> std::ws >> n;                 // n = 42 (espaces ignorés)
}
```

---

## 2. `<iomanip>`

`<iomanip>` fournit des manipulateurs paramétrés pour le formatage.

---

### 2.1 `std::setw(n)` — largeur de champ

Définit la largeur minimale pour la **prochaine** opération formatée uniquement (la largeur se réinitialise après chaque utilisation).

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::setw(8) << 42     << '\n'; //       42
    std::cout << std::setw(8) << "hi"   << '\n'; //       hi
    std::cout << std::setw(8) << 3.14   << '\n'; //     3.14

    // Combiner avec setfill et l'alignement
    std::cout << std::setfill('-') << std::setw(10) << std::left  << "left"  << '\n'; // left------
    std::cout << std::setfill('-') << std::setw(10) << std::right << "right" << '\n'; // -----right
}
```

---

### 2.2 `std::setfill(c)` — caractère de remplissage

Définit le caractère de remplissage (persiste jusqu'à modification, contrairement à `setw`).

```cpp
#include <iostream>
#include <iomanip>

int main() {
    // Remplissage de zéros pour les nombres (utilisation classique)
    std::cout << std::setfill('0') << std::setw(5) << 42   << '\n'; // 00042
    std::cout << std::setfill('0') << std::setw(5) << 7    << '\n'; // 00007

    // Séparateur point pour l'alignement dans les tableaux
    std::cout << std::setfill('.')
              << std::setw(20) << std::left << "Item"
              << std::setw(6)  << std::right << 9.99 << '\n';
    // Item................  9.99
}
```

---

### 2.3 `std::setprecision(n)` — précision en virgule flottante

```cpp
#include <iostream>
#include <iomanip>

int main() {
    double pi = 3.14159265358979;

    // Par défaut : n chiffres significatifs
    std::cout << std::setprecision(3) << pi << '\n'; // 3.14

    // Avec fixed : n chiffres après la virgule
    std::cout << std::fixed << std::setprecision(5) << pi << '\n'; // 3.14159

    // Avec scientific
    std::cout << std::scientific << std::setprecision(2) << pi << '\n'; // 3.14e+00

    // Retour au comportement par défaut
    std::cout << std::defaultfloat << std::setprecision(6) << pi << '\n'; // 3.14159
}
```

---

### 2.4 `std::setbase(n)` — base entière

```cpp
#include <iostream>
#include <iomanip>

int main() {
    int n = 255;
    std::cout << std::setbase(10) << n << '\n'; // 255
    std::cout << std::setbase(16) << n << '\n'; // ff
    std::cout << std::setbase(8)  << n << '\n'; // 377

    // setbase(0) en entrée : détection automatique du préfixe (0x = hex, 0 = octal, sinon décimal)
    int x;
    std::istringstream ss("0xff");
    ss >> std::setbase(0) >> x; // x = 255
}
```

---

### 2.5 `std::showbase` / `std::noshowbase` — préfixe de base

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::hex << std::showbase << 255 << '\n';  // 0xff
    std::cout << std::oct << std::showbase << 255 << '\n';  // 0377
    std::cout << std::hex << std::noshowbase << 255 << '\n'; // ff
}
```

---

### 2.6 `std::showpos` / `std::noshowpos` — signe `+` explicite

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::showpos << 42   << '\n'; // +42
    std::cout << std::showpos << -42  << '\n'; // -42 (signe déjà présent)
    std::cout << std::showpos << 0    << '\n'; // +0
    std::cout << std::noshowpos << 42 << '\n'; // 42
}
```

---

### 2.7 `std::boolalpha` / `std::noboolalpha`

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>

int main() {
    // Sortie
    std::cout << std::boolalpha  << true << " " << false << '\n'; // true false
    std::cout << std::noboolalpha << true << " " << false << '\n'; // 1 0

    // Entrée : affecte également le parsing
    bool b;
    std::istringstream ss("true");
    ss >> std::boolalpha >> b; // b = true
}
```

---

### 2.8 `std::uppercase` / `std::nouppercase`

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::uppercase << std::hex << 0xdeadbeef << '\n'; // DEADBEEF
    std::cout << std::uppercase << std::scientific << 1234.5 << '\n'; // 1.2345E+03
    std::cout << std::nouppercase << std::hex << 0xdeadbeef << '\n'; // deadbeef
}
```

---

### 2.9 `std::showpoint` / `std::noshowpoint`

Force ou supprime le point décimal et les zéros de fin pour les valeurs en virgule flottante.

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::showpoint  << 1.0  << '\n'; // 1.00000 (virgule + zéros affichés)
    std::cout << std::noshowpoint << 1.0 << '\n'; // 1       (aucun zéro de fin)
    std::cout << std::showpoint  << 1.5  << '\n'; // 1.50000
}
```
---

### 2.10 `std::skipws` / `std::noskipws`

Contrôle si `>>` ignore les espaces de tête. Activé par défaut.

```cpp
#include <iostream>
#include <sstream>

int main() {
    std::istringstream ss("  A  B");
    char c;

    // Par défaut (skipws activé) : ignore les espaces avant chaque caractère
    ss >> c; // c = 'A'
    ss >> c; // c = 'B'

    ss.clear(); ss.str("  A  B");
    ss >> std::noskipws;
    ss >> c; // c = ' ' (l'espace est lu tel quel)
    ss >> c; // c = ' '
    ss >> c; // c = 'A'
}
```

---

### 2.11 `std::unitbuf` / `std::nounitbuf`

Vide le buffer (flush) après chaque opération de sortie (comme le comportement par défaut de `cerr`).

```cpp
#include <iostream>

int main() {
    // Avec unitbuf : chaque << vide le buffer immédiatement (lent mais sûr pour les logs)
    std::cout << std::unitbuf;
    std::cout << "step 1\n"; // vidé immédiatement
    std::cout << "step 2\n"; // vidé immédiatement

    std::cout << std::nounitbuf; // retour à la sortie bufférisée normale
}
```

---

### 2.12 `std::get_money` / `std::put_money`

Analyse et formate des valeurs monétaires en utilisant la locale du stream.

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>

int main() {
    // put_money : formate une valeur monétaire (en sous-unités, par ex. centimes)
    std::cout.imbue(std::locale("en_US.UTF-8"));
    std::cout << std::showbase << std::put_money(123456) << '\n'; // $1,234.56

    // get_money : analyse une valeur monétaire depuis l'entrée
    long double amount;
    std::istringstream ss("$1,234.56");
    ss.imbue(std::locale("en_US.UTF-8"));
    ss >> std::get_money(amount); // amount = 123456 (sous-unités)
}
```

---

### 2.13 `std::get_time` / `std::put_time`

Analyse et formate des valeurs de date/heure en utilisant `std::tm`.

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

int main() {
    // put_time : formate une struct std::tm
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::cout << std::put_time(now, "%Y-%m-%d %H:%M:%S") << '\n'; // 2026-04-10 14:30:00

    // get_time : analyse une chaîne de date dans un std::tm
    std::tm parsed = {};
    std::istringstream ss("2026-04-10 14:30:00");
    ss >> std::get_time(&parsed, "%Y-%m-%d %H:%M:%S");
    if (!ss.fail())
        std::cout << "Year: " << (parsed.tm_year + 1900) << '\n'; // Year: 2026

    // Spécificateurs de format courants :
    // %Y = année sur 4 chiffres, %m = mois (01-12), %d = jour (01-31)
    // %H = heure (00-23), %M = minute, %S = seconde
    // %A = nom du jour, %B = nom du mois, %I = heure au format 12h, %p = AM/PM
}
```

---

## 3. Combiner les manipulateurs — patterns pratiques

### Sortie sous forme de tableau formaté

```cpp
#include <iostream>
#include <iomanip>
#include <string>

int main() {
    const int W1 = 16, W2 = 8, W3 = 10;
    std::cout << std::left
              << std::setw(W1) << "Name"
              << std::setw(W2) << "Score"
              << std::setw(W3) << "Grade" << '\n';
    std::cout << std::string(W1 + W2 + W3, '-') << '\n';

    std::cout << std::setw(W1) << "Alice"
              << std::setw(W2) << std::right << std::fixed << std::setprecision(1) << 95.5
              << std::setw(W3) << "A" << '\n';
    std::cout << std::left
              << std::setw(W1) << "Bob"
              << std::setw(W2) << std::right << 78.0
              << std::setw(W3) << "C+" << '\n';
}
// Name            Score     Grade
// ----------------------------------
// Alice             95.5         A
// Bob               78.0        C+
```

### Lecture et validation de l'entrée utilisateur

```cpp
#include <iostream>
#include <limits>

int readInt(const std::string& prompt) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x) break;             // succès
        std::cin.clear();                      // effacer les flags d'erreur
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // vider la mauvaise entrée
        std::cout << "Invalid input, try again.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // vider le saut de ligne final
    return x;
}
```

### Rediriger `cout` pour capturer la sortie

```cpp
#include <iostream>
#include <sstream>

int main() {
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // rediriger

    std::cout << "Hello, captured world!";

    std::cout.rdbuf(old);                // restaurer
    std::string captured = buffer.str(); // "Hello, captured world!"
    std::cout << captured << '\n';
}
```

---

## 4. Distinctions importantes

- `<iostream>` fournit les objets/classes de stream et les opérations fondamentales de stream.
- `<iomanip>` fournit les manipulateurs de formatage (particulièrement ceux paramétrés : `setw`, `setprecision`, `setfill`, `setbase`, `put_time`, `put_money`).
- La plupart des manipulateurs de type flag (`hex`, `fixed`, `left`, `boolalpha`, …) sont persistants — ils restent actifs jusqu'à ce qu'ils soient explicitement modifiés.
- `setw` est le **seul** manipulateur courant qui se réinitialise après une seule utilisation.
- Préférez `'\n'` à `std::endl` dans les boucles sensibles aux performances ; `endl` effectue un flush à chaque appel.