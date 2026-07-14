/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:29:19 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/07/07 11:27:08 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

// Extraire l'adress des pixels des fichiers .xpm
void	ft_load_single_tex(t_game *game, t_img *tex, char *path)
{
	// charge le fichier xpm dans un ptr d'img
	tex->img_ptr = mlx_xpm_file_to_image(game->window.mlx, path, &tex->width, &tex->height);
	if (tex->img_ptr == NULL)
	{
		printf("Error : failed to load texture %s\n", path);
		// ft_gc_free(&game->gc);
		ft_close_window(game);
		exit(1);
	}
	// recupere l'adress mémoire (buffer) contenant les pixels
	tex->img_addr = mlx_get_data_addr(tex->img_ptr, &tex->bits_per_pixels,
		&tex->size_line, &tex->endian);
}

void	ft_load_all_tex(t_game *game)
{
	ft_load_single_tex(game, &game->tex.no, game->tex.no_path);
	ft_load_single_tex(game, &game->tex.so, game->tex.so_path);
	ft_load_single_tex(game, &game->tex.we, game->tex.we_path);
	ft_load_single_tex(game, &game->tex.ea, game->tex.ea_path);
	ft_load_single_tex(game, &game->tex.waepon, WAEPON);
	ft_load_single_tex(game, &game->tex.waepon_shoot, WAEPON_SHOOT);
	ft_load_door_tex(game);
}

/*
La fonction pour lire un pixel :
Les textures sont chargées en mémoire, le moteur de rendu 3D a 
besoin d'une petite fonction pour aller "piocher" la bonne couleur
à la coordonnée (x, y) de l'image.
Puisque les couleurs sont stockées sous forme d'entiers de 32 bits,
on peut utiliser exactement la même logique mathématique que celle
utilisée pour peindre sur l'écran
*/
unsigned int	ft_get_pixel_from_tex(t_img *tex, int x, int y)
{
	int	*buffer;

	// on cast l'adresse de l'img en tableau de int (1 int = 1 pixel)
	buffer = (int *)tex->img_addr;
	// on renvoie la couleur a la coordonnee (x, y)
	return (buffer[y * (tex->size_line / 4) + x]);
}

/*
Calculer wall_x et tex_x (L'axe horizontal de la texture) :
Juste après avoir calculé calculate_wall_height, 
Il savoir exactement où le rayon a frappé le mur 
(par exemple, en plein milieu ou sur le bord gauche)
pour choisir la bonne colonne de la texture
*/

void	ft_calculate_tex_x(t_game *game, t_ray *ray, t_img *tex)
{
	// calcul la coordonnee exacte de l'impact (wall_x)
	if (ray->side == 0)
		ray->wall_x = game->player.pos_y + ray->perp_wall_dist * ray->dir_y;
	else
		ray->wall_x = game->player.pos_x + ray->perp_wall_dist * ray->dir_x;
	// regarde uniquement la partie décimale
	ray->wall_x -= floor(ray->wall_x);

	// calcul de la coordonnee tex_x sur la texture
	ray->tex_x = (int)(ray->wall_x * (double)tex->width);

	// inversement des textures pour eviter l'effet miroir selon la direction du ray
	if (ray->side == 0 && ray->dir_x < 0)
		ray->tex_x = tex->width - ray->tex_x - 1;
	if (ray->side == 1 && ray->dir_y > 0)
		ray->tex_x = tex->width - ray->tex_x - 1;
}

void	ft_draw_tex_stripe(t_game *game, t_ray *ray, int x, t_img *tex)
{
	int				y;
	double			step;
	double			tex_pos;
	int				tex_y;
	unsigned int	color;

	// de combien on avance dans la tex pour chaque pixel de l'ecran
	step = 1.0 * tex->height / ray->line_height;
	// coordonnee de Y de depart sur la texture
	tex_pos = (ray->draw_start - (double)WINDOW_HEIGHT / 2 + (double)ray->line_height / 2) * step;
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		// recup la coo]coordonnee Y exacte sur la texture
		tex_y = (int)tex_pos;
		if (tex_y >= tex->height)
			tex_y = tex->height - 1;
		else if (tex_y < 0)
			tex_y = 0;
		tex_pos += step;
		color = ft_get_pixel_from_tex(tex, ray->tex_x, tex_y);
		// assombrir les murs pour donner de la profondeur
		if (ray->side == 1)
			color = (color >> 1) & 8355711;
		put_pixel(game, x, y, color);
		y++;
	}
}

