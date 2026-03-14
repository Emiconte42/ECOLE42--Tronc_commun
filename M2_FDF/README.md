*This project has been created as part of the 42 curriculum by emiconte.*

# FDF - Version francaise

## Description

FDF est un moteur de rendu 3D en fil de fer (wireframe) permettant d’afficher des cartes d’altitude stockées dans des fichiers `.fdf`.

Le programme lit un fichier contenant des valeurs de hauteur, projette la grille en vue isométrique, puis affiche une représentation en fil de fer colorée à l’aide de la bibliothèque graphique MinilibX.

Les principaux objectifs de ce projet sont :
- Comprendre la projection 3D vers 2D
- Implémenter un algorithme de tracé de lignes
- Gérer des événements graphiques
- Manipuler la mémoire en C de manière rigoureuse

Le projet respecte strictement la Norme 42 et est compatible avec norminette.

---

## Instructions

### Compilation

make

### Exécution

./fdf maps/42.fdf

### Nettoyage

make clean
make fclean
make re

---

## Contrôles

- W, A, S, D ou flèches directionnelles → Déplacement
- + / - → Zoom avant / arrière
- Molette de la souris → Zoom centré sur le curseur
- SPACE → Recentrer la vue
- ESC → Quitter le programme

---

## Architecture

Le projet est organisé en modules distincts afin de garder une structure claire et maintenable :

- Parsing : lecture et validation du fichier `.fdf`
- Modèle de données : stockage de la carte sous forme de grille structurée
- Projection : conversion des coordonnées 3D en espace 2D isométrique
- Rendu : calcul des positions à l’écran
- Dessin : tracé des lignes à l’aide de l’algorithme DDA (Digital Differential Analyzer)
- Événements : gestion du clavier et logique de redraw contrôlée

Le rendu est effectué via un buffer image afin d’assurer un affichage fluide et des mises à jour propres de la fenêtre.

---

## Choix techniques

- Implémentation manuelle de la projection isométrique
- Tracé des lignes avec l’algorithme DDA
- Interpolation des couleurs entre les points
- Gestion stricte des erreurs (arguments invalides, fichiers incorrects, permissions)
- Vérification de la mémoire avec valgrind (aucune fuite)
- Code entièrement conforme à la Norme 42

---

## Gestion des erreurs

Le programme gère correctement :

- Absence d’argument
- Trop d’arguments
- Fichier inexistant
- Erreurs de permissions
- Format de carte invalide
- Échecs d’allocation mémoire

Tous les cas d’erreur provoquent une sortie avec le code 1.

---

## Ressources

Documentation MinilibX (Linux) :
https://harm-smits.github.io/42docs/libs/minilibx

Projection isométrique :
https://fr.wikipedia.org/wiki/Projection_isom%C3%A9trique

Algorithme DDA :
https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)

Sujet officiel FDF :
https://github.com/42Paris/42cursus-fdf

---

## Utilisation de l’IA

L’IA a été utilisée comme assistant pédagogique pendant le développement du projet. Elle a servi principalement à :

- Clarifier les notions mathématiques liées à la projection
- Approfondir la compréhension de la logique de rendu
- Réfléchir aux choix d’architecture

Aucun code source de production n’a été généré automatiquement.


# FDF - English version

## Description

FDF is a 3D wireframe renderer that displays terrain heightmaps stored in `.fdf` files.

The program parses a map file containing altitude values, projects the grid into an isometric view, and renders a colored wireframe representation using the MinilibX graphical library.

The main goals of this project are:
- Understanding 3D to 2D projection
- Implementing a line rasterization algorithm
- Managing graphical events
- Handling memory safely in C

The project strictly follows the 42 Norm and passes norminette.

---

## Instructions

### Compilation

make

### Execution

./fdf maps/42.fdf

### Cleaning

make clean
make fclean
make re

---

## Controls

- W, A, S, D or arrow keys → Move
- + / - → Zoom in / out
- Mouse wheel → Zoom centered on cursor
- SPACE → Recenter view
- ESC → Exit program

---

## Architecture

The project is organized into separate modules to keep the logic clear and maintainable:

- Parsing: reading and validating the `.fdf` file
- Data model: storing the map in a structured grid
- Projection: converting 3D coordinates to isometric 2D space
- Rendering: computing screen positions
- Drawing: line rasterization using the DDA (Digital Differential Analyzer) algorithm
- Events: keyboard handling and controlled redraw logic

Rendering is performed through an image buffer to ensure efficient redraw and clean window updates.

---

## Technical Choices

- Manual implementation of isometric projection
- Line drawing implemented using the DDA algorithm
- Color interpolation between points
- Strict error handling (invalid files, argument checks, permission errors)
- Memory management verified with valgrind (no leaks)
- Fully compliant with the 42 Norm

---

## Error Handling

The program properly handles:

- No argument
- Too many arguments
- Non-existing file
- Permission errors
- Invalid map format
- Memory allocation failures

All error cases exit with status code 1.

---

## Resources

MinilibX documentation (Linux):
https://harm-smits.github.io/42docs/libs/minilibx

Isometric projection overview:
https://en.wikipedia.org/wiki/Isometric_projection

Digital Differential Analyzer (DDA) algorithm:
https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)

42 FDF subject reference:
https://github.com/42Paris/42cursus-fdf

---

## AI Usage

AI was used as a learning and mentoring assistant during the development of this project. It was mainly used to:

- Clarify projection mathematics
- Understand rendering logic
- Review architectural decisions

No production source code was generated automatically.
