# C++ Module 01 — Guide d'apprentissage

**Thème du module :** *Memory allocation, pointers to members, references, et switch statements.*

Ce module est la première fois où l'on vous demande de réfléchir à **l'endroit** où vivent vos objets (stack vs heap), à **la manière** dont vous y faites référence (valeur, pointer, reference), et à **la manière** de dispatcher un comportement sans une chaîne de `if/else`. Chaque exercice isole l'une de ces idées.

---

## Exercice 00 — Braiiiiiinnnzzz

### Ce que vous apprenez
**L'allocation sur la stack vs l'allocation sur la heap, et quand choisir l'une ou l'autre.**

- `Zombie foo("bar");` — **stack**. L'objet meurt lorsqu'il quitte le scope. Le destructor s'exécute automatiquement.
- `Zombie* foo = new Zombie("bar");` — **heap**. L'objet vit jusqu'à ce que vous le `delete`. Vous êtes responsable de ce `delete`.

### Règles empiriques pratiques
| Situation | Utilisation |
|---|---|
| L'objet n'est nécessaire qu'à l'intérieur de cette fonction | Stack |
| L'objet doit survivre à la fonction dans laquelle il a été créé | Heap (`new`) |
| Vous voulez une quantité fixe et connue au compile time | Stack |
| Vous devez déterminer la quantité au runtime | Heap |

### Conseils
- `newZombie` retourne un pointer → l'appelant doit le `delete`. Commentez cela dans votre code ou documentez-le — l'ownership de la mémoire est un *contrat*.
- `randomChump` utilise un **objet local sur la stack** ; pas de `new`, pas de `delete`. Le destructor se déclenche automatiquement à la fin de la fonction.
- Faites en sorte que le destructor affiche quelque chose (ex. `"<name> destroyed"`). Vous *verrez* quand chaque zombie meurt — c'est tout l'intérêt de l'exercice.
- Si vous voyez le message du destructor s'afficher deux fois pour le même zombie, vous avez un double-free. Si vous ne le voyez jamais, vous avez une fuite (leak).

### Piège
C++98 n'a pas de `nullptr`. Utilisez `NULL` ou simplement `0` lorsqu'un pointer pourrait ne pas être défini.

---

## Exercice 01 — Moar brainz!

### Ce que vous apprenez
**L'allocation de tableaux sur la heap en un seul `new[]`, et le `delete[]` correspondant.**

```cpp
Zombie* horde = new Zombie[N];     // single allocation for N objects
// ...
delete[] horde;                    // MUST use delete[], not delete
```

### Détails pratiques
- `new Zombie[N]` appelle le **default constructor** N fois. Votre class `Zombie` **doit** donc avoir un default constructor — ce qui signifie que vous devez avoir un moyen distinct de définir le nom plus tard (ex. un setter, ou l'assignation à l'attribut public).
- Vous ne pouvez pas utiliser `new Zombie[N]("name")` — cette syntaxe n'existe pas. Vous devez itérer avec une boucle et les nommer après l'allocation.
- Le pattern habituel :
  ```cpp
  Zombie* horde = new Zombie[N];
  for (int i = 0; i < N; i++)
      horde[i].setName(name);
  ```

### Conseils
- `delete horde;` (sans `[]`) sur un tableau = **undefined behavior**. Sur la plupart des compilers, cela ne détruit que le premier élément et fait fuiter le reste. Associez toujours `new[]` avec `delete[]`.
- Exécutez avec `valgrind ./moar_brainz` pour confirmer zéro leak / zéro erreur.
- Le sujet de l'ex01 indique d'allouer N Zombies en une **seule** allocation ("single allocation"). Ne faites donc PAS `N` appels individuels à `new Zombie(...)` dans une boucle — cela représenterait `N` allocations.

### Piège
Si votre `Zombie` de l'ex00 n'a pas de default constructor, vous devez en ajouter un pour l'ex01. Chaque exercice étant évalué indépendamment, cela ne pose aucun problème.

---

## Exercice 02 — HI THIS IS BRAIN

### Ce que vous apprenez
**Les references ne sont qu'une autre syntaxe pour la "manipulation d'adresses".** Cet exercice dissipe le mystère : une reference contient la même adresse qu'un pointer ; elle la déréférence simplement de manière implicite.

