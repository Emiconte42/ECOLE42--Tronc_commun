/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:55:04 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 11:40:06 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MACROS_H
# define MACROS_H 

# include "philo.h"

# define EATING				1
# define SLEEPING			2
# define THINKING			3
# define FORK				4

# define STR_FORK			"has taken a fork"
# define STR_EAT			"is eating"
# define STR_SLEEP			"is sleeping"
# define STR_THINK			"is thinking"
# define STR_DEAD			"died"

/* --- Gestion d'erreur ---*/
# define ERR				"Error\n"
# define SUCCESS			0
# define STANDARD_ERROR		1

#endif