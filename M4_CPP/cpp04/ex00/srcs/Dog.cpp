#include "Dog.hpp"
#include "utils.hpp"

Dog::Dog() : Animal()
{
	std::cout << GREEN << "Dog : Default constructor called" << endofline;
	type = "Dog";
}

Dog::Dog(Dog const &cpy) : Animal(cpy)
{
	std::cout << GREEN << "Dog : Copy constructor called" << endofline;
}
Dog & Dog::operator=(Dog const &other)
{
	std::cout << "Dog : Copy assignment operator called" << endofline;
	Animal::operator=(other);
	return (*this);
}

Dog::~Dog()
{
	std::cout << RED << "Dog : Destructor called" << endofline;
}

void Dog::makeSound(void) const
{
	std::cout << BLUE << "Dog : ...WOUAF WOUAF..." << endofline;
}