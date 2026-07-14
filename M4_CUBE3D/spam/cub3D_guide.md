# cub3D — Mon plan de bataille

*Un seul fil à suivre, une étape à la fois. Je ne passe à la suite que quand l'étape d'avant tourne vraiment.*

---

## Comment lire ce document

- La partie **PLAN** explique chaque étape : ce que je dois faire, pourquoi, et ce qu'il faut savoir avant de coder.
- La partie **TODOLIST** à la fin est la liste de courses : je la garde ouverte, et je coche au fur et à mesure.
- Règle d'or : **je ne code jamais une étape sans avoir testé et validé la précédente.** Une étape qui "marche à peu près" n'est pas terminée.

---

## Avant de commencer : le Garbage Collector (GC)

Utiliser un garbage collector: Voici ce que ça veut dire concrètement, sans complexité inutile.

**Le problème que ça résout :** en C, chaque `malloc()` doit être suivi d'un `free()` un jour. Dans un projet comme cub3D, j'alloue plein de choses (lignes de map, textures, structures...). Si j'oublie un `free()`, c'est une fuite mémoire. Si je quitte le programme à cause d'une erreur en plein milieu du parsing, je dois quand même tout libérer correctement avant de faire `exit()`.

**Le principe d'un GC maison :**

1. Je crée une structure simple qui garde une **liste chaînée de tous mes pointeurs alloués**.
2. Chaque fois que je fais un `malloc()`, au lieu d'appeler `malloc()` directement, j'appelle ma propre fonction `ft_gc_calloc()` (dans ma libft) qui :
   - alloue la mémoire normalement,
   - ajoute le pointeur à ma liste de suivi,
   - retourne le pointeur comme un malloc normal.
3. À la fin du programme (succès ou erreur), j'appelle une seule fonction `ft_gc_free_all()` qui parcourt la liste et libère tout, dans n'importe quel ordre.

**Pourquoi c'est puissant pour un projet comme le notre :**

- Plus besoin de me souvenir de chaque `free()` à chaque sortie d'erreur. Si une erreur de parsing arrive à la ligne 50 de mon code, j'appelle juste `gc_free_all()` puis `exit()`, et tout est nettoyé — peu importe combien de choses j'avais déjà allouées.
- Ça évite le pattern classique et fragile : multiplier les `if (x) free(x)` partout dans le code, avec le risque d'en oublier un.

**Ce qu'il faut prévoir dans la structure :**

```
t_gc_node :
    void *ptr        → le pointeur alloué
    t_gc_node *next  → le nœud suivant

t_gc :
    t_gc_node *head  → tête de la liste
```

**Les 3 fonctions à écrire en premier, avant tout le reste du projet :**

- `ft_gc_calloc(t_gc *gc, size_t size)` → alloue + enregistre + retourne le pointeur
- `ft_gc_free_all(t_gc *gc)` → parcourt la liste, free chaque ptr, puis free les nœuds eux-mêmes
- `ft_gc_remove(t_gc *gc, void *ptr)` *(optionnel, utile si tu fais un vrai free avant la fin)* → si jamais je veux libérer un pointeur précis avant la fin sans attendre `gc_free_all`

> ⚠️ Attention : la structure `t_gc` elle-même doit être accessible partout (souvent passée en paramètre, ou un seul pointeur global vers elle — discute avec ton mate de ce qui est toléré côté Norme/variables globales).

> 💡 Conseil : écris et teste ce petit système de GC **isolément**, dans un mini-programme à part, avant de l'intégrer à cub3D. Si le GC a un bug, tout le projet sera instable.

---

## PLAN — Les étapes, dans l'ordre

### Étape 0 — Le Garbage Collector

Voir section au-dessus. Code-le, teste-le seul (alloue 10 trucs, libère tout, vérifie avec valgrind qu'il n'y a aucune fuite). Ne passe à l'étape 1 que quand c'est solide.

