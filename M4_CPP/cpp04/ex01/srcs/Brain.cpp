#include "Brain.hpp"
#include "utils.hpp"


Brain::Brain()
{
	std::cout << GREEN << "Brain : Default constructor called" << endofline;
	for (unsigned int i = 0; i < 100; i++)
	{
		_ideas[i] = "Pas d'idee";
	}
}

Brain::Brain(Brain const &cpy)
{
	std::cout << GREEN << "Brain : Copy constructor called" << endofline;
	*this = cpy; // Brain n'a pas de pointeur, on reutilise operator=
}
Brain & Brain::operator=(Brain const &other)
{
	std::cout << "Brain : Copy assignment operator called" << endofline;
	for (unsigned int i = 0; i < 100; i++)
	{
		_ideas[i] = other._ideas[i];
	}
	return (*this);
}

Brain::~Brain()
{
	std::cout << RED << "Brain : Destructor called" << endofline;
}

// on verifie que l'index est entre 0 et 99, sinon on sort du tableau
void Brain::setIdeas(unsigned int index, std::string const ideas)
{
	if (index < 100)
		this->_ideas[index] = ideas;
}

std::string Brain::getIdeas(unsigned int index) const
{
	if (index < 100)
		return (this->_ideas[index]);
	return ("Index invalide");
}
