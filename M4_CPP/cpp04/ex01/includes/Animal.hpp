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
		virtual ~Animal(); // virtual : sinon delete d'un Animal* n'appelle pas ~Dog / ~Cat et on a une fuite

		std::string const &getType() const;
		virtual void makeSound(void) const; // virtual : c'est le vrai type de l'objet qui decide, chaque animal fait son bruit
};

#endif