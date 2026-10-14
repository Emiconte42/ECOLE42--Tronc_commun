#include "Fixed.hpp"

Fixed::Fixed()
{
	std::cout << "Default constructor called" << std::endl;
	this->_rawBits = 0;
	return;
}
Fixed::Fixed(int const n)
{
	std::cout << "Int constructor called" << std::endl;
	_rawBits = n << _fractionalBits;
}

Fixed::Fixed(float const n)
{
	std::cout << "Float constructor called" << std::endl;
	_rawBits = roundf( n * (1 << _fractionalBits));
}

Fixed::Fixed (Fixed const & cpy)
{
	std::cout << "Copy constructor called" << std::endl;
	this->_rawBits = cpy.getRawBits();
}

Fixed & Fixed::operator=(Fixed const &other)
{
	std::cout << "Copy assignment operator called" << std::endl;
	this->_rawBits = other.getRawBits();
	return (*this);
}

std::ostream & operator<<(std::ostream & out, Fixed const & other)
{
	out << other.toFloat();
	return (out);
}


int Fixed::getRawBits( void ) const
{
	// std::cout << "getRawBits member function called" << std::endl;
	return (this->_rawBits);
}

void Fixed::setRawBits( int const raw )
{
	this->_rawBits = raw;
	return;
}

float Fixed::toFloat( void ) const
{
	int a;
	a = _rawBits / _fractionalBits;
	return(a);
}

int Fixed::toInt( void ) const
{
	return (_rawBits);
}

Fixed::~Fixed()
{
	std::cout << "Destructor called" << std::endl;
}