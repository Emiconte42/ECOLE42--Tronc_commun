#include "includes/Fixed.hpp"
#include "includes/Point.hpp"

int main( void )
{
	Point	a(0, 0);
	Point	b(40, 10);
	Point	c(5, 60);
	Point	p1(0, 0);
	Point	p2(4.1, 3);
	Point	p3(0, 10);
	Point	p4(3, 3);
	Point	p5(5.5, 5.5);
	Point	p6(5, 15);
	const Point	points[] = {p1, p2, p3, p4, p5, p6};

	std::cout << "TRIANGLE" << std::endl;
	std::cout << "Sommet a = " << a << std::endl;
	std::cout << "Sommet b = " << b << std::endl;
	std::cout << "Sommet c = " << c << std::endl;
	std::cout << std::endl;

	std::cout << "Chaque point est-il dans le triangle ?" << std::endl;
	for (int i = 0; i < 6; i++)
	{
		std::cout << "point p" << i + 1 << " " << points[i] << " :\t\t";
		if (bsp(a, b, c, points[i]) == 1)
			std::cout << "Oui" << std::endl;
		else
			std::cout << "Non" << std::endl;
	}

	return 0;
}