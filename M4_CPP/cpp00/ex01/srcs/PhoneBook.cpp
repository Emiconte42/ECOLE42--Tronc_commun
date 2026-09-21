#include "../includes/PhoneBook.hpp"

// Phonebook : initialise le carnet de contacts vide, count permet le calcul du nombre total de contacts
PhoneBook::PhoneBook()
{
	_count = 0; 
}

// Demande une saisie non vide et la stocke dans result. Renvoie false si la lecture échoue.
// Utilise dans addContact()
bool PhoneBook::askAddContact(const std::string &prompt, std::string &result)
{
	bool hasRealChar = false;

	while (hasRealChar == false)
	{
		std::cout << prompt;
		if (!std::getline(std::cin, result))
			return false;
		for (size_t i = 0; i < result.size(); i++)
		{
			if (result[i] != ' ')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	return true;
}

// Demande les 5 champs d'un nouveau contact, puis l'ajoute au carnet (ecrase le plus ancien apres 8 contacts).
void PhoneBook::addContact()
{
	std::string inputFirstName;
	std::string inputLastName;
	std::string inputNickName;
	std::string inputPhoneNumber;
	std::string inputDarkestSecret;

	if (!askAddContact("Enter your first name : ", inputFirstName))
		return;
	if (!askAddContact("Enter your last name : ", inputLastName))
		return;
	if (!askAddContact("Enter your nick name : ", inputNickName))
		return;
	if (!askAddContact("Enter your phone number : ", inputPhoneNumber))
		return;
	if (!askAddContact("Enter your dark secret : ", inputDarkestSecret))
		return;

	_contacts[_count % 8] = Contact(inputFirstName, inputLastName, inputNickName, inputPhoneNumber, inputDarkestSecret);
	_count++;
}


void PhoneBook::searchContact()
{
	// On calcul la limite, cela permets de determiner combien de contacts sont reellement dispo
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

	// Affichage des contacts dans le tableau avec la limite de 9 caracteres + '.'
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

	// Affichage d'un contact selon l'index choisi
	int	index;

	while (true)
	{
		std::cout << "Enter an index : ";
		if (std::cin.eof())
				return;
		if (!(std::cin >> index))
		{
			if (std::cin.eof())
				return;
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			std::cout << "Invalid index." << std::endl;
			continue ;
		}

		if (index < 0 || index >= limit)
		{
			std::cout << "Invalid index." << std::endl;
			continue ;
		}
		std::cin.ignore(10000, '\n');
		break;
	}

	std::cout << "First name : " << _contacts[index].getFirstName() << std::endl ;
	std::cout << "Last Name : " << _contacts[index].getLastName() << std::endl;
	std::cout << "Nick Name : " << _contacts[index].getNickName() << std::endl;
	std::cout << "Phone Number : " << _contacts[index].getPhoneNumber() << std::endl;
	std::cout << "Darkest Secret : " << _contacts[index].getDarkestSecret() << std::endl;
}