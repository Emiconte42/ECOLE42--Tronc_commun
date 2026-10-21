#include "ClapTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	ClapTrap	robot("Bob");
	ClapTrap	robot2(robot);
	ClapTrap	robot3;

	robot3 = robot;
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

	robot2.attack("bobby");
	robot3.attack("Sylvia");
	robot.print_stats();
	robot.takeDamage(1);
	

	robot.print_stats();
	robot.beRepaired(400);

	robot.print_stats();
	robot.takeDamage(20);

	robot.print_stats();
	robot.beRepaired(150);

	robot.print_stats();
	return(0);
}