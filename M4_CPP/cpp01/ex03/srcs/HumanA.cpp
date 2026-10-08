#include "../includes/HumanA.hpp"

HumanA::HumanA(std::string const & humanAName, Weapon& weapon): _weapon(weapon)
{
	_humanAName = humanAName;
}

HumanA::~HumanA() {}

void	HumanA::attack(void)
{
	std::cout << _humanAName << " attacks with their " << _weapon.getType() << std::endl;
}