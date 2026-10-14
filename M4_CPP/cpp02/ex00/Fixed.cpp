#include "Fixed.hpp"

Fixed::Fixed()
{
	std::cout << "Default constructor called" << std::endl;
	this->_rawBits = 0;
	return;
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

Fixed::~Fixed()
{
	std::cout << "Destructor called" << std::endl;

}

int Fixed::getRawBits( void ) const
{
	std::cout << "getRawBits member function called" << std::endl;
	return (this->_rawBits);
}

void Fixed::setRawBits( int const raw )
{
	this->_rawBits = raw;
	return;
}
