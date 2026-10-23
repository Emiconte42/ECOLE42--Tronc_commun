#include "includes/ClapTrap.hpp"
#include "includes/ScavTrap.hpp"
#include "includes/FragTrap.hpp"
#include "utils/utils.hpp"

int main()
{
	print_rules();
	{
		std::cout << "===== FragTrap =====" << std::endl;

		ScavTrap	scav("Bob");
		FragTrap	frag("Lola");

		frag.print_stats();
		frag.attack("Elisa");
		frag.highFivesGuys();
		frag.print_stats();
		scav.guardGate();
		scav.print_stats();
	}
	return (0);
}
