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

	public: // ici les methodes : ajouter, chercher ect... ADD, SEARCH ... voir sujet
		PhoneBook();
		void addContact();
		void searchContact();
};

#endif