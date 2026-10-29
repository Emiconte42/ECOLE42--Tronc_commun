#include "Cat.hpp"
#include "Brain.hpp"
#include "utils.hpp"

Cat::Cat() : Animal()
{
	std::cout << GREEN << "Cat : Default constructor called" << endofline;
	type = "Cat";
	_brain = new Brain(); // le Cat cree son propre Brain
}

Cat::Cat(Cat const &cpy) : Animal(cpy)
{
	std::cout << GREEN << "Cat : Copy constructor called" << endofline;
	_brain = new Brain(*cpy._brain); // deep copy : nouveau Brain avec le contenu de celui de cpy
}
Cat & Cat::operator=(Cat const &other)
{
	std::cout << "Cat : Copy assignment operator called" << endofline;
	if (this != &other) // on protege car si a = a, on delete le Brain qu'on veut copier
	{
		delete (_brain); // l'ancien Brain existe deja : on libere sinon fuite
		_brain = new Brain(*other._brain); // deep copy, pareil que le constructeur de copie
		Animal::operator=(other);
	}
	return (*this);
}

Cat::~Cat()
{
	std::cout << RED << "Cat : Destructor called" << endofline;
	delete _brain; // on detruit le Brain qu'on a cree. Ordre : ~Cat -> ~Brain -> ~Animal
}

void Cat::makeSound(void) const
{
	std::cout << BLUE << "Cat : ...MIAOUUUUUU..." << endofline;
}
Brain *Cat::getBrain(void) const
{
	return (_brain);
}