### Ce qu'il faut afficher
```cpp
std::string   str = "HI THIS IS BRAIN";
std::string*  stringPTR = &str;
std::string&  stringREF = str;

std::cout << &str       << std::endl;  // address of str
std::cout << stringPTR  << std::endl;  // same address
std::cout << &stringREF << std::endl;  // same address

std::cout << str        << std::endl;  // value
std::cout << *stringPTR << std::endl;  // same value (dereference)
std::cout << stringREF  << std::endl;  // same value (implicit)
```

### Points clés à retenir (mémorisez-les)
| | Pointer | Reference |
|---|---|---|
| Peut être null | Oui (`NULL`) | **Jamais** |
| Peut être réassigné | Oui | **Jamais** (lié à l'initialisation) |
| Doit être initialisé | Non (mais UB si utilisé non initialisé) | **Oui, dès la déclaration** |
| Syntaxe pour accéder à la valeur | `*p` | `r` (implicite) |
| Syntaxe pour obtenir l'adresse | `p` (est déjà une adresse) | `&r` |

### Conseil
Lorsque vous voyez `stringREF` *se comporter comme* `str`, vous avez compris les references. C'est du sucre syntaxique par-dessus "un pointer qui ne peut pas être null et ne peut pas être modifié".

---

## Exercice 03 — Unnecessary violence

### Ce que vous apprenez
**Quand utiliser un membre reference vs un membre pointer dans une class.** C'est la suite naturelle de l'ex02.

### La question de conception
- `HumanA` a **toujours** une arme → le membre est une `Weapon&` (une reference ne peut pas être null).
- `HumanB` **peut ou non** avoir une arme → le membre est un `Weapon*` (un pointer peut être `NULL`, et peut être réassigné avec `setWeapon`).

### Détails pratiques
- `Weapon::getType()` doit retourner une `const std::string&`, pas une `std::string`. Un retour par valeur copie la chaîne ; un retour par const-ref évite la copie et respecte le sujet.
- `HumanA` prend la `Weapon` dans son **constructor** — la reference est liée une fois pour toutes.
- `HumanB` n'a **aucun argument d'arme** dans son constructor. Il utilise `setWeapon()` séparément. Initialisez par défaut le pointer à `NULL` pour éviter les valeurs résiduelles (garbage).
- **Ownership :** aucun des deux Humains ne *possède* la Weapon. C'est le `main` qui la possède. Le destructor de HumanB ne doit PAS `delete` le pointer.

### Le piège de la Forme Canonique Orthodoxe (OCF)
- `Weapon` : une OCF complète fonctionne très bien.
- `HumanA` : **impossible** de fournir une OCF complète. Une class avec un membre reference :
  - Ne peut pas avoir de default constructor (la reference doit être liée à la construction).
  - Ne peut pas avoir un `operator=` significatif (les references ne peuvent pas être réassignées/rebound).
  
  La réponse honnête : fournissez un ctor avec paramètres + copy ctor + destructor, et omettez `operator=` ou écrivez un stub pour ne copier que les membres qui ne sont pas des references (comme `name`). Soyez prêt à expliquer cela en soutenance — les évaluateurs vérifient si vous comprenez le *pourquoi*.
- `HumanB` : une OCF complète fonctionne. Le destructor ne doit PAS delete le pointer (ownership partagée, le `main` possède la Weapon).

### Sortie attendue
```
Bob attacks with their crude spiked club
Bob attacks with their some other type of club
Jim attacks with their crude spiked club
Jim attacks with their some other type of club
```
Le fait que la seconde attaque de Bob affiche le type d'arme *mis à jour* est la démonstration : les membres reference/pointer partagent le même objet sous-jacent — modifier `club` est visible à travers Bob.

### Conseil
L'indice du sujet demande "*quand un pointer est-il plus adapté ? quand une reference est-elle plus adaptée ?*" — réponse : **reference quand la relation est obligatoire et permanente ; pointer quand elle est optionnelle ou peut changer.**

---

## Exercice 04 — Sed is for losers

### Ce que vous apprenez
**La manipulation de `std::string` et les I/O de fichiers en C++ (`ifstream`/`ofstream`).** Aussi : réimplémenter quelque chose que vous feriez normalement avec `sed`, sans utiliser `std::string::replace`.

### L'algorithme (puisque `replace` est interdit)
```cpp
std::string result;
size_t i = 0;
while (i < content.size()) {
    size_t found = content.find(s1, i);
    if (found == std::string::npos) {
        result += content.substr(i);  // append the rest
        break;
    }
    result += content.substr(i, found - i);  // text before the match
    result += s2;                             // the replacement
    i = found + s1.size();                    // skip past the match
}
```

### Détails pratiques
- Lisez d'abord le fichier entier dans une seule `std::string` (plus facile qu'une lecture ligne par ligne pour les correspondances réparties sur plusieurs lignes) :
  ```cpp
  std::ifstream in(filename.c_str());
  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string content = buffer.str();
  ```
