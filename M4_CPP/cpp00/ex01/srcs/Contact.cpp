#include "../includes/Contact.hpp"

// Constructeur par défaut, initialise un contact vide lorsque le carnet est cree.
Contact::Contact()
{
	_firstname			= "";
	_lastname			= "";
	_nickname			= "";
	_phonenumber		= "";
	_darkestsecret		= "";
}
// Constructeur avec 5 paramètres, initialise un contact avec les cinq informations fournies
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
// setters fonction publique qui permet de modifier un attribut prive
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

// getters fonction publique qui permetde lire uniquement un attribut prive
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