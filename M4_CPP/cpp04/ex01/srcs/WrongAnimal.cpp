#include "WrongAnimal.hpp"
#include "utils.hpp"

WrongAnimal::WrongAnimal() : type("WrongAnimal")
{
	std::cout << GREEN << "WrongAnimal : Default constructor called" << endofline;
}

WrongAnimal::WrongAnimal(std::string const &animalType) : type(animalType)
{
	std::cout << GREEN << "WrongAnimal : Parametric constructor called" << endofline;
}
WrongAnimal::WrongAnimal(WrongAnimal const &cpy)
{
	std::cout << GREEN << "WrongAnimal : Copy constructor called" << endofline;
	*this = cpy;
}
WrongAnimal & WrongAnimal::operator=(WrongAnimal const &other)
{
	std::cout << "WrongAnimal : Copy assignment operator called" << endofline;
	if (this != &other)
		this->type = other.type;
	return (*this);
}

WrongAnimal::~WrongAnimal()
{
	std::cout << RED << "WrongAnimal : Destructor called" << endofline;
}

std::string const &WrongAnimal::getType() const
{
	return(this->type);
}

void WrongAnimal::makeSound(void) const
{
	std::cout << BLUE << "WrongAnimal : ...Je suis un Wrong Animal de la foret..." << endofline;
}