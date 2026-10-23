#ifndef FRAGTRAP_HPP
#define FRAGTRAP_HPP

#include <iostream>
#include <string>
#include "ClapTrap.hpp"

class FragTrap : public virtual ClapTrap // FragTrap herite de ClapTrap + virtual = pour que DiamondTrap n'ait qu'UN SEUL ClapTrap
{
	public :
		FragTrap();
		FragTrap(std::string const name);
		FragTrap (FragTrap const & cpy);
		FragTrap & operator=(FragTrap const &other);
		~FragTrap();

		void highFivesGuys(void);
};

#endif