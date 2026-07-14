/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_A_SUPPR.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 20:53:43 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/25 17:46:38 by hbelleuv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"
#include <stdio.h>

/* Affiche tous les pointeurs enregistres dans le GC avec leur adresse 
		- sous le format : === Contenu du GC ===
  [0] ptr = 0x2716910
  [1] ptr = 0x27168d0
  [2] ptr = 0x2716890
  [3] ptr = 0x2716840
  [4] ptr = 0x2716800
  [5] ptr = 0x27167c0
  [6] ptr = 0x2716780
  [7] ptr = 0x2716740*/
  void	print_gc(t_list *gc)
{
	t_list	*cur;
	int		i;

	cur = gc;
	i = 0;
	printf("=== Contenu du GC ===\n");
	while (cur)
	{
		printf("  [%d] ptr = %p\n", i, cur->content);
		cur = cur->next;
		i++;
	}
	if (i == 0)
		printf("  (vide)\n");
	printf("===================\n\n");
}

/* Affiche tous les pointeurs du GC en affichant leur valeur comme string.
   N'utiliser que si on est certain que tous les pointeurs sont des char *.
   		- sous le format : === Contenu du GC (strings) ===
							[0] ptr = 0x13df6910 | valeur = "trois"
							[1] ptr = 0x13df68d0 | valeur = "deux"
							[2] ptr = 0x13df6890 | valeur = "un" */
void	print_gc_str(t_list *gc)
{
	t_list	*cur;
	int		i;

	cur = gc;
	i = 0;
	printf("=== Contenu du GC (strings) ===\n");
	while (cur)
	{
		printf("  [%d] ptr = %p | valeur = \"%s\"\n",
			i, cur->content, (char *)cur->content);
		cur = cur->next;
		i++;
	}
	if (i == 0)
		printf("  (vide)\n");
	printf("=============================\n\n");
}

/* Affiche le nombre d'elements actuellement dans le GC
		- sous le format = nombre de GC : 8 element(s) */
void	print_gc_count(t_list *gc)
{
	t_list	*cur;
	int		count;

	cur = gc;
	count = 0;
	while (cur)
	{
		count++;
		cur = cur->next;
	}
	printf("nombre de GC : %d element(s)\n\n", count);
}
