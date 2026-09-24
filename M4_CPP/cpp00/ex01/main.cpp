#include <iostream>
#include "Contact.hpp"
#include "PhoneBook.hpp"

int main()
{
	PhoneBook phoneBook;
	std::string input;
	
	while (std::cin.good() && input != "EXIT")
	{
		std::cout << "Available commands : ADD, SEARCH or EXIT : ";
		if (!std::getline(std::cin, input))
		{
			std::cout << std::endl;
			break;
		}
		if (input == "ADD")
		{
			phoneBook.addContact();
		}
		else if(input == "SEARCH")
		{
			phoneBook.searchContact();
		}
	}
	return 0;
}