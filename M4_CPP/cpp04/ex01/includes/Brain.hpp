#ifndef BRAIN_HPP
#define BRAIN_HPP

#include <iostream>

class Brain
{
	private :
		std::string _ideas[100]; // tableau fixe, pas de pointeur : la copie se fait simplement

	public :
		Brain();
		Brain(Brain const &cpy);
		Brain & operator=(Brain const &other);
		~Brain();

		void setIdeas(unsigned int index, std::string const ideas);
		std::string getIdeas(unsigned int index) const;
};

#endif