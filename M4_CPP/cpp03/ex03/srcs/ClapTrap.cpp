#include "../includes/ClapTrap.hpp"
#include "../utils/utils.hpp"

// Constructeur par defaut : un ClapTrap sans nom donne
// Liste d'initialisation : les attributs recoivent leur valeur des la creation
// Valeurs du sujet : 10 hit points, 10 energy points, 0 attack damage
ClapTrap::ClapTrap() : _name("Default"), _hitPoints(10), _energyPoint(10), _attackDamage(0)
{
	std::cout << GREEN << "ClapTrap : Default constructor called" << endofline;
}

ClapTrap::ClapTrap(std::string const name) : _name(name), _hitPoints(10), _energyPoint(10), _attackDamage(0)
{
	std::cout << GREEN << "ClapTrap : Parametric constructor called" << endofline;
}

// Constructeur de copie : cree un NOUVEAU ClapTrap a partir d'un ClapTrap existant (cpy)
// "const &" : on lit l'original sans le modifier, et sans en faire une copie de plus
ClapTrap::ClapTrap(ClapTrap const & cpy)
{
	std::cout << GREEN << "ClapTrap : Copy constructor called" << endofline;
	*this = cpy; // utilise operator= pour copier les 4 attributs
}

ClapTrap::~ClapTrap()
{
	std::cout << RED << "ClapTrap : Destructor called" << endofline;
}

// Operateur d'affectation : REMPLACE le contenu d'un ClapTrap qui existe deja (a = b)
// Retourne une reference (&) sur l'objet : permet les affectations en chaine (a = b = c)
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

// attack : ClapTrap attaque une cible
// Cout : 1 energy point. Impossible d'attaquer sans hit points ou sans energy points
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

// takeDamage : ClapTrap subit des degats (amount hit points en moins)
// "unsigned int" ne peut pas etre negatif : si on retirait plus que ce qu'il reste,
// le resultat deviendrait un tres grand nombre (4294967295...) au lieu de 0
// Donc on teste AVANT de soustraire
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

// beRepaired : ClapTrap se soigne (+ amount hit points)
// Cout : 1 energy point. Impossible sans energy points ou sans hit points
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

// AJOUT PERSO : affiche les stats du robot
// "const" : la fonction lit les attributs sans jamais les modifier
// Les classes filles (ScavTrap, FragTrap, DiamondTrap) l'heritent : on a pas besoin de la reecrire
void ClapTrap::print_stats() const
{
	std::cout << std::endl;
	std::cout << DIM << "----- Statistiques de " << _name << " -----" << std::endl;
	std::cout << "Hit Point : \t" << _hitPoints << std::endl;
	std::cout << "Energy Point : \t" << _energyPoint << std::endl;
	std::cout << "Attack Damage : " << _attackDamage << std::endl << endofline << RESET;
}

// AJOUT PERSO : affiche les regles du jeu au debut du main
void print_rules()
{
	std::cout << BLUE << "/////////////// Rules of the game ///////////////" << std::endl;
	std::cout << "attack : -1 energy point" << std::endl;
	std::cout << "takeDamage : '- X' hit point" << std::endl;
	std::cout << "beRepaired : '+ X' hit point, -1 energy point" << std::endl;
	std::cout << "No hit points or no energy points : can't attack or repair" << endofline;
}