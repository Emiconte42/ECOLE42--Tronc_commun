#include "Zombie.hpp"

Zombie::Zombie()
{
}

Zombie::~Zombie()
{
	std::cout << _name << " : is dead" << std::endl;
}

std::string	Zombie::getName() const
{
	return _name;
}

void	Zombie::setName(const std::string newName)
{
	_name = newName;
	return ;
}

void	Zombie::announce()
{
	std::cout << _name << " : BraiiiiiiinnnzzzZ..." << std::endl;
}