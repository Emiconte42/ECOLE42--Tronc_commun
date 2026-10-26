#ifndef WRONGCAT_HPP
#define WRONGCAT_HPP

#include <iostream>
#include "WrongAnimal.hpp"

class WrongCat : public virtual WrongAnimal
{

	public :
		WrongCat();
		WrongCat(std::string const type);
		WrongCat(std::string const &cpy);
		WrongCat & operator=(WrongCat const &other);
		~WrongCat();

		void makeSound();
};

#endif