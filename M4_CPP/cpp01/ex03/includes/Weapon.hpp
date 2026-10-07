#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <iostream>

class Weapon
{
	private:
		std::string	_type;

	public:
		Weapon(std::string const &weaponType);
		~Weapon();

		std::string const &getType() const; // retourne reference constante vers type
		void setType(const std::string &newType); // definit type a partir de la nouvelle valeur passee en parametre

};

#endif