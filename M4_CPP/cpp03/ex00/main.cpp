#include "ClapTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	ClapTrap	robot("Bob");

	robot.print_rules();

	robot.print_stats();
	robot.attack("Sarah");
	robot.attack("Sarah");
	robot.attack("Sarah");
	robot.attack("Sarah");
	robot.attack("Sarah");
	robot.attack("Sarah");
	// robot.attack("Sarah");
	// robot.attack("Sarah");
	// robot.attack("Sarah");
	// robot.attack("Sarah");
	// robot.attack("Sarah");
	// robot.attack("Sarah");

	robot.print_stats();
	robot.takeDamage(1);
	

	robot.print_stats();
	robot.beRepaired(4294967290);

	// robot.print_stats();
	// robot.takeDamage(40000);

	// robot.print_stats();
	// robot.beRepaired(150);

	robot.print_stats();
	return(0);
}