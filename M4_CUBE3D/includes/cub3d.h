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

/* ==== KEYCODES ==========================================*/
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
# define DOOR_ANIM_FRAMES 30

/* ==== TEXTURES ==========================================*/
# define WAEPON "asset/weapon.xpm"
# define WAEPON_SHOOT "asset/weapon_shoot.xpm"
# define DOOR_TEX_PATH "asset/doors/so_door"
# define DOOR_TEX_SUFFIX "_tex.xpm"

/* ==== ENUMS ===================================================== */
typedef enum e_error
{
	SUCCESS	=			0,
	ERROR =				1,
}	t_error;

typedef enum e_doors
{
	DOOR_INVALID =		0,
	DOOR_VERTICAL =		1,
	DOOR_HORIZONTAL =	2,
}	t_doors;

/* ==== GENERAL STRUCTS =========================================== */

/* ---- Window -------------------------------------------------- */
typedef struct s_window
{
	void			*mlx;
	void			*window;
}					t_window;

/* ---- Image --------------------------------------------------- */
typedef struct s_img
{
	void			*img_ptr;
	char			*img_addr;
	int				bits_per_pixels;
	int				size_line;
	int				endian;
	int				width;
	int				height;
}					t_img;

/* ---- Minimap ------------------------------------------------- */
typedef struct s_minimap
{
	int				origin_x;
	int				origin_y;
	int				size_map;
	int				tile_size;
	int				color;
	int				border_x;
	int				border_y;
	int				center_x;
	int				center_y;
}					t_minimap;

typedef struct s_rectangle
{
	int				x;
	int				y;
	int				color;
	int				size;
}					t_rectangle;

/* ---- Input --------------------------------------------------- */
typedef struct s_keys
{
	int				move_forward;
	int				move_backward;
	int				strafe_right;
	int				strafe_left;
	int				rotate_right;
	int				rotate_left;
	int				is_shooting;
}					t_keys;

/* ---- Doors --------------------------------------------------- */
typedef struct s_door
{
	int				x;
	int				y;
	int				is_open;
	int				frame;
	t_doors			orientation;
}					t_door;

/* ---- Textures (1_parsing) ------------------------------------ */
typedef struct s_tex
{
	char			*no_path;
	char			*so_path;
	char			*we_path;
	char			*ea_path;
	t_img			no;
	t_img			so;
	t_img			we;
	t_img			ea;
	t_img			waepon;
	t_img			waepon_shoot;
	t_img			door[DOOR_ANIM_FRAMES];
}					t_tex;

/* ---- Player ----------------------------------------------------- */
typedef struct s_player
{
	double			pos_x;
	double			pos_y;
	char			spawn_dir;
	double			dir_x;
	double			dir_y;
	double			plane_x;
	double			plane_y;
}					t_player;

/* ---- Raycasting --------------------------------------------------- */
typedef struct s_ray
{
	double			camera_x;
	double			dir_x;
	double			dir_y;
	int				map_x;
	int				map_y;
	double			side_dist_x;
	double			side_dist_y;
	double			delta_dist_x;
	double			delta_dist_y;
	int				step_x;
	int				step_y;
	int				hit;
	int				side;
	double			perp_wall_dist;
	int				line_height;
	int				draw_start;
	int				draw_end;
	double			wall_x;
	int				tex_x;
}					t_ray;

/* ---- Parsing / Map -------------------------------------------- */
typedef struct s_rgb
{
	int				r;
	int				g;
	int				b;
	unsigned int	hex_color;
}					t_rgb;

typedef struct s_map
{
	char			**grid;
	int				width;
	int				height;
}					t_map;

/* ---- Garbage collector (0_gc) -------------------------------------- */
typedef struct s_gc
{
	void			*content;
	struct s_gc		*next;
}					t_gc;

