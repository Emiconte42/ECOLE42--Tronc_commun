#include "../includes/Fixed.hpp"
#include "../includes/Point.hpp"

float Point::sign (Point p, Point a, Point b)
{
	Fixed	fixed;
	
	fixed = (p._x - b._x) * (a._y - b._y) - (a._x - b._x) * (p._y - b._y);
	return(fixed.toFloat());
}

bool Point::PointInTriangle (Point p, Point a, Point b, Point c)
{
	float	d1, d2, d3;
	
	d1 = sign(p, a, b);
	d2 = sign(p, b, c);
	d3 = sign(p, c, a);
	
	return ((d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0));
}

bool bsp(Point const a, Point const b, Point const c, Point const point)
{
	Point checker;
	return (checker.PointInTriangle(point, a, b, c));
}