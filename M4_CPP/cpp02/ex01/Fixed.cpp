#include "Fixed.hpp"

// Codes de l'exo00
Fixed::Fixed() : _rawBits(0) 
{
	std::cout << "Default constructor called" << std::endl;
}

Fixed::Fixed (Fixed const & cpy)
{
	std::cout << "Copy constructor called" << std::endl;
	*this = cpy;
}

Fixed &Fixed::operator=(Fixed const &other)
{
	std::cout << "Copy assignment operator called" << std::endl;
	if (this != &other)
		this->_rawBits = other.getRawBits();
	return (*this);
}

Fixed::~Fixed()
{
	std::cout << "Destructor called" << std::endl;
}

int Fixed::getRawBits( void ) const
{
	return (this->_rawBits);
}

void Fixed::setRawBits( int const raw )
{
	this->_rawBits = raw;
	return;
}
// Codes de l'exo01

Fixed::Fixed(int const n)
{
	std::cout << "Int constructor called" << std::endl;
	this->_rawBits = n << _fractionalBits; // convertit n INT en virgule fixe en decalant de 8 bits vers la gauche
}
// ----- Convertir 
Fixed::Fixed(float const n)
{
	std::cout << "Float constructor called" << std::endl;
	this->_rawBits = roundf ( n * ( 1 << _fractionalBits)); // convertir n FLOAT en multipliant par 256 et en arrondissant
}

float Fixed::toFloat( void ) const
{
	return ((float)this->_rawBits / (1 << _fractionalBits)); // 1 << _fractionnalBits = 256
}

int Fixed::toInt( void ) const
{
	return (this->_rawBits >> _fractionalBits); // equivaut a resultat / 256
}

std::ostream & operator<<(std::ostream &o, Fixed const & other)
{
	o << other.toFloat(); // met le resultat dans le flux
	return (o); // retourne le flux
}
