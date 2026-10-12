# C++ Module 05 — Guide d'apprentissage

**Thème du module :** *Répétition et Exceptions — lancer, intercepter,
et la hiérarchie `std::exception`, articulés autour d'une chaîne
de classes grandissante (Bureaucrat → Form → Intern).*

Le Mod 04 vous a enseigné le polymorphisme. Le Mod 05 s'appuie sur cette base,
mais son *véritable* sujet est **la gestion des erreurs à la manière C++** :
au lieu de renvoyer `-1` en espérant que l'appelant le vérifie (l'habitude du C),
vous **lancez** (throw) une exception et la laissez se propager jusqu'à ce que quelqu'un
l'**intercepte** (catch). Les quatre exercices forment une seule et même histoire — vous
construisez un `Bureaucrat`, lui donnez des `Form`s à signer et à exécuter, puis
embauchez un `Intern` pour remplir les formulaires à votre place. Chaque étape ajoute
une nouvelle idée par-dessus la précédente.

Voir aussi : [`lexique/`](../../lexique/INDEX.md) pour des zooms sur les
mots-clés `throw`/`try`/`catch` et [`oop/ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md)
pour les règles de la Forme Canonique Orthodoxe que chaque class hors-exception doit respecter ici.

---

## La phrase à mémoriser

> Une **exception** est une valeur que vous lancez (`throw`) à l'endroit précis où vous
> détectez un problème ; elle déroule la stack, en exécutant les destructors en
> chemin, jusqu'à ce qu'un `catch` correspondant la traite — découplant ainsi *l'endroit
> où l'erreur se produit* de *l'endroit où elle est gérée*.

Image de la vie réelle : une **alarme incendie**. La personne qui sent la fumée
(la fonction de bas niveau) ne décide pas de la façon d'évacuer le
bâtiment — elle tire simplement l'alarme (`throw`). Tous les occupants en sortant
ferment la porte de leur bureau derrière eux (les destructors s'exécutent pendant
le stack unwinding). Le responsable de sécurité à la sortie (`catch`) décide
de ce qu'il faut réellement faire.

---

## Ce que l'ensemble du module vous apprend

- **`throw` / `try` / `catch`** — les trois mots-clés de la gestion
  d'erreurs en C++.
- **Les classes d'exception personnalisées** héritant de `std::exception`, avec
  une méthode `what()` — et la particularité du spécificateur `throw()` en C++98.
- **La sécurité des exceptions (exception safety)** : parce que les destructors s'exécutent pendant le unwinding,
  le RAII (ressources possédées par des objets) garantit l'absence de fuites, même lorsque tout
  explose. C'est *pourquoi* le langage s'appuie si lourdement sur les constructors/
  destructors.
- **Les membres `const`** et leur impact sur votre Forme Canonique
  Orthodoxe (le copy-assignment operator devient problématique).
- Une douce **redécouverte des classes abstraites** (ex02) et un
  **clean-dispatch pattern** pour éviter les cascades de `if/else` (ex03).

Pourquoi c'est utile : tout programme C++ non trivial qui touche aux fichiers,
à la mémoire, au réseau ou aux entrées utilisateur a besoin d'une méthode rigoureuse pour signaler
les échecs et s'en remettre. `throw`/`catch` constitue cette discipline, et
la STL elle-même lance des exceptions (`std::out_of_range`, `std::bad_alloc`, …),
vous devez donc être parfaitement à l'aise pour les intercepter.

---

## Le mécanisme, en un schéma

```
   deep_function()  ── detects bad grade ──►  throw GradeTooLow();
        │                                          │
        │  (stack unwinds: locals destructed)      │
        ▼                                          ▼
   middle_function()   ── no matching catch ──►  keeps propagating
        │                                          │
        ▼                                          ▼
   main() { try { ... } catch (std::exception& e) { e.what(); } }
                                     ▲
                          first matching handler wins
```

Si **personne** n'intercepte, `std::terminate()` est appelée et le
programme s'arrête brutalement. Une exception interceptée reprend l'exécution juste après
le bloc `catch` — et non là où le `throw` a eu lieu.

---

## Exercice 00 — Le Bureaucrat

### Ce que vous apprenez
**Comment déclarer, lancer et intercepter vos propres classes d'exception**,
en utilisant une class dont l'invariant (la note/grade doit rester dans `[1, 150]`) est
garanti par le lancement d'exceptions.

### Nouveaux termes

- **Exception** — un objet lancé pour signaler une erreur. Ici, deux
  d'entre elles : `GradeTooHighException`, `GradeTooLowException`.
- **`std::exception`** — la base class standard pour toutes
  les exceptions. Sa seule interface est `virtual const char* what()
  const throw();` — un message lisible par un être humain.
- **Nested class** — une class déclarée *à l'intérieur* d'une autre
  (`Bureaucrat::GradeTooHighException`). Elle restreint la portée de l'exception à
  son propriétaire, ce qui est très lisible et correspond au sujet.
- **Invariant** — une règle qui doit *toujours* être vérifiée pour qu'un objet soit
  valide. Ici : `1 <= grade <= 150`. Le grade **1 est le plus haut**,
  **150 le plus bas** — contre-intuitif, comme aux échecs, dans les rangs
  militaires ou l'expression « tu es mon numéro 1 ».

### Image de la vie réelle
Pensez à une **hiérarchie d'entreprise ou militaire**. Un `Bureaucrat` possède un
nom (fixé à vie — `const`) et un rang. Vous pouvez le *promouvoir*
(incrémenter → le numéro de grade **diminue**, vers 1) ou le *rétrograder*
(décrémenter → le numéro de grade **augmente**, vers 150). Essayez de promouvoir
quelqu'un déjà au grade 1, ou de rétrograder quelqu'un au grade 150, et le
système refuse — il **lance une exception** (`throw`).

### L'extrait de code fondamental — déclarer une classe d'exception

C'est le pattern que vous réutiliserez dans chaque exercice. Remarquez le
`throw()` après `what()` — en **C++98**, la fonction `what()` du standard
porte une exception-specification vide, et pour l'overload proprement,
votre signature doit correspondre :

```cpp
#include <exception>

class Bureaucrat {
public:
    class GradeTooHighException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
    class GradeTooLowException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
    // ... rest of Bureaucrat
};
```

```cpp
// in the .cpp
const char* Bureaucrat::GradeTooHighException::what() const throw() {
    return "grade is too high (must be >= 1)";
}
```

### Lancer et intercepter

```cpp
// throwing — anywhere the invariant would break:
if (grade < 1)
    throw Bureaucrat::GradeTooHighException();

// catching — in main (or any caller):
try {
    Bureaucrat boss("Alice", 0);   // 0 is out of range → throws
}
catch (std::exception& e) {        // catch by reference to the BASE
    std::cerr << e.what() << std::endl;
}
```

Interceptez par **`std::exception&`** (la base, par reference) : un seul handler
couvre à la fois vos exceptions *et* celles de la STL, et la reference
évite le slicing. Le sujet exige explicitement que vos
exceptions soient interceptables de cette manière.

### Où résident les throws
- **Constructor** — validez le grade ; lancez une exception s'il sort de `[1,150]`.
  (Lancer une exception depuis un ctor ne pose aucun problème — l'objet n'est simplement
  jamais considéré comme construit, et aucun destructor ne s'exécute pour lui.)
- **`increment` / `decrement`** — lancez une exception *avant* de franchir la
  limite.

### Astuces
- Surchargez `operator<<` (free function, `std::ostream&`) pour afficher
  `<name>, bureaucrat grade <grade>.` — renvoyez le stream pour que les appels
  puissent s'enchaîner.
- Donnez un message de `what()` au niveau de la class qui soit réellement utile ; vous
  l'affichez, faites en sorte qu'il soit lisible.
- Testez **à la fois** le cas nominal et le cas levant une exception dans le `main`,
  chacun enveloppé dans son propre `try/catch`.

### Piège
**Le nom est `const`** (`const std::string _name`). Dès lors qu'un
membre est `const`, le **copy-assignment operator** généré par le compiler
est virtuellement supprimé — vous ne pouvez pas réassigner un
membre `const`, donc `a = b;` ne compilera pas si vous essayez d'assigner `_name`.
Options : écrire l'`operator=` pour copier uniquement les membres non-const (et
accepter que le nom ne changera pas), ou omettre l'assignation si le `main`
ne l'utilise jamais. Décidez délibérément — ne vous laissez pas surprendre.
*(Attention : votre `ex00/Bureaucrat.hpp` actuel a encore un
`_name` non-`const` et pas encore de `_grade` — mettez-le en conformité avec le
sujet.)*

---

## Exercice 01 — Le Formulaire (Form)

### Ce que vous apprenez
**Des objets qui interagissent via les exceptions et les permissions** — une
class (`Form`) lance des exceptions, une autre (`Bureaucrat`) intercepte et rapporte l'erreur.

### Nouveaux termes

- **`beSigned(const Bureaucrat&)`** — une méthode sur `Form` qui
  compare le grade du bureaucrate au grade requis par le formulaire pour être
  *signé*. Assez élevé → passe `signed = true` ; trop bas →
  lance `Form::GradeTooLowException`.
- **Grade requis** — chaque formulaire possède deux grades `const` : un pour
  le **signer**, un pour l'**exécuter** (utilisé dans l'ex02). Tous deux fixés à la
  construction, tous deux validés (lancer une exception s'ils sont hors de `[1,150]`).

### Image de la vie réelle
Un **bon de commande**. Il reste non signé. Un responsable dont
l'autorité (le grade) est suffisante peut le signer ; un employé junior
ne le peut pas — le formulaire « refuse sa signature ». Le formulaire lui-même détient
la règle définissant *qui est autorisé* ; l'employé se contente d'essayer et
de gérer le refus.

### Structure de la class
- `Form` — `const std::string _name`, `bool _signed` (false à la
  construction), `const int _gradeToSign`, `const int _gradeToExec`.
  Tous **private** (le sujet est explicite : private, pas
  protected — cela aura son importance dans l'ex02).
- Les deux mêmes classes d'exception imbriquées que dans `Bureaucrat`, mais avec la portée de
  `Form`.
- `Bureaucrat::signForm(Form&)` — appelle `beSigned`, et affiche
  soit `<bureaucrat> signed <form>`
  soit `<bureaucrat> couldn't sign <form> because <reason>.`

### L'extrait d'interaction

```cpp
void Bureaucrat::signForm(Form& f) {
    try {
        f.beSigned(*this);
        std::cout << _name << " signed " << f.getName() << std::endl;
    }
    catch (std::exception& e) {
        std::cout << _name << " couldn't sign " << f.getName()
                  << " because " << e.what() << "." << std::endl;
    }
}
```
Remarquez la division du travail : `Form::beSigned` **throws** ;
`Bureaucrat::signForm` **décide de ce qu'il faut faire** (afficher un
message explicite, ne pas crasher). Cette séparation est la raison d'être
des exceptions.

