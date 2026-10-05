#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <iostream>
#include <unistd.h>

class Zombie
{
	private:
		std::string	_name;

	public:
		Zombie();
		~Zombie();

		void announce(void);
		std::string getName() const;
		void setName(const std::string newName);
};

Zombie* zombieHorde( int N, std::string name );

#endif