#ifndef CONTACT_HPP
#define CONTACT_HPP

#include <iostream>
#include <string>

class Contact {
	private:
		std::string	_firstname;
		std::string	_lastname;
		std::string	_nickname;
		std::string	_phonenumber;
		std::string	_darkestsecret;
	public:

		// Constructeur par defaut
		Contact();

		//  Constructeur avec 5 paramètres (vu avec Logan)
		Contact(
			const std::string& firstname,
			const std::string& lastname,
			const std::string& nickname,
			const std::string& phonenumber,
			const std::string& darkestsecret
		);

		// setters
		void	setFirstName(const std::string &firstname);
		void	setLastName(const std::string &lastname);
		void	setNickName(const std::string &nickname);
		void	setPhoneNumber(const std::string &phonenumber);
		void	setDarkestSecret(const std::string &darkestsecret);

		// getters
		std::string	getFirstName() const;
		std::string	getLastName() const;
		std::string	getNickName() const;
		std::string	getPhoneNumber() const;
		std::string	getDarkestSecret() const;
};

#endif