#ifndef DIAMONDTRAP_HPP
#define DIAMONDTRAP_HPP

#include <iostream>
#include "FlagTrap.hpp"
#include "ScavTrap.hpp"

class DiamondTrap : public FlagTrap, public ScavTrap
{
	private :
		std::string		_name;

	public :
		DiamondTrap(); // constructeur par defaut
		DiamondTrap(std::string const name); // constructeur par defaut avec name
		DiamondTrap (DiamondTrap const & cpy); // constructeur par copie
		~DiamondTrap(); // destructeur
		DiamondTrap & operator=(DiamondTrap const &other); // operateur d'assignation

		// std::string getName() const;
		// unsigned int getHitPoint() const;
		// unsigned int getEnergyPoint() const;
		// unsigned int getAttackDamage() const;

		// void attack(const std::string& target);
		// void takeDamage(unsigned int amount);
		// void beRepaired(unsigned int amount);
		void whoAmI();

		// void print_stats() const;
	};
	
	void print_rules();

#endif