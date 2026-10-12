# Makefiles pour le C++ — Guide complet

> **Zoom sur les mots-clés :** [`INLINE`](../../lexique/INLINE.md) · [`EXTERN`](../../lexique/EXTERN.md) · [`STATIC`](../../lexique/STATIC.md)

Les modules CPP de 42 attendent tous la même structure de Makefile : une cible pour le binaire, `clean`, `fclean`, `re`, des flags stricts, et aucun relink lorsque rien n'a changé. Ce guide présente ce modèle, détaille chaque composant et aborde les particularités qui surviennent lors de la compilation en C++ (templates, suivi des dépendances et gestion des headers).

---

## Table des matières

1. [Le Makefile C++ canonique de 42](#1.%20The%20canonical%2042%20C%2B%2B%20Makefile)
2. [Explication des flags](#2.%20Flags%20explained)
3. [Comment `make` décide de recompiler](#3.%20How%20make%20decides%20to%20rebuild)
4. [Les quatre règles obligatoires](#4.%20The%20four%20mandatory%20rules)
5. [Suivi des dépendances des headers](#5.%20Header%20dependency%20tracking)
6. [Pourquoi les templates cassent les Makefiles naïfs](#6.%20Why%20templates%20break%20naive%20Makefiles)
7. [Fichiers objets dans un dossier de build](#7.%20Object%20files%20in%20a%20build%20directory)
8. [Règles de pattern et variables automatiques](#8.%20Pattern%20rules%20and%20automatic%20variables)
9. [`.PHONY` — pourquoi c'est important](#9.%20.PHONY%20%E2%80%94%20why%20it%20matters)
10. [Couleurs et affichage de progression](#10.%20Colors%20and%20progress%20output)
11. [Déboguer un Makefile](#11.%20Debugging%20a%20Makefile)
12. [Règles empiriques](#12.%20Rules%20of%20thumb)
13. [Pièges courants](#13.%20Gotchas)

---

## 1. Le Makefile C++ canonique de 42

```make
NAME     := program

CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := build

SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS     := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
```

Chaque ligne de ce fichier mérite sa place. Voici la justification pour chacune d'elles ci-dessous.

---

## 2. Explication des flags

```
-Wall     — active la plupart des warnings courants
-Wextra   — active davantage de warnings (ceux que -Wall a manqués)
-Werror   — traite les warnings comme des erreurs (exigé à 42)
-std=c++98 — restreint le langage au C++98 (exigé à 42 pour CPP 00–08)
```

Vous pouvez aussi rencontrer :

```
-pedantic        — avertit en cas d'extensions non standard ; bon pour la portabilité
-Wshadow         — avertit lorsqu'une variable locale masque un champ (shadowing)
-Wnon-virtual-dtor — avertit lorsqu'une class avec des méthodes virtual n'a pas de destructeur virtual (utile !)
-fsanitize=address — AddressSanitizer — détecte les UB à l'exécution ; voir DEBUGGING_CPP.md
-g               — inclut les informations de debug (pour lldb/gdb)
-O2              — optimise (pour la performance) ; à éviter pendant le débogage
```

Erreur fréquente : livrer avec `-O2 -g` et s'étonner que le débogage soit étrange. Utilisez `-O0 -g` pendant le développement.

### Choix du compiler

`c++` sur les machines de 42 est un alias vers `clang++` sur macOS et `g++` sur Linux. Utilisez `c++` dans le Makefile — cela fonctionne sur les deux. Ne fixez pas en dur `clang++` ou `g++` sans raison spécifique.

---

## 3. Comment `make` décide de recompiler

La règle fondamentale de `make` : **reconstruire une cible si l'un de ses prérequis est plus récent que la cible** (ou si la cible n'existe pas). Tout découle de là.

```make
program: main.o utils.o
	c++ main.o utils.o -o program

main.o: main.cpp
	c++ -c main.cpp -o main.o
```

Si vous modifiez `main.cpp`, `main.o` est plus ancien que lui → reconstruction de `main.o` → `program` est plus ancien que le nouveau `main.o` → relink. Rien d'autre n'est réexécuté.

Cela fonctionne grâce aux **horodatages des fichiers** (timestamps). Si votre horloge est déréglée ou si vous synchronisez des fichiers avec rsync en perdant les dates de modification (mtime), vous obtiendrez des recompilations inutiles ou — pire — des recompilations *oubliées*.

---

## 4. Les quatre règles obligatoires

42 exige `all`, `clean`, `fclean`, `re` :

| Règle | Rôle |
|---|---|
| `all` (ou simplement `make`) | Compile le binaire. Ne recompile rien de ce qui est déjà à jour. |
| `clean` | Supprime les fichiers objets et les fichiers de dépendances. Conserve le binaire. |
| `fclean` | `clean` + suppression du binaire. |
| `re` | `fclean` puis `all`. |

Et `re` n'est **jamais** `$(MAKE) fclean && $(MAKE) all` avec `&&` — ce sont deux cibles de dépendance ordonnées. La nuance importe quand des sous-makes interviennent, mais pour des projets à dossier unique, les deux fonctionnent.

---

## 5. Suivi des dépendances des headers

Le problème subtil : si vous modifiez un `.hpp`, quels fichiers `.cpp` doivent être recompilés ? Un Makefile naïf l'ignore — il ne suit que `.cpp → .o`. Modifiez `Animal.hpp` et rien ne se reconstruit ; la prochaine exécution du programme utilisera du code obsolète.

**La solution — les fichiers de dépendances générés automatiquement.** `-MMD -MP` indique au compiler de générer un fichier `.d` aux côtés de chaque `.o` :

```
build/main.o: src/main.cpp include/Animal.hpp include/Dog.hpp
```

Ensuite, la dernière ligne du Makefile :
```make
-include $(DEPS)
```
fusionne ces extraits de dépendances dans le graphe de build. Désormais, `make` sait que `main.o` dépend d'`Animal.hpp` ; modifier le header entraîne la recompilation de `main.o`.

### Que signifient `-MMD` et `-MP` ?

- **`-MM`** — génère les dépendances Makefile pour cette source, **en excluant** les headers système (`<iostream>`, `<string>`). Sans cela, chaque `.d` listerait `/usr/include/...` — ce qui est lent et fragile.
- **`-MMD`** — identique à `-MM`, mais effectue également la compilation normale (-MM seul génère les dépendances sans produire de `.o`).
- **`-MP`** — ajoute une règle "phony" vide pour chaque header. Cela évite les erreurs du type "no rule to make target `<header>.hpp`" si vous renommez un header.

Le tiret initial sur `-include` signifie "ne pas échouer si les fichiers `.d` n'existent pas encore" (au premier build).

---

## 6. Pourquoi les templates cassent les Makefiles naïfs

Les templates sont instanciés à chaque point d'`#include`. Cela implique que :

- Modifier le corps d'une méthode templated dans un header **force la recompilation de chaque .cpp qui l'utilise.**
- Votre Makefile doit en être informé. D'où l'utilisation de `-MMD -MP` (§5).

Si vous omettez le suivi des dépendances, vous pouvez modifier une méthode templated sans que le compiler ne soit jamais réexécuté sur le site d'appel — vous vous retrouvez avec un binaire obsolète mélangeant d'anciens et de nouveaux fichiers objets.

C'est pourquoi **chaque Makefile C++ professionnel utilise une forme ou une autre de dépendances générées automatiquement.** Le modèle ci-dessus est le standard.

### Fichiers `.tpp`

Si vous séparez les templates en `Array.hpp` + `Array.tpp` (voir [TEMPLATES.md §8](../advanced/TEMPLATES.md)), le `.tpp` est `#include` depuis le `.hpp`. `-MMD` détecte cette chaîne — aucune configuration supplémentaire n'est requise dans le Makefile.

---

## 7. Fichiers objets dans un dossier de build

Placer les fichiers `.o` à côté des fichiers `.cpp` encombre l'arbre des sources et complique l'utilisation de `git`. Déplacez-les dans `build/` :

```
project/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── Animal.cpp
│   └── Dog.cpp
├── include/
│   ├── Animal.hpp
│   └── Dog.hpp
└── build/               ← généré, ignoré par git
    ├── main.o
    ├── main.d
    ├── Animal.o
    └── ...
```

Et votre `.gitignore` :
```
build/
program
```

### Comment le Makefile mappe les chemins

```make
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
# SRCS = src/main.cpp src/Animal.cpp ...

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# substitution src/%.cpp → build/%.o
# OBJS = build/main.o build/Animal.o ...
```

Et la règle :
```make
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@
```

- `%.o: %.cpp` — règle de pattern : pour n'importe quel `.o`, trouver le `.cpp` correspondant.
- `| $(OBJ_DIR)` — le pipe marque `$(OBJ_DIR)` comme un **order-only prerequisite**. Il s'assure que le dossier existe, mais ne recompile pas le `.o` simplement parce que le mtime du dossier a changé.
- `$<` — le premier prérequis (le fichier `.cpp`).
- `$@` — la cible (le fichier `.o`).
- `-c` — compiler uniquement, sans linker.

---

## 8. Règles de pattern et variables automatiques

Les variables automatiques les plus utiles :

| Variable | Signification |
|---|---|
| `$@` | La cible |
| `$<` | Le premier prérequis |
| `$^` | Tous les prérequis, séparés par un espace, doublons supprimés |
| `$?` | Les prérequis plus récents que la cible |
| `$*` | La racine (stem) d'une règle de pattern (la partie capturée par `%`) |

Exemple :

```make
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
#                     ^^  ^^
#                     |   |
#                     |   le nom du binaire (ex. program)
#                     tous les objets
```

Utiliser `$^` plutôt que `$(OBJS)` apporte un léger gain en robustesse : si vous ajoutez un autre prérequis ultérieurement, cela fonctionnera sans modification.

---

## 9. `.PHONY` — pourquoi c'est important

`.PHONY: all clean fclean re` déclare que ces cibles **ne sont pas des fichiers**. Sans cela, si un fichier nommé `clean` se trouve par hasard dans votre répertoire, `make clean` ne fera rien silencieusement (la "cible" étant déjà à jour — le fichier existe et n'a pas de prérequis).

`.PHONY` force `make` à toujours exécuter la recette, quel que soit l'état du système de fichiers.

---

## 10. Couleurs et affichage de progression

Pour la lisibilité et l'esthétique, vous pouvez ajouter des séquences d'échappement de couleur simples :

```make
GREEN := \033[32m
RESET := \033[0m

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@printf "$(GREEN)Compiling$(RESET) %s\n" $<
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@
```

Le `@` initial devant une commande **désactive son écho**. Associé à `printf`, vous obtenez une ligne de progression épurée au lieu de la commande brute du compiler. La plupart des dépôts de 42 adoptent une variante de cette approche.
---

## 11. Déboguer un Makefile

Quelques fonctionnalités intégrées sont salvatrices.

### Voir ce qui va s'exécuter

```
make -n                    — print commands but don't execute
```

### Afficher les variables développées

```make
print-%:
	@echo '$*=$($*)'
```

Ensuite, `make print-SRCS` affiche ce en quoi `SRCS` a été développé.

### Trace complète

```
make --trace              — print every decision (GNU make 4.0+)
make -d                   — verbose debugging (noisy but thorough)
```

### « J'ai modifié un header et rien ne s'est reconstruit »

- Avez-vous `-MMD` dans la commande de compilation ? Exécutez `make fclean && make` et inspectez `build/*.d` pour voir si les fichiers de dépendances ont été générés.
- La ligne `-include $(DEPS)` est-elle présente *et* atteinte (pas à l'intérieur d'une conditionnelle) ?
- Les fichiers `.d` ont-ils été supprimés par `clean` alors que les fichiers `.o` ont survécu ? Assurez-vous que `clean` supprime les deux.

---

## 12. Règles de base

- **Utilisez `-Wall -Wextra -Werror -std=c++98`.** Ne les assouplissez pas, sauf si le sujet l'exige.
- **Utilisez toujours `-MMD -MP`.** Sans cela, une modification de header casse silencieusement le build.
- **Gardez les fichiers objets en dehors de l'arborescence des sources** (répertoire `build/`).
- **Déclarez les cibles phony.** `all clean fclean re` au minimum.
- **Ne touchez pas au compiler manuellement.** Laissez `make` décider de ce qui doit être reconstruit.
- **Versionnez le `Makefile`, pas sa sortie.** Le binaire et les fichiers `.o` ont leur place dans `.gitignore`.
- **Ne faites jamais `rm -rf $(NAME)`** avec une variable `NAME` qui pourrait être vide. Assurez-vous que `fclean` utilise `-f` pour éviter les erreurs quand le binaire n'existe pas.

---

## 13. Pièges à éviter

- **Des tabulations, pas des espaces.** Chaque ligne de recette doit commencer par une tabulation littérale. Les éditeurs qui convertissent les tabulations en espaces casseront silencieusement votre Makefile.
- **`make` ne met absolument rien en cache concernant le *contenu* des fichiers** — uniquement les timestamps. Si deux fichiers ont le même mtime, le résultat est défini par l'implémentation.
- **`$(SRCS)` via `wildcard` est évalué au moment de la lecture du Makefile.** Si vous faites `touch newfile.cpp` et lancez `make`, il *sera* pris en compte (le wildcard est réévalué à chaque invocation). Mais les fichiers que vous créez **en cours de build** ne sont pas pris en compte lors de cette même invocation.
- **Les chemins dans `-I` n'acceptent pas `~` ou `$HOME`** de manière fiable. Utilisez des chemins relatifs.
- **`clean` peut entrer en race condition avec le build** si vous lancez `make clean` alors qu'un `make` est en cours. Ne le faites pas.
- **Les prérequis order-only (`|`) ne propagent pas le mtime.** C'est intentionnel — vous voulez que le répertoire existe, sans déclencher de rebuilds lorsque son mtime change.
- **`@` devant une commande la masque.** Pratique pour l'aspect visuel, pénible pour le débogage. Si vous ne comprenez pas ce que fait `make`, retirez temporairement le `@`.
- **Oublier `-std=c++98`** permet au compiler d'accepter silencieusement des fonctionnalités C++11 (range-for, `auto`, lambdas). Votre code compilera sur votre machine et échouera lors de l'évaluation 42.
- **La règle par défaut pour `.cpp → .o` dans GNU make utilise `$(CXX)` et `$(CXXFLAGS)`.** Souvent, vous n'avez pas besoin d'écrire votre propre pattern rule — mais en écrire une explicitement est plus clair et portable d'une implémentation de make à l'autre.