- Fichier de sortie : `<filename>.replace` — c'est-à-dire `argv[1] + ".replace"`.
- Gérez les erreurs : fichier manquant, `s1` vide (boucle infinie si vous ne faites pas attention !), mauvais argc.

### Conseils
- **Si `s1` est vide**, votre boucle n'avancera jamais — prévoyez une condition explicite pour cela et quittez avec une erreur ou traitez-le comme "aucun remplacement".
- **N'utilisez pas `getline` + un remplacement ligne par ligne** si `s1` est susceptible de contenir un retour à la ligne. Lisez le fichier entier d'un coup.
- `std::ifstream` prend un `const char*` en C++98, passez donc `filename.c_str()`, pas la `std::string` directement (cela n'existe qu'à partir de C++11).
- Fermez vos streams (ou laissez faire le RAII — ils se ferment à la destruction).

### Piège
Interdits : `std::string::replace`, toutes les fonctions de fichiers C (`fopen`, `fread`, `fwrite`, `fclose`, etc.). Vous devez utiliser les streams C++.

---

## Exercice 05 — Harl 2.0

### Ce que vous apprenez
**Les pointers vers des fonctions membres (pointers to member functions)** — la manière idiomatique en C++ de dispatcher vers l'une de plusieurs méthodes sans une chaîne de `if/else`.

### Syntaxe (c'est tout le sujet de l'exercice)
```cpp
// Declaration of a pointer to a member function of Harl:
void (Harl::*fn)(void);

// Assigning it:
void (Harl::*fn)(void) = &Harl::debug;

// Calling it on an instance:
(this->*fn)();
// or on an object:
(harlInstance.*fn)();
```

### La structure canonique
```cpp
void Harl::complain(std::string level) {
    std::string levels[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    void (Harl::*fns[4])(void) = {
        &Harl::debug, &Harl::info, &Harl::warning, &Harl::error
    };
    for (int i = 0; i < 4; i++) {
        if (level == levels[i]) {
            (this->*fns[i])();
            return;
        }
    }
}
```
### Tips
- Le sujet **interdit** l'approche forêt de if/else. Utiliser un `switch` ici passe également à côté du sujet — c'est dans l'ex06 que le `switch` a sa place. Utilisez l'approche par tableau de member-function pointers.
- `debug`, `info`, `warning`, `error` sont **private**. Seul `complain` est public. C'est délibéré — le client de `Harl` interagit via des niveaux, pas via des noms de méthodes.
- La syntaxe des member-function pointers est *vraiment* laide. Prenez le temps de l'écrire plusieurs fois jusqu'à ce qu'elle cesse d'avoir l'air étrange : `return_type (ClassName::*varname)(params)`.

### Piège
`Harl::debug` seul est un nom de fonction ; vous devez écrire `&Harl::debug` pour obtenir le pointer. Contrairement aux fonctions libres (où `foo` et `&foo` sont équivalents), les member functions *nécessitent* le `&`.

---

## Exercice 06 — Harl filter

### Ce que vous apprenez
**L'instruction `switch` avec fall-through intentionnel.** Cet exercice réutilise la class `Harl` de l'ex05.

### L'astuce
Filtrer en mode "afficher ce niveau **et tout ce qui est plus sévère**". C'est taillé sur mesure pour le fall-through :

