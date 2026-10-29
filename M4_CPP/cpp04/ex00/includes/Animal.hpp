#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <iostream>
#include <string>

class Animal
{
	protected :
		std::string type;

	public :
		Animal();
		Animal(std::string const &animalType);
		Animal(Animal const &cpy);
		Animal & operator=(Animal const &other);
		virtual ~Animal(); // virtual : delete via Animal* doit aussi appeler ~Cat / ~Dog

		std::string const &getType() const;
		virtual void makeSound(void) const; // virtual : but de l'exo, le type reel de l'objet decide
};

#endif