#include "../includes/DiamondTrap.hpp"
#include "../utils/utils.hpp"

// Constructeur par defaut
// ClapTrap("Default_clap_name") : DiamondTrap choisit lui-meme comment construire ClapTrap
// (ClapTrap() seul donnerait "Default" : le sujet veut le suffixe "_clap_name")
// _name("Default") : le nom propre a DiamondTrap
DiamondTrap::DiamondTrap() : ClapTrap("Default_clap_name"), _name("Default") 
{
	std::cout << GREEN << "DiamondTrap : Default constructor called" << endofline;
	_attackDamage = 30; // FragTrap met 30 mais ScavTrap (construit apres) remet 20 : on remet 30 (car le sujet demande : FragTrap)
}

// Constructeur avec nom
// ClapTrap(name + "_clap_name") : DiamondTrap choisit le nom du ClapTrap (sujet : nom + "_clap_name")
// FragTrap(name), ScavTrap(name) : construisent leurs parties (leur appel a ClapTrap est ignore)
// _name(name) : le nom propre a DiamondTrap
DiamondTrap::DiamondTrap(std::string const name) : ClapTrap(name + "_clap_name") , FragTrap(name), ScavTrap(name), _name(name)
{
	std::cout << GREEN << "DiamondTrap : Parametric constructor called" << endofline;
	_attackDamage = 30;
}

// Constructeur de copie
// On passe cpy a CHAQUE parent, chacun copie sa partie.
DiamondTrap::DiamondTrap(DiamondTrap const & cpy) : ClapTrap(cpy), FragTrap(cpy), ScavTrap(cpy), _name(cpy._name)
{
	std::cout << GREEN << "DiamondTrap : Copy constructor called" << endofline;
}

DiamondTrap::~DiamondTrap()
{
	std::cout << RED << "DiamondTrap : Destructor called" << endofline;
}

// Operateur d'affectation
// On demande a ClapTrap de copier sa partie, FT et ST n'ont rien a eux
// puis on copie le _name de DiamondTrap
DiamondTrap &DiamondTrap::operator=(DiamondTrap const &other)
{
	std::cout << "DiamondTrap : Copy assignment operator called" << endofline;
	ClapTrap::operator=(other);
	_name = other._name;
	return (*this);
}

// whoAmI : Affiche les 2 noms : celui de DiamondTrap et celui du ClapTrap
void DiamondTrap::whoAmI()
{
	std::cout << BOLD << "DiamondTrap : " << _name << ", my ClapTrap name is : " << ClapTrap::_name << endofline;
}