#include "../includes/Fixed.hpp"

Fixed::Fixed() : _rawBits(0) {}

Fixed::Fixed (Fixed const & cpy) {
	*this = cpy;
}

Fixed::Fixed(int const n) {
	this->_rawBits = n << _fractionalBits;
}
// ----- Convertir 
Fixed::Fixed(float const n) {
	this->_rawBits = roundf ( n * ( 1 << _fractionalBits));
}

Fixed::~Fixed() {}

Fixed &Fixed::operator=(Fixed const &other) {
	if (this != &other)
		this->_rawBits = other.getRawBits();
	return (*this);
}

// ----- SETTERS / GETTERS / AUTRES FONCTIONS
int Fixed::getRawBits( void ) const {
	return (this->_rawBits);
}

void Fixed::setRawBits( int const raw ) {
	this->_rawBits = raw;
	return;
}

float Fixed::toFloat( void ) const {
	return ((float)this->_rawBits / (1 << _fractionalBits));
}

int Fixed::toInt( void ) const {
	return (this->_rawBits >> _fractionalBits);
}

std::ostream & operator<<(std::ostream &o, Fixed const & other) {
	o << other.toFloat();
	return (o);
}

// ----- COMPARAISON -----
bool	Fixed::operator>(Fixed const &other) const{
	return (this->_rawBits > other._rawBits);
}

bool	Fixed::operator<(Fixed const &other) const{
	return (this->_rawBits < other._rawBits);
}

bool	Fixed::operator>=(Fixed const &other) const{
	return (this->_rawBits >= other._rawBits);
}

bool	Fixed::operator<=(Fixed const &other) const{
	return (this->_rawBits <= other._rawBits);
}

bool	Fixed::operator==(Fixed const &other) const{
	return (this->_rawBits== other._rawBits);
}

bool	Fixed::operator!=(Fixed const &other) const{
	return (this->_rawBits != other._rawBits);
}

// ----- ARITHMETIQUE -----
Fixed Fixed::operator+(Fixed const &other) const {
	Fixed	result;

	result.setRawBits(this->_rawBits + other._rawBits);
	return (result);
}

Fixed Fixed::operator-(Fixed const &other) const {
	Fixed	result;

	result.setRawBits(this->_rawBits - other._rawBits);
	return (result);
}

Fixed Fixed::operator*(Fixed const &other) const {
	Fixed	result;

	result.setRawBits((this->_rawBits * other._rawBits) >> _fractionalBits);
	return (result);
}

Fixed Fixed::operator/(Fixed const &other) const {
	Fixed	result;

	result.setRawBits((this->_rawBits << _fractionalBits) / other._rawBits);
	return (result);
}


// ----- INCREMENTATION / DECREMENTATION -----
Fixed &Fixed::operator++(void) {
	this->_rawBits++;
	return (*this);
}

Fixed Fixed::operator++(int) {
	Fixed tmp(*this);
	
	++_rawBits;
	return (tmp);
}

Fixed &Fixed::operator--(void) {
	this->_rawBits--;
	return (*this);
}

Fixed Fixed::operator--(int) {
	Fixed tmp(*this);

	--_rawBits;
	return (tmp);
}

// INT MIN et INT MAX
Fixed &Fixed::min(Fixed &a, Fixed &b) {
	if (a < b)
		return a;
	return (b);
}

const Fixed &Fixed::min(Fixed const &a, Fixed const &b){
	if (a < b)
		return a;
	return (b);
}

Fixed &Fixed::max(Fixed &a, Fixed &b){
	if (a > b)
		return a;
	return (b);
}

const Fixed &Fixed::max(Fixed const &a, Fixed const &b){
	if (a > b)
		return a;
	return (b);
}