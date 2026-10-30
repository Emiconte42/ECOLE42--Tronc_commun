#ifndef MATERIASOURCE_HPP
#define MATERIASOURCE_HPP

#include <string>
#include "AMateria.hpp"
#include "IMateriaSource.hpp"

/*
	Le vrai livre de recettes :
	il garde jusqu'a 4 modeles et fabrique des copies neuves a partir d'un mot ("ice").
	CLASSE CONCRETE :
		- Pas de =0
		- Une classe qu'on peut creer avec new
		- Toutes les fonctions ont du codes
		- Peut heriter d'interface, de classe concrete ou abstraite
*/

class MateriaSource : public IMateriaSource
{
	public:
		MateriaSource();
		MateriaSource(std::string const & type);
		MateriaSource(MateriaSource const &cpy);
		MateriaSource & operator=(MateriaSource const &other);
		~MateriaSource();
		void learnMateria(AMateria*);
		AMateria* createMateria(std::string const & type);
};

#endif