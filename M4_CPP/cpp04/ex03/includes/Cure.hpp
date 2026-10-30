#ifndef CURE_HPP
#define CURE_HPP

#include "AMateria.hpp"
// #include <string>
#include <iostream>

/*
	Un sort de soin : il affiche * heals bob's wounds *.
	CLASSE CONCRETE :
		- Pas de =0
		- Une classe qu'on peut creer avec new
		- Toutes les fonctions ont du codes
		- Peut heriter d'interface, de classe concrete ou abstraite
*/

class Cure : public AMateria
{
		public :
		Cure();
		Cure(std::string const & type);
		Cure(Cure const &cpy);
		Cure & operator=(Cure const &other);
		~Cure();

		std::string const & getType() const;

		AMateria* clone() const = 0;
		void use(ICharacter& target);
};

#endif