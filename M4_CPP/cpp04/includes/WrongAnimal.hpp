#ifndef WRONGANIMAL_HPP
#define WRONGANIMAL_HPP

#include <iostream>

class WrongAnimal
{
	protected :
		std::string type;
	public :
		WrongAnimal();
		WrongAnimal(std::string const type);
		WrongAnimal(std::string const &cpy);
		WrongAnimal & operator=(WrongAnimal const &other);
		~WrongAnimal();

		void makeSound();
};

#endif