/*
- Si ray->side == 0, le rayon a touché un mur sur l'axe X
(donc on percute la face Est ou Ouest d'un bloc)
- Si le rayon allait vers la droite (dir_x > 0), il percute la face Ouest du mur
- S'il allait vers la gauche (dir_x < 0), il percute la face Est
- Si ray->side == 1, le rayon a touché un mur sur l'axe Y (donc on percute la face Nord ou Sud d'un bloc)
- Si le rayon allait vers le bas de la grille (dir_y > 0), il percute la face Nord du mur
- S'il allait vers le haut de la grille (dir_y < 0), il percute la face Sud
*/
t_img	*ft_get_tex(t_game *game, t_ray *ray)
{
	t_door	*door;

	if (game->map.grid[ray->map_y][ray->map_x] == 'D')
	{
		door = ft_get_door_at(game, ray->map_x, ray->map_y);
		if (door)
			return(&game->tex.door[door->frame]);
	}
	// si on touche un mur vertical sur la carte (Est ou OUest)
	if (ray->side == 0)
	{
		if (ray->dir_x > 0)
			return (&game->tex.we); // regarde vers est donc ouest
		else
			return (&game->tex.ea); // regarde vers ouest donc est
	}
	else
	{
		if (ray->dir_y > 0)
			return (&game->tex.so); // regarde vers sud donc nord
		else
			return (&game->tex.no); // regarde vers nord donc sud
	}

}

/*
FOV :

L'utilisation des vecteurs plutôt que des angles euclidiens (avec des sinus et cosinus)
repose principalement sur des raisons de simplicité mathématique et de performances accrues.
Voici pourquoi la méthode vectorielle est bien supérieure pour construire votre raycaster :

- Des calculs beaucoup plus rapides : Si vous utilisiez des angles, vous devriez appeler
les fonctions trigonométriques sin() et cos() pour calculer la direction de
chaque rayon lancé sur chaque colonne verticale de votre écran.
Avec les vecteurs, la direction d'un rayon se calcule avec de simples additions
et multiplications en combinant le vecteur de direction et
une fraction du vecteur du plan de la caméra.

- Une intégration parfaite avec l'algorithme DDA (Digital Differential Analysis) :
L'algorithme DDA que vous allez coder a besoin de sauter de case en case
sur les axes X et Y de votre grille. Les vecteurs fournissent naturellement
ces composantes directionnelles X et Y prêtes à l'emploi,
rendant le calcul des distances de saut beaucoup plus direct.

- Un évitement naturel de l'effet "Fisheye" :
Pour que les murs ne paraissent pas arrondis comme dans un aquarium
(le fameux effet "fisheye"), il faut calculer 
la distance perpendiculaire entre le mur touché et le plan de la caméra.
L'utilisation d'un vecteur pour définir la surface physique de l'écran (le plan de la caméra)
rend le calcul de cette distance perpendiculaire beaucoup plus simple
et direct que de devoir ajuster des distances réelles avec des angles.

Cela dit, les angles trigonométriques ne sont pas totalement bannis de votre programme !
Vous utiliserez bien des sinus et des cosinus, mais uniquement lorsque
le joueur tournera la caméra avec les flèches du clavier.
À ce moment précis, vous appliquerez une matrice de rotation mathématique
(utilisant cos et sin) pour faire pivoter vos vecteurs de direction et de plan.
Le reste du temps, lors de l'affichage pur de l'image, votre boucle de rendu
fonctionnera uniquement avec des additions vectorielles très rapides !

Le champ de vision (FOV) est déterminé par le ratio entre
la longueur du vecteur de direction et celle du vecteur du plan.
Avec ces valeurs, le calcul mathématique (2 * atan(0.66 / 1.0))
donne un FOV d'environ 66 degrés, ce qui est la valeur parfaite
pour un jeu de tir à la première personne (FPS)
*/


/*
Le but de cette fonction est de configurer step et side_dist :
- Si le rayon part vers l'Ouest (X négatif), il fera des pas de -1.
La distance vers le premier mur correspond à la fraction de la case actuelle multipliée par delta_dist.
- S'il part vers l'Est (X positif), il fera des pas de +1.
*/
void	ft_init_ray(t_game *game, t_ray *ray)
{
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (game->player.pos_x - ray->map_x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - game->player.pos_x) * ray->delta_dist_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (game->player.pos_y - ray->map_y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - game->player.pos_y) * ray->delta_dist_y;
	}
}

