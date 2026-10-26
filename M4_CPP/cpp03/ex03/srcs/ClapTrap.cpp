#include "../includes/ClapTrap.hpp"
#include "../utils/utils.hpp"

ClapTrap::ClapTrap() : _name("Default"), _hitPoints(10), _energyPoint(10), _attackDamage(0)
{
	std::cout << GREEN << "ClapTrap : Default constructor called" << endofline;
}

ClapTrap::ClapTrap(std::string const name) : _name(name), _hitPoints(10), _energyPoint(10), _attackDamage(0)
{
	std::cout << GREEN << "ClapTrap : Parametric constructor called" << endofline;
}

ClapTrap::ClapTrap(ClapTrap const & cpy)
{
	std::cout << GREEN << "ClapTrap : Copy constructor called" << endofline;
	*this = cpy; // utilise operator= pour copier les 4 attributs
}

ClapTrap::~ClapTrap()
{
	std::cout << RED << "ClapTrap : Destructor called" << endofline;
}

ClapTrap &ClapTrap::operator=(ClapTrap const &other)
{
	std::cout << "ClapTrap : Copy assignment operator called" << endofline;
	if (this != &other) // protection evite de se copier sur soi-meme (a = a)
	{
		this->_name = other._name;
		this->_hitPoints = other._hitPoints;
		this->_energyPoint = other._energyPoint;
		this->_attackDamage = other._attackDamage;
	}
	return (*this);
}

void ClapTrap::attack(const std::string& target)
{
	if (_hitPoints < 1)
	{
		std::cout << BOLD << "ClapTrap : " << _name << " can't attack: no hit points left!" << endofline;
		return;
	}
	if (_energyPoint < 1)
	{
		std::cout << BOLD << "ClapTrap : " << _name << " can't attack: no energy points left!" << endofline;
		return;
	}
	else
		_energyPoint -= 1;
	std::cout << BOLD << "ClapTrap : " << _name << " attack " << target << ", causing " << _attackDamage << " points of damage!" << endofline;
	return;
}

void ClapTrap::takeDamage(unsigned int amount)
{
	if (amount >= _hitPoints)
	{
		_hitPoints = 0;
		std::cout << BOLD << "ClapTrap : " << _name << " takes " << amount << " points of damage and has no hit points left!" << endofline;
		return;
	}
	else
		_hitPoints -= amount;
	std::cout << BOLD << "ClapTrap : " << _name << " takes " << amount << " points of damage !" << endofline;
}

void ClapTrap::beRepaired(unsigned int amount)
{
	if (_energyPoint == 0)
	{
		std::cout << BOLD << "ClapTrap : " << _name << " can't repair itself: no energy points left!" << endofline;
		return;
	}
	if (_hitPoints == 0)
	{
		std::cout << BOLD << "ClapTrap : " << _name << " can't repair itself: no hit points left!" << endofline;
		return;
	}
	if (amount > (UINT_MAX - _hitPoints)) // Protection contre le depassement
	{
		_hitPoints = UINT_MAX;
		std::cout << BOLD << "ClapTrap : " << _name << " repairs itself, regaining " << amount << " hit points!" << std::endl;
		std::cout << "But " << _name << " hit point is max at 4294967295" << endofline;
		_energyPoint -= 1;
		return;
	}
	else
	{
		_hitPoints += amount;
		_energyPoint -= 1;
		std::cout << BOLD << "ClapTrap : " << _name << " repairs itself, regaining " << amount << " hit points!" << endofline;
	}
	return;
}

void ClapTrap::print_stats() const
{
	std::cout << std::endl;
	std::cout << DIM << "----- Statistiques de " << _name << " -----" << std::endl;
	std::cout << "Hit Point : \t" << _hitPoints << std::endl;
	std::cout << "Energy Point : \t" << _energyPoint << std::endl;
	std::cout << "Attack Damage : " << _attackDamage << std::endl << endofline << RESET;
}

void print_rules()
{
	std::cout << BLUE << "/////////////// Rules of the game ///////////////" << std::endl;
	std::cout << "attack : -1 energy point" << std::endl;
	std::cout << "takeDamage : '- X' hit point" << std::endl;
	std::cout << "beRepaired : '+ X' hit point, -1 energy point" << std::endl;
	std::cout << "No hit points or no energy points : can't attack or repair" << endofline;
}
