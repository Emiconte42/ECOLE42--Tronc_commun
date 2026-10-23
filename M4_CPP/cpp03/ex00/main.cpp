#include "ClapTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	print_rules();

	std::cout << "===== Part 1 =====" << std::endl;
	{
		ClapTrap	robot("Bob");

		robot.print_stats();
		robot.attack("Sarah");
		robot.print_stats();
		robot.takeDamage(4);
		robot.print_stats();
		robot.beRepaired(3);
		robot.print_stats();
	}

	std::cout << "\n===== Part 2 =====" << std::endl;
	{
		ClapTrap	robot("Bob");

		for (int i = 0; i < 11; i++)
			robot.attack("Sarah");
		robot.beRepaired(5);
		robot.print_stats();
	}

	std::cout << "\n===== Part 3 =====" << std::endl;
	{
		ClapTrap	robot("Bob");

		robot.takeDamage(100);
		robot.attack("Sarah");
		robot.beRepaired(5);
		robot.print_stats();
	}

	std::cout << "\n===== Part 4 = copie et affectation =====" << std::endl;
	{
		ClapTrap	robot("Bob");
		robot.takeDamage(4);

		ClapTrap	robot2(robot);
		robot2.print_stats();

		ClapTrap	robot3;
		robot3.print_stats();

		robot3 = robot;

		robot2.print_stats();
		robot3.print_stats();
	}

	return (0);
}