#include "Contact.hpp"

// Constructeur par défaut
Contact::Contact()
{
	_firstname			= "";
	_lastname			= "";
	_nickname			= "";
	_phonenumber		= "";
	_darkestsecret		= "";
}
// Constructeur avec 5 paramètres (vu avec Logan)
Contact::Contact(
	const std::string& firstname,
	const std::string& lastname,
	const std::string& nickname,
	const std::string& phonenumber,
	const std::string& darkestsecret
):
	_firstname(firstname),
	_lastname(lastname),
	_nickname(nickname),
	_phonenumber(phonenumber),
	_darkestsecret(darkestsecret)
{
}

void	Contact::setFirstName(const std::string& firstname) {
	_firstname = firstname;
}
void	Contact::setLastName(const std::string &lastname) {
	_lastname = lastname;
}
void	Contact::setNickName(const std::string &nickname) {
	_nickname = nickname;
}
void	Contact::setPhoneNumber(const std::string &phonenumber) {
	_phonenumber = phonenumber;
}
void	Contact::setDarkestSecret(const std::string &darkestsecret) {
	_darkestsecret = darkestsecret;
}


std::string	Contact::getFirstName() const {
	return _firstname;
}
std::string	Contact::getLastName() const {
	return _lastname;
}
std::string	Contact::getNickName() const {
	return _nickname;
}
std::string	Contact::getPhoneNumber() const {
	return _phonenumber;
}
std::string	Contact::getDarkestSecret() const {
	return _darkestsecret;
}