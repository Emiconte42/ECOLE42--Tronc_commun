#ifndef SCAVTRAP_HPP
#define SCAVTRAP_HPP

#include <iostream>
#include <string>
#include "ClapTrap.hpp"

class ScavTrap : public virtual ClapTrap // ScavTrap herite de ClapTrap + virtual = pour que DiamondTrap n'ait qu'UN SEUL ClapTrap
{
	public :
		ScavTrap();
		ScavTrap(std::string const name);
		ScavTrap (ScavTrap const & cpy);
		ScavTrap & operator=(ScavTrap const &other);
		~ScavTrap();

		void attack(const std::string& target);
		void guardGate();
};

#endif