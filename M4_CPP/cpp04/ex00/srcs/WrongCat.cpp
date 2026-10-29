#include "WrongCat.hpp"
#include "utils.hpp"

WrongCat::WrongCat() : WrongAnimal()
{
	std::cout << GREEN << "WrongCat : Default constructor called" << endofline;
	type = "WrongCat";
}

WrongCat::WrongCat(WrongCat const &cpy) : WrongAnimal(cpy)
{
	std::cout << GREEN << "WrongCat : Copy constructor called" << endofline;
}
WrongCat & WrongCat::operator=(WrongCat const &other)
{
	std::cout << "WrongCat : Copy assignment operator called" << endofline;
	WrongAnimal::operator=(other);
	return (*this);
}

WrongCat::~WrongCat()
{
	std::cout << RED << "WrongCat : Destructor called" << endofline;
}

void WrongCat::makeSound(void) const
{
	std::cout << BLUE << "WrongCat : ...miou miou..." << endofline;
}