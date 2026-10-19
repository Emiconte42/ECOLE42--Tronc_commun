#ifndef CLAPTRAP_HPP
#define CLAPTRAP_HPP

#include <iostream>

class ClapTrap
{
	private :
		std::string	_name;
		unsigned int	_hitPoints;
		unsigned int	_energyPoint;
		unsigned int	_attackDamage;

	public :
		ClapTrap(); // constructeur par defaut
		ClapTrap(std::string const name); // constructeur par defaut avec name
		ClapTrap (ClapTrap const & cpy); // constructeur par copie
		~ClapTrap(); // destructeur
	
		// ClapTrap & operator=(ClapTrap const &other); // operateur d'assignation
		void attack(const std::string& target);
		void takeDamage(unsigned int amount);
		void beRepaired(unsigned int amount);
};

#endif