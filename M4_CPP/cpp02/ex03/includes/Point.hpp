#ifndef POINT_HPP
#define POINT_HPP
#include "Fixed.hpp"

#include <iostream>

class Point
{
	private:
		Fixed const	_x;
		Fixed const	_y;

	public:
		Point(); 
		Point(float const a, float const b);
		Point(Point const & cpy);
		~Point();
		
		Point &operator=(Point const &other);

		Fixed getX( void ) const;
		Fixed getY( void ) const;
		float sign (Point p, Point a, Point b);
		bool PointInTriangle (Point p, Point a, Point b, Point c);
	};
	bool bsp(Point const a, Point const b, Point const c, Point const point);
	std::ostream & operator<<(std::ostream &o, Point const &point);

#endif