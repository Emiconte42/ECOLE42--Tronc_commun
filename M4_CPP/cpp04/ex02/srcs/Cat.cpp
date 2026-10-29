#include "Cat.hpp"
#include "Brain.hpp"
#include "utils.hpp"

Cat::Cat() : AAnimal()
{
	std::cout << GREEN << "Cat : Default constructor called" << endofline;
	type = "Cat";
	_brain = new Brain();
}

Cat::Cat(Cat const &cpy) : AAnimal(cpy)
{
	std::cout << GREEN << "Cat : Copy constructor called" << endofline;
	_brain = new Brain(*cpy._brain);
}
Cat & Cat::operator=(Cat const &other)
{
	std::cout << "Cat : Copy assignment operator called" << endofline;
	if (this != &other)
	{
		delete (_brain);
		_brain = new Brain(*other._brain);
		AAnimal::operator=(other);
	}
	return (*this);
}

Cat::~Cat()
{
	std::cout << RED << "Cat : Destructor called" << endofline;
	delete _brain;
}

void Cat::makeSound(void) const
{
	std::cout << BLUE << "Cat : ...MIAOUUUUUU..." << endofline;
}
Brain *Cat::getBrain(void) const
{
	return (_brain);
}