### Étape 1 — Parser le fichier `.cub` + structures de données

**But :** lire le fichier, en extraire textures/couleurs/map, sans encore rien afficher.

Ce que je fais :
- Je définis mes structures (`t_game`, `t_player`, `t_map`, `t_tex`, etc. — voir le détail dans le guide complet si besoin).
- J'écris une fonction qui lit le fichier ligne par ligne.
- Pour chaque ligne, je détecte : NO / SO / WE / EA / F / C / ou début de map.
- Je stocke tout dans mes structures.
- Je n'affiche rien encore — je me contente d'un `printf` de debug pour vérifier que j'ai bien lu les bonnes valeurs.

**Test de fin d'étape :** je lance mon programme sur un `.cub` valide et j'affiche en console toutes les infos parsées (chemins textures, couleurs RGB, dimensions de la map). Tout doit correspondre exactement au fichier.

### Étape 1bis — Validation du fichier et de la map

Je ne code pas le rendu avant d'avoir un parsing **qui rejette correctement tous les cas invalides.** Voir la liste de tests complète plus bas dans ce document (section "Cas de test de parsing").

**Test de fin d'étape :** je fais tourner manuellement chaque fichier de test invalide. Chaque fichier doit afficher `Error\n` + un message clair, et **rien ne doit segfault.**

### Étape 2 — Comprendre la miniLibX : fenêtre + pixels

**But :** ouvrir une fenêtre vide, savoir dessiner un pixel, savoir fermer proprement.

Ce que je fais :
- `mlx_init()`, `mlx_new_window()`, j'affiche une fenêtre vide.
- Je crée une image en mémoire, je remplis quelques pixels à la main pour vérifier que je sais écrire dans le buffer.
- Je gère la fermeture : ESC et la croix rouge doivent fermer la fenêtre **sans erreur ni zombie process**.

**Test de fin d'étape :** une fenêtre s'ouvre, affiche un carré de couleur dessiné pixel par pixel, et se ferme proprement avec ESC ou la croix.

### Étape 3 — Minimap 2D

**But :** pratiquer mlx sur un problème simple avant d'attaquer le raycasting.

Ce que je fais :
- J'affiche ma map sous forme de petits carrés colorés (un carré = une case de la map).
- Mur = une couleur, case vide = une autre couleur, position du joueur = une troisième couleur (ou une forme).

**Pourquoi cette étape avant le raycasting :** ça me permet de vérifier que ma map est bien parsée, bien stockée, et que je sais manipuler des pixels et des formes simples — sans la complexité des maths de raycasting.

**Test de fin d'étape :** je vois ma map à l'écran, avec le joueur positionné au bon endroit.

### Étape 4 — Inputs clavier sur la minimap

**But :** faire bouger un point (le joueur) sur la minimap avec le clavier, avant de faire bouger une vraie caméra 3D.

Ce que je fais :
- Je branche les hooks clavier (`mlx_hook` sur KeyPress/KeyRelease).
- WASD déplace le point représentant le joueur sur la minimap.
- Les flèches gauche/droite tournent une petite ligne représentant la direction du regard.
- ESC ferme le programme proprement.

**Test de fin d'étape :** je peux déplacer et faire tourner le joueur sur la minimap avec le clavier, sans lag, sans crash.

### Étape 5 — Raycasting sans texture (murs colorés, sol/plafond)

**But :** la vraie vue 3D, mais en couleurs unies.

Ce que je fais :
- Je remplis le haut de l'écran avec la couleur plafond, le bas avec la couleur sol.
- J'implémente le DDA (l'algorithme qui lance un rayon et trouve le premier mur touché).
- Je calcule la distance perpendiculaire (pas la distance brute, sinon effet "fisheye").
- Je dessine chaque colonne de mur en couleur unie, avec la bonne hauteur.
- Je branche le mouvement (WASD + flèches) sur la vraie caméra 3D cette fois (plus seulement la minimap).

