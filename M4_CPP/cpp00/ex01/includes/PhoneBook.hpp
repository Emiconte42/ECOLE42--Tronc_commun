#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include "Contact.hpp"

class PhoneBook
{
	private:
		Contact _contacts[8];
		int _count;
		bool askAddContact(const std::string &prompt, std::string &result);

	public:
		PhoneBook();
		void addContact();
		void searchContact();
};

#endif