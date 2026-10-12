#include "Zombie.hpp"

int	main(void)
{
	Zombie	*hordeZombie;
	int i = 0;
	int	nbZombie = 5;

	hordeZombie = zombieHorde(nbZombie, "horde of Zombies");
	while (i < nbZombie)
		hordeZombie[i++].announce();

	delete[] hordeZombie;
	return (0);
}