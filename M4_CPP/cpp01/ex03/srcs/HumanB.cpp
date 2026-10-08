#include "../includes/HumanB.hpp"

HumanB::HumanB(std::string const & humanBName) : _weapon(NULL)
{
	_humanBName = humanBName;
}

HumanB::~HumanB() {}

void HumanB::setWeapon(Weapon& weapon)
{
	_weapon = &weapon;
}
void	HumanB::attack(void)
{
	if (_weapon == NULL)
		std::cout << _humanBName << " has no weapon to attack with" << std::endl;
	else
		std::cout << _humanBName << " attacks with their " << _weapon->getType() << std::endl;
}