/* ---- Game (aggregate) ----------------------------------------------- */
typedef struct s_game
{
	t_list			*gc;
	t_tex			tex;
	t_rgb			floor;
	t_rgb			ceiling;
	int				defined_floor;
	int				defined_ceiling;
	t_map			map;
	t_player		player;
	t_window		window;
	t_minimap		minimap;
	t_img			img;
	t_ray			ray;
	t_keys			keys;
	int				mouse_enabled;
	int				mouse_ignore;
	t_door			*doors;
	int				doors_count;
}					t_game;

/* ==== FUNCTIONS 1_PARSING =============================== */

/* 1_extract_data */
int				ft_parse_cub_file(char *filename, t_game *game);

/* 2_ft_parse_params */
int				ft_check_missing_elements(t_game *game);
int				ft_parse_param(t_game *game, char *line);

/* 3_parse_utils */
int				ft_check_extension(char *filename);
int				ft_parse_color(t_game *game, t_rgb *color, char *line);
int				ft_parse_texture(t_game *game, char **target_path, char *line);
int				ft_check_map_not_last(char *line);

/* 4_map_validate */
int				ft_check_walls(t_game *game);
int				ft_validate_map(t_game *game);
int				ft_check_surrounding(t_game *game, int i, int j);

/* 5_map_validate_utils */
int				ft_check_cell(t_game *game, int i, int j);
void			ft_fix_map_spaces(t_game *game);

/* 6_map_validate_doors */
void			ft_register_doors(t_game *game);
int				ft_count_doors(t_game *game);
t_doors			ft_check_door_walls(t_game *game, int i, int j);

/* ==== FUNCTIONS 2_WINDOW ================================ */

/* 1_init_window */
int				ft_init_window(t_game *game);
int				ft_init_img(t_game *game);

/* 2_hooks */
int				ft_key_hook(int keycode, void *param);
int				ft_key_release(int keycode, void *param);
void			ft_game_loop(void *param);

/* 3_mouse.c */
int				ft_mouse_rotate(int x, int y, t_game *game);

/* 4_run */
int				ft_run_window(t_game *game);

/* 5_close */
void			ft_close_window(t_game *game);
int				ft_close_and_exit(t_game *game);

/* ==== FUNCTIONS 3_RAYCASTING ============================ */

/* 1_init */
void			ft_init_player_dir(t_game *game);
void			ft_init_ray(t_game *game, t_ray *ray);

/* 2_dda */
void			ft_dda(t_game *game, t_ray *ray);
void			ft_calculate_wall_dist(t_ray *ray);
void			ft_calculate_wall_height(t_ray *ray);

/* 3_texture */
unsigned int	ft_get_pixel_from_tex(t_img *tex, int x, int y);
void			ft_calculate_tex_x(t_game *game, t_ray *ray, t_img *tex);
t_img			*ft_get_tex(t_game *game, t_ray *ray);
void			ft_draw_tex_stripe(t_game *game, t_ray *ray, int x, t_img *tex);

/* 4_texture_load */
void			ft_load_all_tex(t_game *game);

/* 5_render */
void			ft_cast_rays(t_game *game);

/* ==== FUNCTIONS 4_GAME ================================== */

/* 1_movements */
int				ft_can_move(t_game *game, double new_x, double new_y);
void			ft_move_axis(t_game *game, double dx, double dy, double sign);
void			ft_rotate(t_game *game, double angle);

/* 2_doors */
t_door			*ft_get_door_at(t_game *game, int x, int y);
void			ft_toggle_door(t_game *game);
void			ft_update_doors(t_game *game);

/* ==== FUNCTIONS 5_RENDER ================================ */

/* 1_render */
void			ft_redraw(t_game *game);
int				ft_init_draw(t_game *game);
void			put_pixel(t_game *game, int x, int y, int color);

/* 2_render_utils */
void			ft_draw_weapon(t_game *game);

/* 3_minimap */
int				draw_map(t_game *game);
int				draw_player(t_game *game);

/* 4_minimap_utils */
void			draw_map_border(t_game *game);
void			draw_compass(t_game *game);
int				ft_get_door_color(t_game *game, int x, int y);

/* 5_doors */
void			ft_render_door(t_game *game, t_ray ray, int x, t_img *tex);

#endif
