#include "Zombie.hpp"

int	main(void)
{
	Zombie	*zombie;
	randomChump("Emilie");

	zombie = newZombie("Yoooo");
	delete(zombie);
	return (0);
}