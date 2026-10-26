#ifndef DOG_HPP
#define DOG_HPP

#include <iostream>
#include "Animal.hpp"

class Dog : public Animal
{
	public :
		Dog();
		Dog(std::string const type);
		Dog(std::string const &cpy);
		Dog & operator=(Dog const &other);
		~Dog();

		std::string getType() const;
		void makeSound();
};

#endif