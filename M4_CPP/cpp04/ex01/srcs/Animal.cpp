#include "Animal.hpp"
#include "utils.hpp"

Animal::Animal() : type("Animal")
{
	std::cout << GREEN << "Animal : Default constructor called" << endofline;
}

Animal::Animal(std::string const &animalType) : type(animalType)
{
	std::cout << GREEN << "Animal : Parametric constructor called" << endofline;
}

Animal::Animal(Animal const &cpy)
{
	std::cout << GREEN << "Animal : Copy constructor called" << endofline;
	*this = cpy;
}

Animal & Animal::operator=(Animal const &other)
{
	std::cout << "Animal : Copy assignment operator called" << endofline;
	if (this != &other)
		this->type = other.type;
	return (*this);
}

Animal::~Animal()
{
	std::cout << RED << "Animal : Destructor called" << endofline;
}

std::string const &Animal::getType() const
{
	return(this->type);
}

void Animal::makeSound(void) const
{
	std::cout << BLUE << "Animal : ...Je suis un animal de la foret..." << endofline;
}