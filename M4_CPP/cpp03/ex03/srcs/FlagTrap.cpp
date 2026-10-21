#include "../includes/FlagTrap.hpp"
#include "../utils/utils.hpp"

FlagTrap::FlagTrap()
{
	std::cout << GREEN << "FlagTrap : Default constructor called" << endofline;
	_name = "none";
	_hitPoints = 100;
	_energyPoint = 100;
	_attackDamage = 30;
}

FlagTrap::FlagTrap(std::string const name) : ClapTrap(name)
{
	std::cout << GREEN << "FlagTrap : Parametric constructor called" << endofline;
	_hitPoints = 100;
	_energyPoint = 100;
	_attackDamage = 30;
}

FlagTrap::FlagTrap(FlagTrap const & cpy) : ClapTrap(cpy)
{
	std::cout << GREEN << "FlagTrap : Copy constructor called" << endofline;
	*this = cpy;
}

FlagTrap::~FlagTrap()
{
	std::cout << RED << "FlagTrap : Destructor called" << endofline;
}

FlagTrap &FlagTrap::operator=(FlagTrap const &other)
{
	std::cout << GREEN << "Copy assignment operator called" << endofline;
	if (this != &other)
	{
		this->_name = other.getName();
		this->_hitPoints = other.getHitPoint();
		this->_energyPoint = other.getEnergyPoint();
		this->_attackDamage = other.getAttackDamage();
	}
	return (*this);
}

void FlagTrap::highFivesGuys(void)
{
	std::cout << BOLD << _name << " : Hey guys, high five ? ✋" << endofline;
}