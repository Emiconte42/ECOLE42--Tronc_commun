#include "../includes/PhoneBook.hpp"

PhoneBook::PhoneBook()
{
	_count = 0; // _contacts[0].._contacts[7] sont construits automatiquement via Contact::Contact()
}

void PhoneBook::addContact()
{
	bool	hasRealChar = false;
	
	// Enter the FirstName, with flag false at start, if search and find the char the flag is true
	std::string inputFirstName;
	while (hasRealChar == false)
	{
		std::cout << "Enter your first name : ";
		if (!std::getline(std::cin, inputFirstName))
			return;
		for (size_t i = 0; i < inputFirstName.size(); i++)
		{
			if (inputFirstName[i] != ' ')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	std::cout << inputFirstName << std::endl; // A SUPPRIMER A L'ISSUE DU PROJET
	hasRealChar = false;

	// Enter the Last Name
	std::string inputLastName;
	while (hasRealChar == false)
	{
		std::cout << "Enter your last name : ";
		if (!std::getline(std::cin, inputLastName))
			return;
		for (size_t i = 0; i < inputLastName.size(); i++)
		{
			if (inputLastName[i] != ' ')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	std::cout << inputLastName << std::endl; // A SUPPRIMER A L'ISSUE DU PROJET
	hasRealChar = false;

	// Enter the Nick Name
	std::string inputNickName;
	while (hasRealChar == false)
	{
		std::cout << "Enter your nick name : ";
		if (!std::getline(std::cin, inputNickName))
			return;
		for (size_t i = 0; i < inputNickName.size(); i++)
		{
			if (inputNickName[i] != ' ')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	std::cout << inputNickName << std::endl; // A SUPPRIMER A L'ISSUE DU PROJET
	hasRealChar = false;

	// Enter the Phone Number
	std::string inputPhoneNumber;
	while (hasRealChar == false)
	{
		std::cout << "Enter your phone number : ";
		if (!std::getline(std::cin, inputPhoneNumber))
			return;
		for (size_t i = 0; i < inputPhoneNumber.size(); i++)
		{
			if (inputPhoneNumber[i] != ' ')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	std::cout << inputPhoneNumber << std::endl; // A SUPPRIMER A L'ISSUE DU PROJET
	hasRealChar = false;

	// Enter the Darkest Secret
	std::string inputDarkestSecret;
	while (hasRealChar == false)
	{
		std::cout << "Enter your dark secret : ";
		if (!std::getline(std::cin, inputDarkestSecret))
			return;
		for (size_t i = 0; i < inputDarkestSecret.size(); i++)
		{
			if (inputDarkestSecret[i] != ' ')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	std::cout << inputDarkestSecret << std::endl; // A SUPPRIMER A L'ISSUE DU PROJET
	hasRealChar = false;

	_contacts[_count % 8] = Contact(inputFirstName, inputLastName, inputNickName, inputPhoneNumber, inputDarkestSecret);
	_count++;
}

void PhoneBook::searchContact()
{
	// Calcul de la limite
	int limit;

	if (_count < 8)
		limit = _count;
	else
		limit = 8;
		
	if (limit == 0)
	{
		std::cout << "PhoneBook is empty" << std::endl;
		return ;
	}
	// Affichage du header du tableau
	std::cout << "|" << std::setw(10) << "Index" << "|";
	std::cout << std::setw(10) << "First Name" << "|";
	std::cout << std::setw(10) << "Last Name" << "|";
	std::cout << std::setw(10) << "Nick Name" << "|" << std::endl;

	// Affichage de la separation du tableau
	std::cout << "|" << std::string(10, '*') << "|";
	std::cout << std::string(10, '*') << "|";
	std::cout << std::string(10, '*') << "|";
	std::cout << std::string(10, '*') << "|" << std::endl;

	// Affichage des contacts
	for (int i = 0; i < limit; i++)
	{
		std::cout << "|" << std::setw(10) << i << "|";

		std::string firstname = _contacts[i].getFirstName();
		if (firstname.size() > 10)
			firstname = firstname.substr(0, 9) + ".";
		std::cout << std::setw(10) << firstname << "|";

		std::string lastname = _contacts[i].getLastName();
		if (lastname.size() > 10)
			lastname = lastname.substr(0, 9) + ".";
		std::cout << std::setw(10) << lastname << "|";

		std::string nickname = _contacts[i].getNickName();
		if (nickname.size() > 10)
			nickname = nickname.substr(0, 9) + ".";
		std::cout << std::setw(10) << nickname << "|" << std::endl;
	}	

	// Affichage d'un contact selon l'index
	std::cout << "Enter an index :" << std::endl ;
	std::string	index;
	std::cin >> index;
	if (index.empty() || index.size() > 1 || index[0] < '0' || index[0] > '8')
	{
		std::cout << "Invalid index, " ;
		std::cout << "enter an index :" << std::endl ;
		std::cin >> index;
	}
}