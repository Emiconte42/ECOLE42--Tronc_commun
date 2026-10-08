#ifndef HUMANB_HPP
#define HUMANB_HPP

#include <iostream>
#include "Weapon.hpp"

class HumanB
{
	private:
		std::string _humanBName;
		Weapon*		_weapon;

	public:
		HumanB(std::string const & humanBName);
		~HumanB	();

		void setWeapon(Weapon& weapon);
		void attack(void);
};	
#endif