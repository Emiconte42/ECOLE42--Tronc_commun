#include "includes/Fixed.hpp"
#include "includes/Point.hpp"

int main( void )
{
	Point	a(0, 0);
	Point	b(10, 0);
	Point	c(0, 10);
	Point	p1(0, 0);
	Point	p2(4.1, 3);
	Point	p3(0, 10);
	Point	p4(3, 3);
	Point	p5(5.5, 5.5);
	Point	p6(5, 5);

	std::cout << "TRIANGLE" << std::endl;
	std::cout << "Sommet a = " << a << std::endl;
	std::cout << "Sommet b = " << b << std::endl;
	std::cout << "Sommet c = " << c << std::endl;
	std::cout << std::endl;

	std::cout << "point p1 = " << p1 << std::endl;
	std::cout << "p1 est il dans le triangle ?\t\t" << bsp(a,b,c,p1) << std::endl;

	std::cout << "point p2 = " << p2 << std::endl;
	std::cout << "p2 est il dans le triangle ?\t\t" << bsp(a,b,c,p2) << std::endl;

	std::cout << "point p3 = " << p3 << std::endl;
	std::cout << "p3 est il dans le triangle ?\t\t" << bsp(a,b,c,p3) << std::endl;

	std::cout << "point p4 = " << p4 << std::endl;
	std::cout << "p4 est il dans le triangle ?\t\t" << bsp(a,b,c,p4) << std::endl;

	std::cout << "point p5 = " << p5 << std::endl;
	std::cout << "p5 est il dans le triangle ?\t\t" << bsp(a,b,c,p5) << std::endl;

	std::cout << "point p6 = " << p6 << std::endl;
	std::cout << "p6 est il dans le triangle ?\t\t" << bsp(a,b,c,p6) << std::endl;

	return 0;
}