#ifndef HUMANA_HPP
#define HUMANA_HPP

#include <iostream>
#include "Weapon.hpp"

class HumanA
{
	private:
		std::string _humanAName;
		Weapon&		_weapon;

	public:
		HumanA(std::string const & _humanAName, Weapon& _weapon);
		~HumanA	();
};	

#endif