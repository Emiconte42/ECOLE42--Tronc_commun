#include "includes/DiamondTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	std::cout << "===== Part 1 =====" << std::endl;
	{
		DiamondTrap robot("Bob");

		robot.whoAmI();			// DiamondTrap
		robot.print_stats();	// ClapTrap
		robot.attack("Cible");	// attack() de ScavTrap
		robot.guardGate();		// ScavTrap
		robot.highFivesGuys();	// FragTrap
	}

	std::cout << "\n===== Part 2 =====" << std::endl;
	{
		DiamondTrap robot("Bob");
		DiamondTrap robot2(robot);

		robot2.whoAmI();
		robot2.takeDamage(5);
		robot2.beRepaired(10);
		robot2.print_stats();
	}

	return (0);
}