/*
Le vecteur de direction (dir_x, dir_y) indique vers où le joueur regarde,
et sa longueur doit toujours être exactement de 1.
Le plan de la caméra (plane_x, plane_y) doit toujours être
perpendiculaire à la direction :
En lui donnant une longueur de 0.66, on obtient un champ de vision (FOV) parfait
d'environ 66 degrés, ce qui est le standard 
pour un jeu de tir à la première personne
*/
void	ft_init_player_dir(t_game *game)
{
	if (game->player.spawn_dir == 'N')
	{
		game->player.dir_x = 0.0;
		game->player.dir_y = -1.0;
		game->player.plane_x = 0.66;
		game->player.plane_y = 0.0;
	}
	if (game->player.spawn_dir == 'S')
	{
		game->player.dir_x = 0.0;
		game->player.dir_y = 1.0;
		game->player.plane_x = -0.66;
		game->player.plane_y = 0.0;
	}
	if (game->player.spawn_dir == 'W')
	{
		game->player.dir_x = 1.0;
		game->player.dir_y = 0.0;
		game->player.plane_x = 0.0;
		game->player.plane_y = 0.66;
	}
	if (game->player.spawn_dir == 'E')
	{
		game->player.dir_x = -1.0;
		game->player.dir_y = 0.0;
		game->player.plane_x = 0.0;
		game->player.plane_y = -0.66;
	}
}

/*
l'algorithme DDA (Digital Differential Analysis) :
Le rôle de cette boucle est simple : faire avancer notre rayon virtuellement sur la grille de la carte,
case par case, jusqu'à ce qu'il percute un mur.
Il ne vérifiera pas tous les millièmes de pixels, 
mais fera des "bonds" d'une bordure de case à l'autre pour être extrêmement rapide
*/

void	ft_dda(t_game *game, t_ray *ray)
{
	t_door	*door;

	ray->hit = 0;
	// on verifie qu'on a pas touche de mur
	while (ray->hit == 0)
	{
		// on saute vers la bordure de la case la plus proche (X ou Y)
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x;
			ray->map_x += ray->step_x;
			ray->side = 0; // on a percute un mur vertical (N ou S)
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y;
			ray->map_y += ray->step_y;
			ray->side = 1; // on a percute un mur horizontal (W ou E)
		}
		// verifie si la case actuelle est un mur ou pas
		if (game->map.grid[ray->map_y][ray->map_x] == '1')
			ray->hit = 1;
		else if (game->map.grid[ray->map_y][ray->map_x] == 'D')
		{
			door = ft_get_door_at(game, ray->map_x, ray->map_y);
			if (door && door->frame < DOOR_ANIM_FRAMES - 1)
				ray->hit = 1;
		}
	}
}
/*
Le calcul de la vraie distance (evite le Fisheye)
Si on calculait la distance directe entre le joueur et le point d'impact du mur,
on se retrouverait avec un rendu en forme d'aquarium (le fameux effet "fisheye")
Pour corriger ça, on ne veut pas la distance au joueur,
mais la distance perpendiculaire au plan de la caméra.

Grâce aux mathématiques du DDA :
- il suffit de prendre la dernière distance calculée (side_dist) et 
de lui soustraire la valeur d'un saut de case (delta_dist)
*/

void	ft_calculate_wall_dist(t_ray *ray)
{
	if (ray->side == 0)
		ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
}

/*
Calculer la taille du mur à l'écran :
Plus un mur est loin, plus il doit apparaître petit.
La formule est : 
la hauteur de la ligne (line_height) est simplement l'inverse 
de notre fameuse perp_wall_dist multipliée par la hauteur de la fenêtre.

Ensuite, on calcule le point de départ (draw_start) 
et le point de fin (draw_end) pour que le mur soit 
parfaitement centré verticalement au milieu de l'écran.
*/
void	ft_calculate_wall_height(t_ray *ray)
{
	// evite la division par 0 si on est dans le mur
	if (ray->perp_wall_dist == 0)
		ray->perp_wall_dist = 0.1;
	ray->line_height = (int)(WINDOW_HEIGHT / ray->perp_wall_dist);
	// calcul pixel de depart (centré)
	ray->draw_start = -ray->line_height / 2 + WINDOW_HEIGHT / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0; // pour ne pas dessiner en dehors de l'ecran
	// calcul pixel de fin (centré)
	ray->draw_end = ray->line_height / 2 + WINDOW_HEIGHT / 2;
	if (ray->draw_end >= WINDOW_HEIGHT)
		ray->draw_end = WINDOW_HEIGHT - 1; // permet de brider la taille de la fenêtre
}

