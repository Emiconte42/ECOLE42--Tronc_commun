#ifndef FIXED_HPP
#define FIXED_HPP

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

class Fixed
{
	private:
		int	_rawBits; // Un int pour stocker.
		static const int	_fractionalBits = 8;

	public:
		Fixed(); // constructeur par defaut
		Fixed(int const n); // Constructeur qui prend un const int en parametre
		Fixed(float const n); // Constructeur qui prend un const float en parametre
		Fixed(Fixed const & cpy); // constructeur par copie
		~Fixed(); // destructeur

		Fixed & operator=(Fixed const &other);
		int getRawBits( void ) const;
		void setRawBits( int const raw );
		float toFloat( void ) const;
		int toInt( void ) const;
	};
	
	std::ostream & operator<<(std::ostream & out, Fixed const & other); // operateur d'assignation
#endif