#include "PhoneBook.hpp"

PhoneBook::PhoneBook()
{
	_count = 0;
}

bool PhoneBook::askAddContact(const std::string &prompt, std::string &result)
{
	bool hasRealChar = false;

	while (hasRealChar == false)
	{
		std::cout << prompt;
		if (!std::getline(std::cin, result))
		{
			std::cout << std::endl;
			return false;
		}
		for (size_t i = 0; i < result.size(); i++)
		{
			if (result[i] != ' ' && result[i] != '\t')
			{
				hasRealChar = true;
				break;
			}
		}
	}
	return true;
}

void PhoneBook::addContact()
{
	std::string inputFirstName;
	std::string inputLastName;
	std::string inputNickName;
	std::string inputPhoneNumber;
	std::string inputDarkestSecret;
	Contact newContact;

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

	newContact.setFirstName(inputFirstName);
	newContact.setLastName(inputLastName);
	newContact.setNickName(inputNickName);
	newContact.setPhoneNumber(inputPhoneNumber);
	newContact.setDarkestSecret(inputDarkestSecret);

	_contacts[_count % 8] = newContact;
	_count++;
}

static std::string truncate(const std::string &text)
{
	if (text.size() > 10)
		return text.substr(0, 9) + ".";
	return text;
}

int PhoneBook::getLimit() const
{
	int limit = _count;
	if (limit > 8)
		limit = 8;
	return limit;
}

void PhoneBook::displayContactList(int limit) const
{
	std::cout << "|" << std::setw(10) << "Index" << "|";
	std::cout << std::setw(10) << "First Name" << "|";
	std::cout << std::setw(10) << "Last Name" << "|";
	std::cout << std::setw(10) << "Nick Name" << "|" << std::endl;

	std::cout << "|" << std::string(10, '*') << "|";
	std::cout << std::string(10, '*') << "|";
	std::cout << std::string(10, '*') << "|";
	std::cout << std::string(10, '*') << "|" << std::endl;

	for (int i = 0; i < limit; i++)
	{
		std::cout << "|" << std::setw(10) << i << "|";

		std::cout << std::setw(10) << truncate(_contacts[i].getFirstName()) << "|";
		std::cout << std::setw(10) << truncate(_contacts[i].getLastName()) << "|";
		std::cout << std::setw(10) << truncate(_contacts[i].getNickName()) << "|" << std::endl;
	}	
}

void PhoneBook::searchByIndex(int limit) const
{
	int	index;

	while (true)
	{
		std::cout << "Enter an index : ";

		if (std::cin.eof())
				return;
		if (std::cin.peek() == '\n')
		{
			std::cin.get();
			std::cout << "Empty index." << std::endl;
			continue;
		}
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
			std::cin.ignore(10000, '\n');
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

void PhoneBook::searchContact()
{
	int limit = getLimit();
	if (limit == 0)
	{
		std::cout << "PhoneBook is empty" << std::endl;
		return ;
	}
	displayContactList(limit);
	searchByIndex(limit);
}
