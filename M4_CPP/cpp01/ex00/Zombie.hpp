#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <iostream>
#include <string>

class Zombie
{
	private:
		std::string	_name;

	public:
		Zombie();
		~Zombie();

		void announce(void);
		std::string getName() const; // getters permet de lire uniquement un attribut prive
		void setName(const std::string& newName); // setters permet de modifier un attribut prive
};

Zombie* newZombie( std::string name );
void randomChump( std::string name );

#endif