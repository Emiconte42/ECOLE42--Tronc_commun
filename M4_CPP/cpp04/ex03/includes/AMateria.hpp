#ifndef AMATERIA_HPP
#define AMATERIA_HPP

#include <iostream>
#include <string>

class AMateria
{
	protected :
		// std::string type;

	public :
		AMateria();
		AMateria(std::string const & type);
		AMateria(AMateria const &cpy);
		AMateria & operator=(AMateria const &other);
		virtual ~AMateria();

		std::string const & getType() const;

		virtual AMateria* clone() const = 0;
		virtual void use(ICharacter& target);
};

#endif