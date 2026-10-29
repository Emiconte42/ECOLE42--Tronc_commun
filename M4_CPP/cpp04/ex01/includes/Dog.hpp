#ifndef DOG_HPP
#define DOG_HPP

#include <iostream>
#include "Animal.hpp"
#include "Brain.hpp"

class Dog : public Animal
{
	public :
		Dog();
		Dog(Dog const &cpy);
		Dog & operator=(Dog const &other);
		~Dog();

		virtual void makeSound(void) const;
		Brain	*getBrain(void) const; // getter : pour verifier la deep copy dans le main

		private :
		Brain	*_brain; // pointeur : la copie et operator= doivent faire une deep copy
};

#endif