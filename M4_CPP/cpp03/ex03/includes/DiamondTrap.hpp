#ifndef DIAMONDTRAP_HPP
#define DIAMONDTRAP_HPP

#include <iostream>
#include <string>
#include "FragTrap.hpp"
#include "ScavTrap.hpp"

class DiamondTrap : public FragTrap, public ScavTrap // DiamondTrap herite de FT ET de ST + grace au "virtual" dans FT & ST, il n'y a qu'UN SEUL ClapTrap
{
	private :
		std::string		_name;	// meme nom que ClapTrap::_name 
								// il "masque" celui de ClapTrap::_name = nom + "_clap_name"
	public :
		DiamondTrap();
		DiamondTrap(std::string const name);
		DiamondTrap (DiamondTrap const & cpy);
		DiamondTrap & operator=(DiamondTrap const &other);
		~DiamondTrap();

		void whoAmI();			// affiche _name et ClapTrap::_name
		using ScavTrap::attack;	// indique a DiamondTrap d'utiliser ScavTrap
	};

#endif