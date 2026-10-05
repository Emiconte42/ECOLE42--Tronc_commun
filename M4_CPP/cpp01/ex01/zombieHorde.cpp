#include "Zombie.hpp"

Zombie* zombieHorde( int N, std::string name )
{
	Zombie	*hordeZombie;
	int i = 0;

	hordeZombie = new Zombie[N];
	while ( i < N)
	{
		hordeZombie[i].setName(name);
		i++;
	}
	return (hordeZombie);
}