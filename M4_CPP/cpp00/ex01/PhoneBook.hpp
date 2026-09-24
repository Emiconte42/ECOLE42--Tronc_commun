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
		int getLimit() const;
		void displayContactList(int limit) const;
		void searchByIndex(int limit) const;

	public:
		PhoneBook();
		void addContact();
		void searchContact();
};

#endif