**Test de fin d'étape :** je me déplace dans un couloir en vue 3D, les murs ont une taille cohérente (pas d'étirement bizarre), pas d'effet de poisson (fisheye).

### Étape 6 — Textures sur les murs (NO/SO/EA/WE)

**But :** remplacer les couleurs unies par les vraies textures.

Ce que je fais :
- Je charge les 4 fichiers de texture au démarrage (une seule fois, jamais dans la boucle de rendu).
- Pour chaque colonne de mur, je détermine quel mur a été touché (nord/sud/est/ouest).
- Je calcule quelle colonne de pixels de la texture afficher.

**Test de fin d'étape :** les murs affichent les bonnes textures selon leur orientation, sans effet miroir, sans décalage vertical bizarre.

### Étape 7 — Finitions

**But :** un programme qui ne plante jamais, qui gère toutes les erreurs, et qui ne fuit pas de mémoire.

Ce que je fais :
- Je repasse sur tous les cas d'erreur (mauvais argument, fichier inexistant, mauvaise extension...).
- Je vérifie avec valgrind qu'il n'y a aucune fuite mémoire, même dans les cas d'erreur.
- Je vérifie que `ft_gc_free_all()` est bien appelé sur **tous** les chemins de sortie du programme (succès, erreur, ESC, croix rouge).

**Test de fin d'étape :** valgrind ne signale aucune fuite, et aucun mauvais usage du programme ne provoque un crash.

---

### BONUS (uniquement si le mandatory est parfait)

- Étape 8 — Collisions avec les murs
- Étape 9 — Minimap affichée en overlay dans le rendu 3D
- Étape 10 — Portes (open/close)
- Étape 11 — Sprites animés
- Étape 12 — Rotation à la souris

---

## Cas de test de parsing — ma checklist d'erreurs à gérer

Je crée un fichier `.cub` invalide pour chaque cas, et je vérifie que mon programme répond bien `Error\n` + message, sans jamais crasher :

| Fichier de test | Ce qu'il doit vérifier |
|---|---|
| `broken_wall.cub` | Map non fermée par des murs → doit être rejetée |
| `empty_line.cub` | Ligne vide au milieu de la map → doit être rejetée |
| `invalid_char.cub` | Caractère non autorisé dans la map (ex: `X`, `2`) → rejeté |
| `no_player.cub` | Aucun spawn (N/S/E/W) dans la map → rejeté |
| `too_much_player.cub` | Plusieurs spawns dans la map → rejeté |
| `twice_colors.cub` | Couleur F ou C définie deux fois → rejeté |
| `twice_textures.cub` | Texture NO/SO/EA/WE définie deux fois → rejeté |
| `wrong_ext.cubcubc` | Extension de fichier incorrecte → rejeté |
| `missing_texture.cub` | Une texture (NO/SO/EA/WE) ou couleur (F/C) manquante → rejeté |
| `bad_rgb_range.cub` | Valeur RGB hors de [0,255] (ex: 300 ou -5) → rejeté |
| `bad_rgb_format.cub` | RGB avec un nombre de valeurs incorrect (ex: `F 220,100`) → rejeté |
| `texture_not_found.cub` | Chemin de texture qui n'existe pas sur le disque → rejeté |
| `map_not_last.cub` | La map n'est pas le dernier élément du fichier → rejeté |
| `no_map.cub` | Aucune map dans le fichier → rejeté |
| `unknown_identifier.cub` | Identifiant inconnu (ex: `XY ./texture.xpm`) → rejeté |
| `no_args.cub` | Lancer le programme sans aucun argument → message d'erreur propre |
| `two_args.cub` | Lancer le programme avec plusieurs fichiers en argument → comportement défini et géré |

> 💡 Je garde tous ces fichiers `.cub` invalides dans un dossier `tests/` à part. Ils ne sont pas à rendre, mais ils me sauvent énormément de temps en debug, et serviront pendant la défense.

