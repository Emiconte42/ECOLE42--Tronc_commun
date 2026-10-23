#ifndef FRAGTRAP_HPP
#define FRAGTRAP_HPP

#include <iostream>
#include <string>
#include "ClapTrap.hpp"

class FragTrap : public ClapTrap
{
	public :
		FragTrap(); // constructeur par defaut
		FragTrap(std::string const name); // constructeur par defaut avec name
		FragTrap (FragTrap const & cpy); // constructeur par copie
		~FragTrap(); // destructeur
		FragTrap & operator=(FragTrap const &other);

		void highFivesGuys(void);
};

#endif