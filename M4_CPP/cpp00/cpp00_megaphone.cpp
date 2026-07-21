/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpp00_megaphone.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 11:24:48 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/20 13:25:36 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream> // pour utiliser std::count et std::endl

int main(int argc, char **argv)
{
	int	i; // pour se balader dans les arguments
	int	j; // pour se balader a l'interieur de chaque argument

	if (argc == 1) // si l'executable est tout seul
		std::cout << "* LOUD AND UNBEARABLE FEEDBACK NOISE *";
		// std::cout = accede au flux et ecris les messages en ecriture uniquement
		// l'operateur << est un operateur d'ecriture

	i = 1; // on saute l'executable ./megaphone
	while (argv[i]) // tant que les arguments ne sont pas null
	{
		j = 0;
		while (argv[i][j]) // tant qu'on est pas a la fin de la phrase
		{
			std::cout << (char)toupper(argv[i][j]); // on ecrit sur le stdout char par char
			j++; // on avance d'un char
		}
		i++; // on passe a un autre argument
	}
	std::cout << std::endl; // on affiche un saut de ligne que ce soit pour l'executable seul ou pour plusieurs arguments
	return(0);
}