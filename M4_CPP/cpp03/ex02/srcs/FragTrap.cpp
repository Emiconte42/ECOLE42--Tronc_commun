#include "../includes/FragTrap.hpp"
#include "../utils/utils.hpp"

FragTrap::FragTrap()
{
	std::cout << GREEN << "FragTrap : Default constructor called" << endofline;
	_name = "none";
	_hitPoints = 100;
	_energyPoint = 100;
	_attackDamage = 30;
}

FragTrap::FragTrap(std::string const name) : ClapTrap(name)
{
	std::cout << GREEN << "FragTrap : Parametric constructor called" << endofline;
	_hitPoints = 100;
	_energyPoint = 100;
	_attackDamage = 30;
}

FragTrap::FragTrap(FragTrap const & cpy) : ClapTrap(cpy)
{
	std::cout << GREEN << "FragTrap : Copy constructor called" << endofline;
	*this = cpy;
}

FragTrap::~FragTrap()
{
	std::cout << RED << "FragTrap : Destructor called" << endofline;
}

FragTrap &FragTrap::operator=(FragTrap const &other)
{
	std::cout << "FragTrap : Copy assignment operator called" << endofline;
	if (this != &other) // protection : evite de se copier sur soi-meme (a = a)
	{
		this->_name = other.getName();
		this->_hitPoints = other.getHitPoint();
		this->_energyPoint = other.getEnergyPoint();
		this->_attackDamage = other.getAttackDamage();
	}
	return (*this);
}

void FragTrap::highFivesGuys(void)
{
	std::cout << BOLD << "FragTrap : " << _name << " : Hey guys, high five ? ✋" << endofline;
}