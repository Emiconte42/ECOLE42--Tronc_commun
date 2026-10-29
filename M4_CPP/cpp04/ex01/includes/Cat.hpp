#ifndef CAT_HPP
#define CAT_HPP

#include <iostream>
#include "Animal.hpp"
#include "Brain.hpp"

class Cat : public Animal
{
	public :
		Cat();
		Cat(Cat const &cpy);
		Cat & operator=(Cat const &other);
		~Cat();

		virtual void makeSound(void) const;
		Brain	*getBrain(void) const; // getter : pour verifier la deep copy dans le main

		private :
		Brain	*_brain; // pointeur : la copie et operator= doivent faire une deep copy
};

#endif