# cub3D — Fichiers de test

## Maps VALIDES (`valid/`)

| Fichier | Ce qu'il teste |
|---|---|
| `valid_basic.cub` | Map minimale valide, spawn Nord |
| `valid_spawn_south.cub` | Spawn Sud (S) |
| `valid_spawn_east.cub` | Spawn Est (E) |
| `valid_spawn_west.cub` | Spawn Ouest (W) |
| `valid_spaces_in_map.cub` | Espaces valides dans la map |
| `valid_elements_any_order.cub` | Éléments dans le désordre (F avant NO, etc.) |
| `valid_elements_extra_spaces.cub` | Espaces multiples entre identifiant et valeur |
| `valid_large_map.cub` | Grande map avec couloirs |

## Maps INVALIDES (`invalid/`)

| Fichier | Ce qu'il doit déclencher |
|---|---|
| `broken_wall.cub` | Map non fermée par des murs |
| `empty_line_in_map.cub` | Ligne vide au milieu de la map |
| `invalid_char.cub` | Caractère interdit dans la map (`X`) |
| `no_player.cub` | Aucun spawn (N/S/E/W) |
| `too_much_player.cub` | Deux spawns dans la map |
| `twice_colors.cub` | Couleur `F` définie deux fois |
| `twice_textures.cub` | Texture `NO` définie deux fois |
| `wrong_ext.cubcubc` | Mauvaise extension de fichier |
| `missing_texture.cub` | Texture `WE` absente |
| `missing_color.cub` | Couleur `C` absente |
| `bad_rgb_range.cub` | Valeurs RGB hors [0,255] (300 et -5) |
| `bad_rgb_format.cub` | RGB avec seulement 2 valeurs au lieu de 3 |
| `texture_not_found.cub` | Chemin de texture inexistant sur le disque |
| `map_not_last.cub` | Contenu après la map (F redéfini après la map) |
| `no_map.cub` | Aucune map dans le fichier |
| `unknown_identifier.cub` | Identifiant inconnu `XY` |

## Cas d'usage à tester à la main (pas de fichier .cub)

| Commande | Ce qu'il doit déclencher |
|---|---|
| `./cub3D` | Aucun argument → message d'erreur propre |
| `./cub3D map1.cub map2.cub` | Trop d'arguments → comportement défini |
| `./cub3D fichier_inexistant.cub` | Fichier introuvable → erreur propre |

## Résultat attendu pour tous les fichiers invalides

```
Error
<message explicite de ton choix>
```
Aucun segfault, aucun zombie process, aucune fuite mémoire (valgrind).
