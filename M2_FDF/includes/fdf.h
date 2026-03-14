/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 15:20:47 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/15 19:53:09 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

/*=======================
Librairies
=======================*/

# include "../libs/minilibx-linux/mlx.h"
# include "../libs/libft/libft.h"
# include "keys.h"
# include <fcntl.h>

/*=======================
Structure generale
=======================*/

# define Z_SCALE 5

/* Digital Differential Analyzer - Algotithm */
typedef struct s_dda
{
	float	x;
	float	y;
	float	x_inc;
	float	y_inc;
}	t_dda;

/* Point dans l'espace */
typedef struct s_point
{
	int			x;
	int			y;
}				t_point;

/* Image */
typedef struct s_img
{
	void		*img_ptr;
	char		*img_addr;
	int			bits_per_pixels;
	int			size_line;
	int			endian;
}				t_img;

/* Mlx / Window */
typedef struct s_window
{
	void		*mlx;
	void		*window;
	int			win_width;
	int			win_height;
}				t_window;

/* Map */
typedef struct s_map
{
	char		*filename;
	int			width;
	int			height;
	int			**data;
	int			z_min;
	int			z_max;
}				t_map;

/* Camera / View */
typedef struct s_view
{
	int			origin_x;
	int			origin_y;
	int			spacing;
}				t_view;

/* fdf */
typedef struct s_fdf
{
	t_dda		dda;
	t_img		img;
	t_window	window;
	t_map		map;
	t_view		view;
}				t_fdf;

/*       initialization.c       */
char	*get_filename(char *argv);
void	init_config(t_fdf *fdf);
void	init_view(t_fdf *fdf);
int		init_mlx(t_fdf *fdf);

/*       map_size.c / map_alloc.c / map_parse.c / map_load.c         */
int		map_load(t_fdf *fdf, const char *filename);
void	map_free(t_fdf *fdf);
int		map_size(const char *filename, int *width, int *height);
int		map_parse(const char *filename, t_fdf *fdf);

/*      graphics_run/graphics_map/graphics_point/graphics_line/graphics_pixel*/
int		get_color(int z, t_fdf *fdf);
int		graphics_run(t_fdf *fdf);
void	draw_map(t_fdf *fdf);
void	draw_line(t_fdf *fdf, t_point a, t_point b, int color);
void	draw_pixel(t_fdf *fdf, int x, int y, int color);
void	find_min_max(t_fdf *fdf);

/*       events.c/events_keys.c       */
int		key_hook(int keycode, void *param);
int		mouse_hook(int button, int x, int y, void *param);
void	zoom_in(t_fdf *fdf);
void	zoom_out(t_fdf *fdf);
void	zoom_at(t_fdf *fdf, int mouse_x, int mouse_y, int delta);
void	move_left(t_fdf *fdf);
void	move_right(t_fdf *fdf);
void	move_up(t_fdf *fdf);
void	move_down(t_fdf *fdf);
void	space_center(t_fdf *fdf);

/*       clean_and_exit.c       */
int		clean_and_exit(t_fdf *fdf);

#endif