/*
Peindre la colonne de pixels :
utilisation les couleurs hexadécimales (hex_color) ajoutées lors du parsing
on écrit directement dans la mémoire de l'image de la miniLibX via img_addr.
(Astuce : Pour donner une illusion de profondeur en 3D avant même d'avoir des textures,
on va peindre les murs touchés sur l'axe Y dans un gris plus sombre que ceux touchés sur l'axe X.)
*/
void	ft_draw_vertical_stripe(t_game *game,t_ray *ray, int x)
{
	int				y;
	unsigned int	color;
	int				*buffer;

	// on cast l'adress de l'image en tableau de int (32 bits = 1 pixel)
	buffer = (int *)game->img.img_addr;
	y = 0;
	while (y < WINDOW_HEIGHT)
	{
		// si on est en haut (floor)
		if (y < ray->draw_start)
			color = game->ceiling.hex_color;
		// si on est en bas (ceiling)
		else if (y > ray->draw_end)
			color = game->floor.hex_color;
		// sinon on est sur le mur
		else
		{
			if (ray->side == 0)
				color = 0x555555; // Mur de face (Y)
			else
				color = 0xAAAAAA; // mur de profil (X)
		}
		// on dessine le pixel a la coordonnee (x,y) dans l'image
		buffer[y * (game->img.size_line / 4) + x] = color;
		y++;
	}
}

void	ft_cast_rays(t_game *game)
{
	int		x;
	t_ray	ray;
	t_img	*tex;

	x = 0;
	while (x < WINDOW_WIDTH)
	{
		// calculer la dir du ray pour cette colonne x
		ray.camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
		ray.dir_x = game->player.dir_x + game->player.plane_x * ray.camera_x;
		ray.dir_y = game->player.dir_y + game->player.plane_y * ray.camera_x;

		// initialisation de la position sur la map
		ray.map_x = (int)game->player.pos_x;
		ray.map_y = (int)game->player.pos_y;

		// calcul du delta_dist (longueur du ray pour traverser une case)
		// on utilise fabs() pour s'assurer que la distance est toujours positive
		if (ray.dir_x == 0)
			ray.delta_dist_x = 1e30;
		else
			ray.delta_dist_x = fabs(1.0 / ray.dir_x);
		if (ray.dir_y == 0)
			ray.delta_dist_y = 1e30;
		else
			ray.delta_dist_y = fabs(1.0 / ray.dir_y);

		// initialisation de step (les pas) + side_dist (distances initiales)
		ft_init_ray(game, &ray);

		// lance le ray jusqu'au mur
		ft_dda(game, &ray);

		// calcul la dist parfaite
		ft_calculate_wall_dist(&ray);

		// calculer la taille du mur a l'ecran
		ft_calculate_wall_height(&ray);

		// selectionne la bonne img adapte au bon mur (no, so, ea, we)
		tex = ft_get_tex(game, &ray);

		//calcul quelle colonne de pixel de l'img doit on lire
		ft_calculate_tex_x(game, &ray, tex);

		// dessiner la colonne de pixel a l'ecran avec texture
		// ft_draw_vertical_stripe(game, &ray, x);
		// ft_draw_tex_stripe(game, &ray, x, tex);
		if (game->map.grid[ray.map_y][ray.map_x] == 'D')
			ft_render_door(game, ray, x, tex);
		else
			ft_draw_tex_stripe(game, &ray, x, tex);
		x++;
	}
}

void	ft_load_door_tex(t_game *game)
{
	int		i;
	char	*num;
	char	*path;

	i = 0;
	while (i < DOOR_ANIM_FRAMES)
	{
		num = ft_gc_itoa(i, &game->gc);
		path = ft_gc_strjoin(DOOR_TEX_PATH, num, &game->gc);
		path = ft_gc_strjoin(path, DOOR_TEX_SUFFIX, &game->gc);
		ft_load_single_tex(game, &game->tex.door[i], path);
		i++;
	}
}
