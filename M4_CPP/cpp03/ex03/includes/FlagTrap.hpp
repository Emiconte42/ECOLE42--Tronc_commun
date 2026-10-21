#ifndef FLAGTRAP_HPP
#define FLAGTRAP_HPP

#include <iostream>
#include "ClapTrap.hpp"

class FlagTrap : public ClapTrap
{
	public :
		FlagTrap(); // constructeur par defaut
		FlagTrap(std::string const name); // constructeur par defaut avec name
		FlagTrap (FlagTrap const & cpy); // constructeur par copie
		~FlagTrap(); // destructeur
		FlagTrap & operator=(FlagTrap const &other);

		void highFivesGuys(void);
};

#endif