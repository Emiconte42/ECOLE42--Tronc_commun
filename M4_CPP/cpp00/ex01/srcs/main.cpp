#include <iostream>
#include "../includes/Contact.hpp"
#include "../includes/PhoneBook.hpp"

int main()
{
	PhoneBook phoneBook;
	std::string input;
	
	while (input != "EXIT")
	{
		std::cout << "Available commands : ADD, SEARCH or EXIT : " << std::endl;
		if (!std::getline(std::cin, input))
			break;
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