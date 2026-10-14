#ifndef FIXED_HPP
#define FIXED_HPP

#include <iostream>
#include <string>

class Fixed
{
	private:
		int _rawBits; // Un int pour stocker la valeur.
		static const int	_fractionalBits = 8; // Un static constant int pour stocker le nombre de fractional bits. Sa valeur sera toujours 8.

	public:
		Fixed(); // constructeur par defaut
		Fixed (Fixed const & cpy); // constructeur par copie
		~Fixed(); // destructeur
	
		Fixed & operator=(Fixed const &other); // operateur d'assignation
		int getRawBits( void ) const;
		void setRawBits( int const raw );
};

#endif