---

## TODOLIST — Ma liste de courses

Je coche dans l'ordre. Je ne saute jamais une case.

### Préparation
- [ ] Lire le sujet en entier une première fois (sans coder)
- [ ] Mettre en place mon repo Git + Makefile de base (`all`, `clean`, `fclean`, `re`, `bonus`)
- [ ] Écrire et tester mon Garbage Collector seul (mini-programme de test + valgrind)

### Parsing
- [ ] Définir mes structures (`t_game`, `t_player`, `t_map`, `t_tex`, etc.)
- [ ] Lire le fichier ligne par ligne
- [ ] Détecter et stocker NO / SO / EA / WE
- [ ] Détecter et stocker F / C (couleurs)
- [ ] Détecter le début et la fin de la map
- [ ] Stocker la map sous forme de tableau de chaînes
- [ ] Vérifier que la map ne contient que des caractères autorisés
- [ ] Vérifier qu'il y a exactement un spawn (N/S/E/W)
- [ ] Vérifier que la map est fermée par des murs
- [ ] Vérifier les couleurs RGB (format + plage 0-255)
- [ ] Vérifier que les textures et couleurs ne sont pas dupliquées
- [ ] Vérifier que les chemins de texture existent et sont lisibles
- [ ] Vérifier l'extension du fichier (`.cub`)
- [ ] Vérifier le nombre d'arguments du programme
- [ ] Tester tous les fichiers invalides de ma checklist (voir tableau plus haut)
- [ ] Tester avec valgrind : aucune fuite, même en cas d'erreur

### miniLibX — bases
- [ ] Ouvrir une fenêtre vide
- [ ] Créer une image et dessiner des pixels à la main
- [ ] Gérer ESC → fermeture propre
- [ ] Gérer la croix rouge → fermeture propre
- [ ] Vérifier qu'aucun process zombie ne reste après fermeture

### Minimap
- [ ] Afficher la map sous forme de carrés colorés
- [ ] Afficher la position du joueur sur la minimap
- [ ] Brancher WASD pour déplacer le joueur sur la minimap
- [ ] Brancher les flèches gauche/droite pour tourner la direction sur la minimap

### Raycasting (sans texture)
- [ ] Remplir le plafond et le sol avec les couleurs du `.cub`
- [ ] Implémenter le DDA (lancer de rayon, trouver le mur touché)
- [ ] Calculer la distance perpendiculaire (éviter le fisheye)
- [ ] Dessiner les colonnes de murs en couleur unie
- [ ] Brancher le mouvement réel du joueur (WASD) sur la caméra 3D
- [ ] Brancher la rotation (flèches) sur la caméra 3D
- [ ] Tester : déplacement fluide, pas de fisheye, proportions correctes

### Textures
- [ ] Charger les 4 textures au démarrage (une seule fois)
- [ ] Déterminer quelle texture afficher selon le mur touché (N/S/E/W)
- [ ] Calculer la colonne de texture à afficher (`tex_x`)
- [ ] Calculer la ligne de texture à afficher (`tex_y`)
- [ ] Vérifier qu'il n'y a pas d'effet miroir
- [ ] Vérifier qu'il n'y a pas de décalage vertical

### Finitions
- [ ] Repasser sur tous les cas d'erreur d'usage (mauvais argument, fichier inexistant)
- [ ] Vérifier avec valgrind : zéro fuite, dans tous les scénarios (succès, erreur, ESC, croix rouge)
- [ ] Vérifier la Norme sur tout le projet
- [ ] Écrire le README.md (description, instructions, ressources + usage de l'IA)
- [ ] Relire le sujet une dernière fois pour vérifier que rien n'est oublié

### Bonus (seulement si tout le mandatory est parfait)
- [ ] Collisions avec les murs
- [ ] Minimap en overlay dans le rendu 3D
- [ ] Portes (open/close)
- [ ] Sprites animés
- [ ] Rotation à la souris