```cpp
switch (levelIndex) {
    case 0:  // DEBUG
        harl.complain("DEBUG");
        // fall through
    case 1:  // INFO
        harl.complain("INFO");
        // fall through
    case 2:  // WARNING
        harl.complain("WARNING");
        // fall through
    case 3:  // ERROR
        harl.complain("ERROR");
        break;
    default:
        std::cout << "[ Probably complaining about insignificant problems ]" << std::endl;
}
```

### Détails pratiques
- Traduisez d'abord l'argument string (`"WARNING"`) en un index de 0 à 3.
- Passez les niveaux inconnus à la branche `default` — la sortie attendue est exactement `[ Probably complaining about insignificant problems ]`.
- Le nom de l'exécutable doit être **`harlFilter`** (exigence du sujet — vérifiez votre Makefile).

### Tips
- Ajoutez des commentaires comme `// fall through` au-dessus de chaque fall-through implicite — les compilers modernes émettent un warning à ce sujet (`-Wimplicit-fallthrough`) et les relecteurs vous en remercieront.
- La spécification de la sortie montre une ligne vide entre les sections (par ex. entre `WARNING` et `ERROR`). Assurez-vous que vos fonctions `Harl::warning()`, etc., se terminent par un saut de ligne, et déterminez si vous avez besoin d'un saut de ligne supplémentaire entre les niveaux.

### Piège
L'ex06 est **optionnel** — vous pouvez valider le module sans lui. Mais c'est le seul exercice qui enseigne le `switch` dans ce module, donc ne le sautez que si vous manquez vraiment de temps.

---

## Checklist globale du module avant la soutenance

- [ ] Compile avec `c++ -Wall -Wextra -Werror -std=c++98` — aucun warning.
- [ ] Zéro fuite de mémoire sous `valgrind` (en particulier ex00, ex01, ex03).
- [ ] Aucun `using namespace std;` nulle part.
- [ ] Aucun mot-clé `friend`.
- [ ] Aucun container STL (`vector`, `map`, etc.) — interdit jusqu'au Module 08.
- [ ] Pas de `*printf`, `*alloc`, `free` — c'est -42 immédiat.
- [ ] Chaque class est dans sa propre paire `.hpp` / `.cpp`, nommée `ClassName.hpp` / `ClassName.cpp`.
- [ ] Chaque `.hpp` a des include guards.
- [ ] Aucune implémentation de fonction dans les headers (sauf pour les templates).
- [ ] Chaque header est **auto-suffisant** — il inclut tout ce dont il a besoin et ne dépend pas de l'ordre d'inclusion.
- [ ] À partir du Mod02 (et déjà ici quand cela a du sens), les classes suivent la **Forme Canonique Orthodoxe** — avec l'exception de type HumanA expliquée.

## Carte concept-vers-exercice (référence rapide)

| Concept | Où il apparaît |
|---|---|
| Stack vs heap allocation | ex00 |
| `new[]` / `delete[]` | ex01 |
| Memory leaks & `valgrind` | ex00, ex01, ex03 |
| References vs pointers (syntaxe) | ex02 |
| References vs pointers (en tant que membres de class) | ex03 |
| Forme Canonique Orthodoxe avec des references | ex03 |
| E/S sur fichiers avec `ifstream`/`ofstream` | ex04 |
| Manipulation de `std::string` | ex04 |
| Pointers vers des member functions | ex05 |
| Instruction `switch` (avec fall-through) | ex06 |

## Éléments à lire / revoir séparément

- **References** — la page de cppreference sur "Reference declaration" (à survoler, pas à mémoriser).
- **Opérateurs d'accès aux membres** — spécifiquement `.*` et `->*` (utilisés dans l'ex05).
- **Streams de fichiers** — interactions entre `<fstream>`, `<sstream>`, `<iostream>`.
- **Member functions de `std::string`** — voir [STRING_FUNCTIONS.md](../../notions/io-errors/STRING_FUNCTIONS.md) dans ce répertoire.
- **Forme Canonique Orthodoxe** — voir [ORTHODOX_CANONICAL_FORM.md](../../notions/oop/ORTHODOX_CANONICAL_FORM.md).