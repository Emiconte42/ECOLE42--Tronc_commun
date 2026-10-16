#ifndef FIXED_HPP
#define FIXED_HPP

#include <iostream>
#include <cmath> // necessaire pour roundf

class Fixed
{
	private:
		int	_rawBits;
		static const int	_fractionalBits = 8;

	public:
		// Constructeur et destructeur de l'ex00
		Fixed(); 
		Fixed(Fixed const & cpy);
		Fixed &operator=(Fixed const &other);
		~Fixed();
		
		// Constructeur de l'ex01
		Fixed(int const n);
		Fixed(float const n);

		//Getters et Setters de l'ex00
		int getRawBits( void ) const;
		void setRawBits( int const raw );

		// Nvelles fonctions de l'ex01
		float toFloat( void ) const;
		int toInt( void ) const;
	};
	
	// surcharge de l'operateur a mettre hors de la classe
	std::ostream & operator<<(std::ostream &o, Fixed const & other); // operateur d'assignation
#endif