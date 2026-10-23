#include "../includes/ScavTrap.hpp"
#include "../utils/utils.hpp"

ScavTrap::ScavTrap()
{
	std::cout << GREEN << "ScavTrap : Default constructor called" << endofline;
	_name = "none";
	_hitPoints = 100;
	_energyPoint = 50;
	_attackDamage = 20;
	_guardGate = false;
}

ScavTrap::ScavTrap(std::string const name) : ClapTrap(name)
{
	std::cout << GREEN << "ScavTrap : Parametric constructor called" << endofline;
	_hitPoints = 100;
	_energyPoint = 50;
	_attackDamage = 20;
	_guardGate = false;
}

ScavTrap::ScavTrap(ScavTrap const & cpy) : ClapTrap(cpy)
{
	std::cout << GREEN << "ScavTrap : Copy constructor called" << endofline;
	*this = cpy;
}

ScavTrap::~ScavTrap()
{
	std::cout << RED << "ScavTrap : Destructor called" << endofline;
}

bool ScavTrap::getGuardGate() const 
{
	return (this->_guardGate);
}

ScavTrap &ScavTrap::operator=(ScavTrap const &other)
{
	std::cout << "ScavTrap : Copy assignment operator called" << endofline;
	if (this != &other) // protection : evite de se copier sur soi-meme (a = a)
	{
		this->_name = other.getName();
		this->_hitPoints = other.getHitPoint();
		this->_energyPoint = other.getEnergyPoint();
		this->_attackDamage = other.getAttackDamage();
		this->_guardGate = other.getGuardGate();
	}
	return (*this);
}

void ScavTrap::attack(const std::string& target)
{
	if (_hitPoints < 1)
	{
		std::cout << BOLD << "ScavTrap : " << _name << " can't attack: no hit points left!" << endofline;
		return;
	}
	if (_energyPoint < 1)
	{
		std::cout << BOLD << "ScavTrap : " << _name << " can't attack: no energy points left!" << endofline;
		return;
	}
	else
		_energyPoint -= 1;
	std::cout << BOLD << "ScavTrap : " << _name << " attack " << target << ", causing " << _attackDamage << " points of damage!" << endofline;
	return;
}

void ScavTrap::guardGate()
{
	if (_guardGate == true)
		std::cout << BOLD << "ScavTrap : " << _name << " is already guarding the gate!" << endofline;
	else
	{
		std::cout << BOLD << "ScavTrap : " << _name << " is now in Gate keeper mode" << endofline;
		_guardGate = true;
	}
}

void ScavTrap::print_stats() const
{
	std::cout << std::endl;
	std::cout << DIM << "----- ScavTrap Statistiques de " << _name << " -----" << std::endl;
	std::cout << "Hit Point : \t" << _hitPoints << std::endl;
	std::cout << "Energy Point : \t" << _energyPoint << std::endl;
	std::cout << "Attack Damage : " << _attackDamage << std::endl;
	std::cout << "Guard Gate : " << _guardGate << std::endl << endofline;
}