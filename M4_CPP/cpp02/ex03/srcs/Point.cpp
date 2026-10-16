#include "../includes/Point.hpp"

Point::Point() : _x(0), _y(0) {}

Point::Point(float const x, float const y) : _x(x), _y(y) {}

Point::Point (Point const & cpy) : _x(cpy._x), _y(cpy._y) {
}

Point::~Point() {}

Point &Point::operator=(Point const &other) {
	(void)other;
	return (*this);
}

// ----- SETTERS / GETTERS / AUTRES FONCTIONS
Fixed Point::getX( void ) const {
	return (_x); // recupere les coordonnes du point
}

Fixed Point::getY( void ) const {
	return (_y); // recupere les coordonnes du point
}

std::ostream & operator<<(std::ostream &o, Point const &point) {
	o << "(" << point.getX() << ", " << point.getY() << ")";
	return (o);
}