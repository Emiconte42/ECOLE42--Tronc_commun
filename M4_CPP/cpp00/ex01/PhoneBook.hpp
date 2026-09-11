#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include <iostream>
#include <string>

class PhoneBook
{
	private:
		Contact _contacts[8];
		int _count;

	public: // ici les methodes : ajouter, chercher ect... ADD, SEARCH ... voir sujet
		void addContact();
		void searchContact();
};

#endif