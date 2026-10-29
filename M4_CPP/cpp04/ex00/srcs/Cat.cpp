#include "Cat.hpp"
#include "utils.hpp"

Cat::Cat() : Animal()
{
	std::cout << GREEN << "Cat : Default constructor called" << endofline;
	type = "Cat";
}

Cat::Cat(Cat const &cpy) : Animal(cpy)
{
	std::cout << GREEN << "Cat : Copy constructor called" << endofline;
}
Cat & Cat::operator=(Cat const &other)
{
	std::cout << "Cat : Copy assignment operator called" << endofline;
	Animal::operator=(other);
	return (*this);
}

Cat::~Cat()
{
	std::cout << RED << "Cat : Destructor called" << endofline;
}

void Cat::makeSound(void) const
{
	std::cout << BLUE << "Cat : ...MIAOUUUUUU..." << endofline;
}