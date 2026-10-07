#include "../includes/Weapon.hpp"

std::string const &Weapon::getType() const
{
	return (this->_type);
}

void Weapon::setType(const std::string &newType)
{
	_type = newType;
}

Weapon::Weapon(std::string const &weaponType)
{
	_type = weaponType;
}

Weapon::~Weapon()
{}