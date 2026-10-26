#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <iostream>

class Animal
{
	protected :
		std::string type;
	public :
		Animal();
		Animal(std::string const type);
		Animal(std::string const &cpy);
		Animal & operator=(Animal const &other);
		~Animal();

		std::string getType() const;
		void makeSound();
};

#endif