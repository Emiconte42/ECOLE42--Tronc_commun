#ifndef ICHARACTER_HPP
#define ICHARACTER_HPP

#include "AMateria.hpp"
#include <string>

/*
	Le contrat d'un personnage : 
	il dit ce qu'un personnage sait faire (avoir un nom, equiper, desequiper, lancer un sort).
	INTERFACE :
	- Des fonctions virtuelles pures (=0)
	- Un destructeur virtuel
	- Peut heriter d'autres interfaces uniquement
*/

// class AMateria;

class ICharacter
{
	public:
		virtual ~ICharacter() {}
		virtual std::string const & getName() const = 0;
		virtual void equip(AMateria* m) = 0;
		virtual void unequip(int idx) = 0;
		virtual void use(int idx, ICharacter& target) = 0;
};

#endif