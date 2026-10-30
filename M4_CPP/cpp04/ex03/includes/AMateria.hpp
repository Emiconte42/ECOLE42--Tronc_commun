#ifndef AMATERIA_HPP
#define AMATERIA_HPP

// #include "ICharacter.hpp"
#include <string>

/*
	Le moule commun des sorts :
	il garde le type ("ice", "cure") et oblige chaque sort a savoir se copier (clone).
	CLASSE ABTRAITE:
		- Avec au moins une fonction = 0
		- Elle peut avoir des attributs et du code
		- Peut heriter d'interface, de classe concrete ou abstraite
*/

class ICharacter;

class AMateria
{
	protected :
		std::string type;

	public :
		AMateria();
		AMateria(std::string const & type);
		AMateria(AMateria const &cpy);
		AMateria & operator=(AMateria const &other);
		virtual ~AMateria();

		std::string const & getType() const;

		virtual AMateria* clone() const = 0;
		virtual void use(ICharacter& target);
};

#endif