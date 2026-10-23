#include "../includes/FragTrap.hpp"
#include "../utils/utils.hpp"

FragTrap::FragTrap() : ClapTrap() // ": ClapTrap()" : on construit d'abord la partie ClapTrap
{
	std::cout << GREEN << "FragTrap : Default constructor called" << endofline;
	_hitPoints = 100; // ClapTrap a mis 10/10/0, on ecrase avec les valeurs du sujet 
	_energyPoint = 100;
	_attackDamage = 30;
}

FragTrap::FragTrap(std::string const name) : ClapTrap(name) // ": ClapTrap(name)" : on transmet le nom au constructeur de ClapTrap
{
	std::cout << GREEN << "FragTrap : Parametric constructor called" << endofline;
	_hitPoints = 100;
	_energyPoint = 100;
	_attackDamage = 30;
}

FragTrap::FragTrap(FragTrap const & cpy) : ClapTrap(cpy)
{
	std::cout << GREEN << "FragTrap : Copy constructor called" << endofline;
}

FragTrap::~FragTrap()
{
	std::cout << RED << "FragTrap : Destructor called" << endofline;
}

// Operateur d'affectation
// FragTrap n'a aucun attribut a lui : il n'y a rien a copier dans cette classe
FragTrap &FragTrap::operator=(FragTrap const &other)
{
	std::cout << "FragTrap : Copy assignment operator called" << endofline;
	ClapTrap::operator=(other); // on demande a ClapTrap de copier SA partie (nom, hit points...)
	return (*this);
}

void FragTrap::highFivesGuys(void)
{
	std::cout << BOLD << "FragTrap : " << _name << " : Hey guys, high five ? ✋" << endofline; // _name : attribut herite de ClapTrap
}