### Comparaison des échelons — le piège mental
« Suffisamment élevé » signifie que le **numéro d'échelon est suffisamment petit**. Un
bureaucrate peut signer si `bureaucrat.grade <= form.gradeToSign`
(chiffre plus bas = plus d'autorité). Écrivez-le une fois, attentivement, et
commentez le `<=` si nécessaire — c'est le bug logique nº 1 de ce
module.

### Astuces
- L'`operator<<` pour `Form` doit afficher tout son état (nom, statut
  signé, les deux échelons) — pratique pour le débogage.
- `_signed` démarre à `false` ; seul `beSigned` le modifie.
- Les getters sont `const` ; le fait que les membres d'échelon soient `const` signifie (comme dans
  l'ex00) que votre `operator=` ne peut copier que `_signed`.

### Piège à éviter
Forward declaration / ordre des includes : `Form` a besoin de connaître
`Bureaucrat` et vice-versa. Utilisez une **forward declaration**
(`class Bureaucrat;`) dans un header et incluez la définition complète
dans le `.cpp` pour briser l'inclusion circulaire (`#include`).

---

## Exercice 02 — Formulaire abstrait & formulaires concrets

### Ce que vous apprenez
**Combiner les exceptions avec le polymorphisme** : `Form` devient
**abstraite** (`AForm`), et chaque formulaire concret effectue sa propre action
lors de son exécution — mais les *vérifications de permissions* ne résident qu'une seule fois, dans la base.

