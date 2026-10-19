#include "ClapTrap.hpp"
#include "utils/utils.hpp"

ClapTrap::ClapTrap()
{
	std::cout << "Default constructor called" << std::endl;
}

ClapTrap::ClapTrap(std::string const name)
{
	std::cout << "Constructor called with parameter" << std::endl;
	_name = name;
}

ClapTrap::ClapTrap(ClapTrap const & cpy)
{
	std::cout << "Copy constructor called" << std::endl;
	*this = cpy;
}

ClapTrap::~ClapTrap()
{
	std::cout << "Destructor called" << std::endl;
}