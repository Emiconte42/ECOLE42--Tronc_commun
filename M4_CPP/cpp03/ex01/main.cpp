#include "includes/ClapTrap.hpp"
#include "includes/ScavTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	ClapTrap	robot1("Bob");
	ScavTrap	robot2("Lola");

	print_rules();

	robot1.print_stats();
	robot1.attack("Lola");
	robot1.takeDamage(30);
	robot1.beRepaired(10);
	robot1.print_stats();

	robot2.print_stats();
	robot2.attack("Bob");
	robot2.takeDamage(30);
	robot2.beRepaired(10);
	robot2.guardGate();
	robot2.guardGate();
	robot2.print_stats();
	
	return (0);
}