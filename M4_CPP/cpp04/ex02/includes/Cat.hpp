#ifndef CAT_HPP
#define CAT_HPP

#include <iostream>
#include "AAnimal.hpp"
#include "Brain.hpp"

class Cat : public AAnimal
{
	public :
		Cat();
		Cat(Cat const &cpy);
		Cat & operator=(Cat const &other);
		~Cat();

		virtual void makeSound(void) const;
		Brain	*getBrain(void) const;

		private :
		Brain	*_brain;
};

#endif