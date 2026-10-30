#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "ICharacter.hpp"
#include <string>

/*
	Le vrai heros : 
	il a un nom et un sac de 4 Materias, et il fait ce que promet ICharacter.
	CLASSE CONCRETE :
		- Pas de =0
		- Une classe qu'on peut creer avec new
		- Toutes les fonctions ont du codes
		- Peut heriter d'interface, de classe concrete ou abstraite

*/

class Character : public ICharacter
{
	private:
		std::string	_nameCharacter;

	public :
		Character();
		Character(std::string const &nameCharacter);
		Character(Character const & copy);
		Character & operator=(Character const &other);

		~Character();
		std::string const & getName() const;
		void equip(AMateria* m);
		void unequip(int idx);
		void use(int idx, ICharacter& target);
};

#endif