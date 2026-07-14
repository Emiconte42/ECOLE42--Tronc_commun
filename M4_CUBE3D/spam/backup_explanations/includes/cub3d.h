/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 12:03:57 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/30 19:30:58 by hbelleuv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

/* ==== LIBRARIES UTILS =========================================== */
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <math.h>

/* ==== LIBRARIES PROJECT ========================================= */
# include "../libs/minilibx-linux/mlx.h"
# include "../libs/libft/libft.h"

/* ==== HEADERS =================================================== */
// # include "parsing.h"

/* ==== KEYCODES ==========================================*/

/* keynboard */
# define KEY_ESC 65307
# define KEY_SPACE 32

# define KEY_A 97
# define KEY_D 100
# define KEY_W 119
# define KEY_S 115
# define KEY_F 102
# define KEY_M 109

# define KEY_UP 65362
# define KEY_DOWN 65364
# define KEY_LEFT 65361
# define KEY_RIGHT 65363

/* ==== SIZE ==========================================*/

# define WINDOW_WIDTH 1920
# define WINDOW_HEIGHT 1080
# define MINIMAP_MAX_SIZE 200
# define PLAYER_POINT_SIZE 5
# define PLAYER_DIR_LEN 20
# define BEAM_STEP 0.2
# define MOVE_SPEED 0.04
# define PLAYER_RADIUS 0.08
# define ANGLE_SPEED 0.03
# define MOUSE_SENSITIVITY 0.001
# define WAEPON "asset/weapon.xpm"
# define WAEPON_SHOOT "asset/weapon_shoot.xpm"
# define DOOR_ANIM_FRAMES 30
# define DOOR_TEX_PATH "asset/doors/so_door"
# define DOOR_TEX_SUFFIX "_tex.xpm"

/* ==== ENUMS ===================================================== */
typedef enum e_error
{
	SUCCESS	= 0,
	ERROR = 1,
} t_error;

typedef enum e_doors
{
	DOOR_INVALID = 0,
	DOOR_VERTICAL = 1,
	DOOR_HORIZONTAL = 2,
} t_doors;

/* ==== GENERAL STRUCTS =========================================== */

/* 2_init_map - window */
typedef struct s_window {
	void			*mlx;
	void			*window;
}					t_window;

/* Minimap */
typedef struct s_minimap {
	int				origin_x;
	int				origin_y;
	int				size_map;
	int				tile_size;
	int				color;
}					t_minimap;

typedef struct s_rectangle // contour des cases de la minimap
{
	int				x;
	int				y;
	int				color;
	int				size;
}					t_rectangle;

/* Image */
typedef struct s_img {
	void			*img_ptr;
	char			*img_addr;
	int				bits_per_pixels;
	int				size_line;
	int				endian;
	int				width; // largeur image (texture)
	int				height; // hauteur image (texture)
}				t_img;

typedef struct s_keys{
	int				move_forward; // W
	int				move_backward; // S
	int				strafe_right; // A
	int				strafe_left; // D
	int				rotate_right; // fleche
	int				rotate_left; // fleche
	int				is_shooting; // F
}				t_keys;

typedef struct s_door{
	int				x;
	int				y;
	int				is_open;
	int				frame;
	t_doors			orientation;
}				t_door;

/* 1_parsing */
typedef struct s_rgb {
	int					r;
	int					g;
	int					b;
	unsigned int		hex_color;
}						t_rgb;

typedef struct s_tex {
	char				*no_path;
	char				*so_path;
	char				*we_path;
	char				*ea_path;
	t_img				no;
	t_img				so;
	t_img				we;
	t_img				ea;
	t_img				waepon;
	t_img				waepon_shoot;
	t_img				door[DOOR_ANIM_FRAMES];
}						t_tex;

typedef struct s_map {
	char				**grid; // map dans un double tab
	int					width; // largeur map
	int					height; // hauteur map
}						t_map;

typedef struct s_player {
	double				pos_x; // pos x du joueur
	double				pos_y; // pos y du joueur
	char				spawn_dir; // orientation de depart du joueur
	double				dir_x; // vecteur de direction : indique vers ou le joueur regarde
	double				dir_y;
	double				plane_x; // vecteur du plan de la camera (ecran)
	double				plane_y; // toujours perpendiculaire a la direction : sa longueur determine le FOV
}						t_player;

/* 0_gc */
typedef struct s_gc {
	void				*content; // le pointeur alloué
	struct s_gc			*next; // le noeud suivant
}						t_gc;

