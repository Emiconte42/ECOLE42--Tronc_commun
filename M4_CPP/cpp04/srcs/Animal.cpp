#include "../includes/Animal.hpp"
#include "../utils/utils.hpp"

Animal::Animal()
{
	
}                                                                              
Animal(std::string const type)
Animal(std::string const &cpy)
Animal & operator=(Animal const &other)
~Animal()
std::string getType() const
void makeSound()