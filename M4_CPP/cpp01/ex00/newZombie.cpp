#include "Zombie.hpp"

Zombie*	newZombie(std::string name)
{
	Zombie	*zombie = new Zombie();
	if (!zombie)
		return (NULL);
	zombie->setName(name);
	zombie->announce();
	return (zombie);
}