typedef struct s_ray {
	double				camera_x;
	double				dir_x;
	double				dir_y;
	int					map_x; // la case (int) de la map ou se trouve le rayon
	int					map_y;
	double				side_dist_x; // distance jusqu'au prochain bord
	double				side_dist_y;
	double				delta_dist_x; // distance a parcourir pour traverser une case
	double				delta_dist_y;
	int					step_x; // direction du saut (+1 ou -1)
	int					step_y;
	int					hit; // booleen (a t on touche un mur ?) (0 ou 1)
	int					side; // quel cote du mur a ete touche (0 = X et 1 = Y)
	double				perp_wall_dist; // "vrai distance" a la perpendiculaire du mur
	int					line_height; // hauteur de la ligne a dessiner
	int					draw_start; // Pixel de depart
	int					draw_end; // pixel de fin
	double				wall_x; // coordonnee exacte ou le mur a ete touche
	int					tex_x; // coordonnee X (colonne) sur la texture
} 						t_ray;

/* general */
typedef struct s_game {
	t_list				*gc; // garbage collector
	t_tex				tex; // chemins des textures
	t_rgb				floor; // couleur sol
	t_rgb				ceiling; // couleur plafond
	int					defined_floor; // boolen de verification doublon
	int					defined_ceiling;
	t_map				map; // data de la carte
	t_player			player; // data du joueur
	t_window			window; // afficher la fenetre du jeu
	t_minimap			minimap; // gestion de la vue dans la fenetre
	t_img				img;
	t_ray				ray;
	t_keys				keys;
	int					mouse_enabled;
	int					mouse_ignore;
	t_door				*doors;
	int					doors_count;
}						t_game;

/* ==== FUNCTIONS DEBUG =================================== */
void	print_gc(t_list *gc);
void	print_gc_str(t_list *gc);
void	print_gc_count(t_list *gc);

/* ==== FUNCTIONS PARSING ============== */
int		ft_parse_cub_file(char *filename, t_game *game);
int		ft_parse_texture(t_game *game, char **target_path, char *line);
int		ft_parse_color(t_game *game, t_rgb *color, char *line);
int		ft_validate_map(t_game *game);
int		ft_check_walls(t_game *game);
int		parse_param(t_game *game, char *line);
int		ft_check_missing_elements(t_game *game);
int		ft_check_map_not_last(char *line);
int		ft_check_extension(char *filename);
t_doors	ft_check_door_walls(t_game *game, int i, int j);
int		ft_count_doors(t_game *game);
void	ft_register_doors(t_game *game);

/* ==== FUNCTIONS WINDOW ============== */

/* 1_init_window */
int		ft_init_window(t_game *game);
int		ft_init_img(t_game *game);
void	ft_close_window(t_game *game);
int		ft_close_and_exit(t_game *game);

/* 2_hooks */
int		ft_key_hook(int keycode, void *param);
int		ft_key_release_hook(int keycode, void *param);
void	ft_game_loop(void *param);

/* 3_mouse.c */
int		ft_mouse_rotate(int x, int y, t_game *game);

/* 4_run */
int		ft_run_window(t_game *game);

// raycasting
void	ft_cast_rays(t_game *game);
void	ft_init_player_dir(t_game *game);
void	ft_load_all_tex(t_game *game);
unsigned int	ft_get_pixel_from_tex(t_img *tex, int x, int y);
void	ft_load_door_tex(t_game *game);
void	ft_calculate_wall_dist(t_ray *ray);
void	ft_calculate_wall_height(t_ray *ray);
t_img	*ft_get_tex(t_game *game, t_ray *ray);
void	ft_calculate_tex_x(t_game *game, t_ray *ray, t_img *tex);
void	ft_draw_tex_stripe(t_game *game, t_ray *ray, int x, t_img *tex);

/* ==== FUNCTIONS GAME ============== */

/* 1_movements */
int		ft_can_move(t_game *game, double new_x, double new_y);
void	ft_move_axis(t_game *game, double dx, double dy, double sign);
void	ft_rotate(t_game *game, double angle);

/* 2_doors */
t_door	*ft_get_door_at(t_game *game, int x, int y);
void	ft_toggle_door(t_game *game);
void	ft_update_doors(t_game *game);

/* ==== FUNCTIONS RENDER ============== */

/* 1_render */
void	ft_redraw(t_game *game);
int		ft_init_draw(t_game *game);
void	put_pixel(t_game *game, int x, int y, int color);
void	ft_draw_weapon(t_game *game);

/* 3_minimap */
int		draw_map(t_game *game);
int		draw_player(t_game *game);
int		draw_square_border(t_game *game, t_rectangle rectangle);
void	draw_compass(t_game *game);

/* 4_doors */
int		ft_is_door_transparent(unsigned int color);
void	ft_dda_bg(t_game *game, t_ray *ray);
void	ft_draw_door_stripe(t_game *game, t_ray *ray, int x, t_img *tex);
void	ft_render_door(t_game *game, t_ray ray, int x, t_img *tex);

#endif
