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
		virtual ~WrongAnimal();

		std::string const &getType() const;
		void makeSound(void) const; // pas virtual : ici c'est le type du pointeur qui decide (le mauvais exemple)
};

#endif