#ifndef FIXED_HPP
#define FIXED_HPP

#include <iostream>
#include <cmath>

class Fixed
{
	private:
		int	_rawBits;
		static const int	_fractionalBits = 8;

	public:
		Fixed(); 
		Fixed(Fixed const & cpy);
		Fixed(int const n);
		Fixed(float const n);
		~Fixed();
		
		Fixed &operator=(Fixed const &other);

		int getRawBits( void ) const;
		void setRawBits( int const raw );
		float toFloat( void ) const;
		int toInt( void ) const;

		// comparaison
		bool	operator >(Fixed const &other) const;
		bool	operator <(Fixed const &other) const;
		bool	operator >=(Fixed const &other) const;
		bool	operator <=(Fixed const &other) const;
		bool	operator ==(Fixed const &other) const;
		bool	operator !=(Fixed const &other) const;

		// arithmetique
		Fixed operator+(Fixed const &other) const;
		Fixed operator-(Fixed const &other) const;
		Fixed operator*(Fixed const &other) const;
		Fixed operator/(Fixed const &other) const;
		
		// incrementation / decrementation
		Fixed &operator++(void);
		Fixed operator++(int );
		Fixed &operator--(void);
		Fixed operator--(int);

		// MIN et MAX - static : la fonction appartient a la classe, pas a un objet. on l'appelle sans objet
		static Fixed &min(Fixed &a, Fixed &b); // a et b sont modifiables
		static const Fixed &min(Fixed const &a, Fixed const &b); // a et b ne sont pas modifiables
		static Fixed &max(Fixed &a, Fixed &b);
		static const Fixed &max(Fixed const &a, Fixed const &b);
	};
	
	// surcharge de l'operateur
	std::ostream & operator<<(std::ostream &o, Fixed const & other);
#endif