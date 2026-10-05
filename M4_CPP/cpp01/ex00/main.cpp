#include "Zombie.hpp"

int	main(void)
{
	Zombie	*zombie;
	randomChump("Zombie Staaaack");

	zombie = newZombie("Zombie Heaaaap");
	delete(zombie);
	return (0);
}