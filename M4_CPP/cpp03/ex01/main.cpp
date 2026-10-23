#include "includes/ClapTrap.hpp"
#include "includes/ScavTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	print_rules();
	{
		std::cout << "===== ScavTrap =====" << std::endl;

		ScavTrap	scav("Lola");

		scav.print_stats();
		scav.attack("Bob");
		scav.guardGate();
		scav.guardGate();
		scav.beRepaired(10);
		scav.takeDamage(2);
		scav.print_stats();
	}
	return (0);
}