#include "Dog.hpp"
#include "Brain.hpp"
#include "utils.hpp"

Dog::Dog() : AAnimal()
{
	std::cout << GREEN << "Dog : Default constructor called" << endofline;
	type = "Dog";
	_brain = new Brain();
}

Dog::Dog(Dog const &cpy) : AAnimal(cpy)
{
	std::cout << GREEN << "Dog : Copy constructor called" << endofline;
	_brain = new Brain(*cpy._brain);
}
Dog & Dog::operator=(Dog const &other)
{
	std::cout << "Dog : Copy assignment operator called" << endofline;
	if (this != &other)
	{
		delete (_brain);
		_brain = new Brain(*other._brain);
		AAnimal::operator=(other);
	}
	return (*this);
}

Dog::~Dog()
{
	std::cout << RED << "Dog : Destructor called" << endofline;
	delete _brain;
}

void Dog::makeSound(void) const
{
	std::cout << BLUE << "Dog : ...WOUAF WOUAF..." << endofline;
}
Brain *Dog::getBrain(void) const
{
	return (_brain);
}
