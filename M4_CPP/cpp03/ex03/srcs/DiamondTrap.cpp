#include "../includes/DiamondTrap.hpp"
#include "../utils/utils.hpp"

// DiamondTrap::DiamondTrap()
// {
// 	std::cout << GREEN << "DiamondTrap : Default constructor called" << endofline;
// 	_name = "none";
// 	_hitPoints = 100;
// 	_energyPoint = 50;
// 	_attackDamage = 20;
// 	_guardGate = false;
// }

// DiamondTrap::DiamondTrap(std::string const name) : ClapTrap(name)
// {
// 	std::cout << GREEN << "DiamondTrap : Parametric constructor called" << endofline;
// 	_hitPoints = 100;
// 	_energyPoint = 50;
// 	_attackDamage = 20;
// 	_guardGate = false;
// }

// DiamondTrap::DiamondTrap(DiamondTrap const & cpy) : ClapTrap(cpy)
// {
// 	std::cout << GREEN << "DiamondTrap : Copy constructor called" << endofline;
// 	*this = cpy;
// }

// DiamondTrap::~DiamondTrap()
// {
// 	std::cout << RED << "DiamondTrap : Destructor called" << endofline;
// }

// bool DiamondTrap::getGuardGate() const 
// {
// 	return (this->_guardGate);
// }

// DiamondTrap &DiamondTrap::operator=(DiamondTrap const &other)
// {
// 	std::cout << GREEN << "Copy assignment operator called" << endofline;
// 	if (this != &other)
// 	{
// 		this->_name = other.getName();
// 		this->_hitPoints = other.getHitPoint();
// 		this->_energyPoint = other.getEnergyPoint();
// 		this->_attackDamage = other.getAttackDamage();
// 		this->_guardGate = other.getGuardGate();
// 	}
// 	return (*this);
// }

// void DiamondTrap::attack(const std::string& target)
// {
// 	if (_hitPoints < 1)
// 	{
// 		std::cout << BOLD << "DiamondTrap : " << _name << " can't attack: no hit points left!" << endofline;
// 		return;
// 	}
// 	if (_energyPoint < 1)
// 	{
// 		std::cout << BOLD << "DiamondTrap : " << _name << " can't attack: no energy points left!" << endofline;
// 		return;
// 	}
// 	else
// 		_energyPoint -= 1;
// 	std::cout << BOLD << "DiamondTrap " << _name << " attack " << target << ", causing " << _attackDamage << " points of damage!" << endofline;
// 	return;
// }

// void DiamondTrap::guardGate()
// {
// 	if (_guardGate == true)
// 		std::cout << BOLD << "DiamondTrap : " << _name << " is already guarding the gate!" << endofline;
// 	else
// 	{
// 		std::cout << BOLD << "DiamondTrap : " << _name << " is now in Gate keeper mode" << endofline;
// 		_guardGate = true;
// 	}
// }

// void DiamondTrap::print_stats() const
// {
// 	std::cout << std::endl;
// 	std::cout << DIM << "----- DiamondTrap Statistiques de " << _name << " -----" << std::endl;
// 	std::cout << "Hit Point : \t" << _hitPoints << std::endl;
// 	std::cout << "Energy Point : \t" << _energyPoint << std::endl;
// 	std::cout << "Attack Damage : " << _attackDamage << std::endl;
// 	std::cout << "Guard Gate : " << _guardGate << std::endl << endofline;
// }