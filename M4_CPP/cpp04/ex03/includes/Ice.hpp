#ifndef ICE_HPP
#define ICE_HPP

#include "AMateria.hpp"
// #include <string>
#include <iostream>

/*
	Un sort d'attaque : il affiche * shoots an ice bolt at bob *.
	CLASSE CONCRETE :
		- Pas de =0
		- Une classe qu'on peut creer avec new
		- Toutes les fonctions ont du codes
		- Peut heriter d'interface, de classe concrete ou abstraite
*/

class Ice : public AMateria
{
	public :
		Ice();
		Ice(std::string const & type);
		Ice(Ice const &cpy);
		Ice & operator=(Ice const &other);
		~Ice();

		std::string const & getType() const;

		AMateria* clone() const;
		void use(ICharacter& target);
};

#endif