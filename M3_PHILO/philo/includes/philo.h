/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 13:28:07 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 12:49:09 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>
# include <limits.h>
# include "macros.h"

typedef struct s_table	t_table;

typedef struct s_philo
{
	int					id;
	pthread_t			thread;
	pthread_mutex_t		*fork_left;
	pthread_mutex_t		*fork_right;
	long				last_meal;
	t_table				*table;
	int					meal_count;
	int					status;
}						t_philo;

typedef struct s_table
{
	int					number_of_philosophers;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	int					must_eat_count;
	pthread_mutex_t		*forks;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t		death_mutex;
	pthread_mutex_t		last_meal_mutex;
	pthread_t			monitor;
	int					is_dead;
	int					simulation_finished;
	long				start_time;
	t_philo				*philos;
	int					full;
	pthread_mutex_t		full_mutex;
}						t_table;

/*---utils---*/
long				ft_atol(const char *nptr);
long				ft_isdigit(int c);
size_t				ft_strlen(const char *str);

/*---parsing---*/
int					ft_parsing(t_table *table, int argc, char **argv);

/*---check_parsing*/
int					ft_check_is_digit(char *str);
int					ft_check_zero(long value);
int					ft_check_overflow(long value);

/*---initialisation---*/
int					ft_init(t_table *table);

/*---error---*/
void				ft_cleanup(t_table *table);
void				ft_cleanup_init(t_table *table);

/* ---start--- */
int					ft_start(t_table *table);
void				*ft_monitor(void *arg);
/* ---state--- */
int					ft_get_is_dead(t_table *table);
void				ft_set_is_dead(t_table *table);

/*---time---*/
long				get_current_time(void);
void				ft_wait(t_philo *philo);

/*---printf---*/
void				ft_printf(t_table *table, t_philo *philo, char *message);

/*---routine---*/
void				*ft_routine(void *arg);

/*---1philo---*/
int					handle_one_philo(t_table *table);

/*---threads---*/
int					create_threads(t_table *table);
void				join_threads(t_table *table);

/*--forks---*/
int					ft_take_forks(t_philo *philo);
int					ft_unlock_one_fork(pthread_mutex_t *fork);
int					ft_unlock_both_forks(t_philo *philo);

/*---meals---*/
int					ft_check_satiated(t_philo *philo);
long				ft_get_last_meal(t_philo *philo);
void				ft_set_last_meal(t_philo *philo);
long				ft_get_meal_count(t_philo *philo);

int					ft_is_dead_only(t_table *table);

#endif