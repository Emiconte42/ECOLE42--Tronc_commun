#ifndef CAT_HPP
#define CAT_HPP

#include <iostream>
#include "Animal.hpp"

class Cat : public Animal
{
	public :
		Cat();
		Cat(std::string const type);
		Cat(std::string const &cpy);
		Cat & operator=(Cat const &other);
		~Cat();

		void makeSound();
};

#endif