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
		Fixed(int const n); // construit _rawBits en n * 256
		Fixed(float const n); // construit _rawBits en n * 256 + arrondi

		//Getters et Setters de l'ex00
		int getRawBits( void ) const;
		void setRawBits( int const raw );

		// Nvelles fonctions de l'ex01
		float toFloat( void ) const; // retourne le nombre avec ses decimales en / 256
		int toInt( void ) const; // retourne seulement la partie entiere en / 256
	};
	
	// surcharge de l'operateur a mettre hors de la classe
	std::ostream & operator<<(std::ostream &o, Fixed const & other);
#endif