#include "../includes/ScavTrap.hpp"
#include "../utils/utils.hpp"

ScavTrap::ScavTrap() : ClapTrap() // ": ClapTrap()" : on construit d'abord la partie ClapTrap
{
	std::cout << GREEN << "ScavTrap : Default constructor called" << endofline;
	_hitPoints = 100; // ClapTrap a mis 10/10/0, on ecrase avec les valeurs du sujet
	_energyPoint = 50;
	_attackDamage = 20;
}

ScavTrap::ScavTrap(std::string const name) : ClapTrap(name) // ": ClapTrap(name)" : on transmet le nom au constructeur de ClapTrap
{
	std::cout << GREEN << "ScavTrap : Parametric constructor called" << endofline;
	_hitPoints = 100;
	_energyPoint = 50;
	_attackDamage = 20;
}

// Constructeur de copie
// ": ClapTrap(cpy)" : ScavTrap n'a pas d'attribut a lui, ClapTrap copie deja tout (nom et stats)
ScavTrap::ScavTrap(ScavTrap const & cpy) : ClapTrap(cpy)
{
	std::cout << GREEN << "ScavTrap : Copy constructor called" << endofline;
}

ScavTrap::~ScavTrap()
{
	std::cout << RED << "ScavTrap : Destructor called" << endofline;
}

// Operateur d'affectation
// ScavTrap  n'a aucun attribut a lui : il n'y a rien a copier dans cette classe
ScavTrap &ScavTrap::operator=(ScavTrap const &other)
{
	std::cout << "ScavTrap : Copy assignment operator called" << endofline;
	ClapTrap::operator=(other); // on demande a ClapTrap de copier SA partie (nom, hit points...)
	return (*this);
}

// attack : version PROPRE a ScavTrap
// Elle remplace celle de ClapTrap pour un ScavTrap : seul le message change ("ScavTrap" au lieu de "ClapTrap")
// Meme regles : pas d'attaque sans hit points ni energy points, cout 1 energy point
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

// guardGate : capacite speciale de ScavTrap
// Le sujet demande seulement d'afficher un message : le mode Gatekeeper
void ScavTrap::guardGate()
{
		std::cout << BOLD << "ScavTrap : " << _name << " is now in Gate keeper mode" << endofline;
}