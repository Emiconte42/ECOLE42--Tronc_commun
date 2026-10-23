#ifndef SCAVTRAP_HPP
#define SCAVTRAP_HPP

#include <iostream>
#include <string>
#include "ClapTrap.hpp"

class ScavTrap : public ClapTrap
{
	private :
		bool	_guardGate;

	public :
		ScavTrap(); // constructeur par defaut
		ScavTrap(std::string const name); // constructeur par defaut avec name
		ScavTrap (ScavTrap const & cpy); // constructeur par copie
		~ScavTrap(); // destructeur
		ScavTrap & operator=(ScavTrap const &other);

		bool getGuardGate() const;
		void attack(const std::string& target);
		void guardGate();

		void print_stats() const;
};

#endif