#ifndef WRONGANIMAL_HPP
#define WRONGANIMAL_HPP

#include <iostream>
#include <string>

class WrongAnimal
{
	protected :
		std::string type;
	public :
		WrongAnimal();
		WrongAnimal(std::string const &animalType);
		WrongAnimal(WrongAnimal const &cpy);
		WrongAnimal & operator=(WrongAnimal const &other);
		virtual ~WrongAnimal(); // virtual : delete via WrongAnimal* doit aussi appeler ~WrongCat

		std::string const &getType() const;
		void makeSound(void) const; // PAS virtual : but de l'exo, le type du pointeur decide
};

#endif