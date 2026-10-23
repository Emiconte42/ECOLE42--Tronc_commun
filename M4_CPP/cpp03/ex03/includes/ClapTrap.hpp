#ifndef CLAPTRAP_HPP
#define CLAPTRAP_HPP

#include <iostream>
#include <limits.h>
#include <string>

class ClapTrap
{
	protected :	// Protected : Partage les attributs avec les classes heritees. Private: ne partage rien a personne reserve a la classe elle meme.
		std::string		_name;
		unsigned int	_hitPoints;
		unsigned int	_energyPoint;
		unsigned int	_attackDamage;

	public :
		ClapTrap();
		ClapTrap(std::string const name);
		ClapTrap (ClapTrap const & cpy);
		ClapTrap & operator=(ClapTrap const &other);
		~ClapTrap();

		void attack(const std::string& target);
		void takeDamage(unsigned int amount);
		void beRepaired(unsigned int amount);
		void print_stats() const;
	};

	void print_rules();

#endif