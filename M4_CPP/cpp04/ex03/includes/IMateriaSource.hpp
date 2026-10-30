#ifndef IMATERIASOURCE_HPP
#define IMATERIASOURCE_HPP

#include "AMateria.hpp"
#include <string>

/*
	Le contrat d'un livre de recettes :
	il dit qu'on peut apprendre un modele et fabriquer une Materia.
	INTERFACE :
		- Des fonctions virtuelles pures (=0)
		- Un destructeur virtuel
		- Peut heriter d'autres interfaces uniquement
*/

class IMateriaSource
{
	public:
	virtual ~IMateriaSource() {}
	virtual void learnMateria(AMateria*) = 0;
	virtual AMateria* createMateria(std::string const & type) = 0;
};

#endif