#ifndef AANIMAL_HPP
#define AANIMAL_HPP

#include <iostream>
#include <string>

class AAnimal
{
	protected :
		std::string type;

	public :
		AAnimal();
		AAnimal(std::string const &animalType);
		AAnimal(AAnimal const &cpy);
		AAnimal & operator=(AAnimal const &other);
		virtual ~AAnimal();

		std::string const &getType() const;
		virtual void makeSound(void) const = 0;
};

#endif