### Nouveaux termes

- **`AForm`** — la base abstraite (renommage de `Form`). Ajoutez une action **pure
  virtual** afin que la class ne puisse pas être instanciée directement :
  ```cpp
  virtual void execute(Bureaucrat const& executor) const = 0;
  ```
  (Ou gardez `execute` concret dans la base pour les vérifications et créez une fonction
  `action()` protected et pure-virtual — voir le pattern élégant
  ci-dessous.)
- **Formulaires concrets** — trois sous-classes, chacune avec ses propres échelons
  de signature/exécution et sa propre action :

  | Form | sign | exec | Action |
  |---|---|---|---|
  | `ShrubberyCreationForm` | 145 | 137 | écrit `<target>_shrubbery` avec des arbres en ASCII |
  | `RobotomyRequestForm` | 72 | 45 | bruits de perceuse ; robotomise la cible **50%** du temps |
  | `PresidentialPardonForm` | 25 | 5 | annonce que la cible est pardonnée par Zaphod Beeblebrox |

- **Nouvelles exceptions** souhaitables : quelque chose comme
  `AForm::FormNotSignedException` et une vérification d'échelon d'exécution qui
  réutilise `GradeTooLowException`.

### Image de la vie réelle
Un **bureau de l'administration avec différentes démarches administratives**. Chaque formulaire partage
la même règle d'accueil : *« est-il signé ? votre habilitation est-elle assez élevée
pour l'exécuter ? »* — cette vérification est identique pour tous les formulaires, elle
se trouve donc à l'accueil (la base class). Mais *ce que le formulaire
fait réellement* — planter un arbuste, lobotomiser quelqu'un, accorder
un pardon — est spécifique à chaque formulaire (l'override).

### Le pattern élégant (le sujet y fait allusion)
Faites les **vérifications une seule fois** dans la base, puis déléguez le travail spécifique
à un hook virtuel. C'est le design pattern Template-Method :

```cpp
// AForm.cpp — concrete, in the base:
void AForm::execute(Bureaucrat const& executor) const {
    if (!_signed)
        throw AForm::FormNotSignedException();
    if (executor.getGrade() > _gradeToExec)
        throw AForm::GradeTooLowException();
    this->action();        // ← pure virtual, each form overrides
}
```

```cpp
// each concrete form:
protected:
    virtual void action() const;   // does the shrubbery/robotomy/pardon
```

Vous écrivez la vérification d'échelon/signature **une seule fois** ; chaque nouveau formulaire ne
fournit que son `action()`. Comparez cela au fait de copier-coller les deux mêmes
`if` dans trois fonctions `execute()` — la version avec la base-class explique pourquoi le
sujet la qualifie de « plus élégante ».

### Accéder aux membres privés de la base
Les attributs de `AForm` sont **private**, mais les formulaires concrets ont besoin
de leurs noms/échelons. Définissez-les en passant les valeurs **au constructeur de la base**
dans la liste d'initialisation ; lisez-les via des **getters protected**. Ne rendez pas les membres
`protected` — c'est exactement ce que le sujet interdit.

```cpp
ShrubberyCreationForm::ShrubberyCreationForm(std::string const& target)
    : AForm("ShrubberyCreationForm", 145, 137), _target(target) {}
```

### Astuces
- **Destructeur virtuel sur `AForm`** (réflexe du Mod 04) — vous
  supprimez les formulaires concrets à travers un `AForm*`.
- `ShrubberyCreationForm` utilise `<fstream>` (`std::ofstream`) pour
  écrire le fichier — RAII garantit que le fichier est fermé même si une
  exception est levée.
- Les 50% de `RobotomyRequestForm` — un `std::rand()` initialisé avec une graine une seule fois ; ne
  sur-concevez pas l'aléatoire pour un exercice de piscine.
- Ajoutez `Bureaucrat::executeForm(AForm const&)` en miroir de `signForm` :
  tentez `execute`, affichez `<bureaucrat> executed <form>` ou l'erreur.

### Piège à éviter
Une reference sur `AForm` peut pointer vers n'importe quel formulaire concret —
`execute()` dispatche virtuellement vers la bonne méthode `action()`. Mais si
vous avez oublié le destructeur virtuel, faire un delete via un `AForm*` provoquera une fuite de
la partie concrète. Valgrind vous le signalera.

---

## Exercice 03 — Le Stagiaire

### Ce que vous apprenez
**Un dispatch propre sans cascades de `if/else if/else`** — instancier un
`AForm*` du bon type à partir d'un *nom sous forme de chaîne de caractères*.

### Nouveaux termes

- **`Intern`** — une class sans nom et sans échelon. Une méthode :
  `AForm* makeForm(std::string const& formName, std::string const&
  target);`. Retourne un formulaire alloué sur la heap du type correspondant, ou
  gère le cas du « nom de formulaire inconnu » (affiche une erreur / retourne
  `0`). Affiche `Intern creates <form>` en cas de succès.

### Image de la vie réelle
Vous vous approchez du **stagiaire** et lui dites : *« trouve-moi une demande de robotomie
pour Bender. »* Peu importe la *façon* dont il trouve le bon formulaire dans le
classeur — il vous tend le bon, rempli avec votre cible.
`makeForm("robotomy request", "Bender")` → un `RobotomyRequestForm*`.

### L'approche interdite
Le sujet **interdit explicitement** ceci :

```cpp
// ❌ do NOT do this — the subject penalises it:
if (name == "shrubbery creation") return new ShrubberyCreationForm(target);
else if (name == "robotomy request") return new RobotomyRequestForm(target);
else if (name == "presidential pardon") return new PresidentialPardonForm(target);
```

### Le pattern propre — une table de correspondance de pointeurs de fonctions
Associez chaque nom à une **fonction de création**, faites une seule boucle, appelez via un
pointeur. C'est la technique que l'exercice évalue ; l'implémentation exacte
vous appartient :

```cpp
// a private static helper per form type — same signature:
//   static AForm* makeShrubbery(std::string const& target)
//       { return new ShrubberyCreationForm(target); }

AForm* Intern::makeForm(std::string const& name, std::string const& target) {
    std::string       names[]  = { "shrubbery creation",
                                   "robotomy request",
                                   "presidential pardon" };
    AForm* (*makers[])(std::string const&) = { /* &makeShrubbery, ... */ };

    for (int i = 0; i < 3; ++i)
        if (names[i] == name)
            return makers[i](target);

    // unknown name: print error, return 0
    return 0;
}
```

Pourquoi c'est mieux : ajouter un quatrième formulaire = ajouter une ligne à chaque tableau,
ne rien toucher d'autre. Aucune logique de branchement à relire.

### Astuces
- **Qui supprime le formulaire ?** `makeForm` fait un `new` dessus ; l'appelant en est le propriétaire
  et doit faire le `delete`. Testez cela dans le `main` et vérifiez avec
  valgrind.
- Le lecteur du `AForm*` retourné le signe et l'exécute généralement —
  c'est le test d'intégration complet du module.
- `AForm* (*makers[])(std::string const&)` est un type intimidant ; un
  `typedef` le rend lisible :
  ```cpp
  typedef AForm* (*FormMaker)(std::string const&);
  ```

### Piège à éviter
En cas de **nom de formulaire inconnu**, ne faites aucun `new` et ne créez pas de fuite —
affichez simplement un message et retournez `0`. Et protégez le code appelant : si `makeForm`
peut retourner `0`, l'appelant doit vérifier avant de faire `delete`/`execute`.

---

## Ce que ce module vous apporte

- Vous savez déclarer, throw et catch des exceptions personnalisées, et vous connaissez
  la signature C++98 `what() const throw()` par cœur.
- Vous faites vos catch par `std::exception&` (base, par reference) et savez pourquoi
  (un seul handler, pas de slicing).
- Vous comprenez *pourquoi* le RAII et les exceptions sont intimement liés :
  les destructeurs s'exécutent pendant le stack unwinding, donc les ressources sont protégées.
- Vous avez constaté ce qu'un membre `const` implique pour votre OCF, et avez fait un
  choix délibéré concernant l'`operator=`.
- Vous pouvez remplacer une cascade de dispatch de type en `if/else` par un
  tableau de pointeurs de fonctions sans hésiter.

Après le Mod 05, « gérer l'erreur là où cela a du sens, pas là où elle
se produit » devient un réflexe — et vous êtes prêt pour les templates (Mod 06/07)
où la STL lèvera des exceptions en permanence.