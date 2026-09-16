#include "PhoneBook.hpp"

PhoneBook::PhoneBook()
{
	_count = 0;
	// _contacts[0].._contacts[7] sont construits automatiquement
	// via Contact::Contact()
}

void PhoneBook::addContact(){
	std::string inputFirstName;
	std::cout << "Enter your first name : ";
	if(inputFirstName.empty()) {
		std::cout << "Enter your first name : ";
	}
	std::getline(std::cin, inputFirstName);
	for (size_t i = 0; i < inputFirstName.size(); i++)
	{
		if (inputFirstName[i] == ' ' || inputFirstName.empty())
			break;
	}
}
	// std::cout << inputFirstName << std::endl;

	// std::string inputLastName;
	// std::cout << "Enter your last name : ";
	// std::getline(std::cin, inputLastName);
	// std::cout << inputLastName << std::endl;

	// std::string inputNickName;
	// std::cout << "Enter your nick name : ";
	// std::getline(std::cin, inputNickName);
	// std::cout << inputNickName << std::endl;

	// std::string inputPhoneNumber;
	// std::cout << "Enter your phone number : ";
	// std::getline(std::cin, inputPhoneNumber);
	// std::cout << inputPhoneNumber << std::endl;

	// std::string inputDarkestSecret;
	// std::cout << "Enter your darkest secret : ";
	// std::getline(std::cin, inputDarkestSecret);
	// std::cout << inputDarkestSecret << std::endl;
