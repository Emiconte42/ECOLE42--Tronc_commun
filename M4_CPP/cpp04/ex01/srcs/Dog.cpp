#include "Dog.hpp"
#include "Brain.hpp"
#include "utils.hpp"

Dog::Dog() : Animal()
{
	std::cout << GREEN << "Dog : Default constructor called" << endofline;
	type = "Dog";
	_brain = new Brain(); // le Dog cree son propre Brain
}

Dog::Dog(Dog const &cpy) : Animal(cpy)
{
	std::cout << GREEN << "Dog : Copy constructor called" << endofline;
	_brain = new Brain(*cpy._brain); // deep copy : nouveau Brain avec le contenu de celui de cpy
}
Dog & Dog::operator=(Dog const &other)
{
	std::cout << "Dog : Copy assignment operator called" << endofline;
	if (this != &other) // on protege car si a = a, on delete le Brain qu'on veut copier
	{
		delete (_brain); // l'ancien Brain existe deja : on libere sinon fuite
		_brain = new Brain(*other._brain); // deep copy, pareil que le constructeur de copie
		Animal::operator=(other);
	}
	return (*this);
}

Dog::~Dog()
{
	std::cout << RED << "Dog : Destructor called" << endofline;
	delete _brain; // on detruit le Brain qu'on a cree. Ordre : ~Dog -> ~Brain -> ~Animal
}

void Dog::makeSound(void) const
{
	std::cout << BLUE << "Dog : ...WOUAF WOUAF..." << endofline;
}
Brain *Dog::getBrain(void) const
{
	return (_brain);
}
