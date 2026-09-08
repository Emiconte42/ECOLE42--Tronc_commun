#include <iostream>
#include <string>

class Contact
{
	private:
		std::string	_first_name;
		std::string	_last_name;
		std::string	_nick_name;
		std::string	_phone_number;
		std::string	_darkest_secret;
	public:
		void	setFirstName(const std::string &firstname) {
			_first_name = firstname;
		}

		std::string	getFirstName() const {
			return _first_name;
		}

		void	setLastName(const std::string &lastname) {
			_last_name = lastname;
		}

		std::string	getLastName() const {
			return _last_name;
		}

		void	setNickName(const std::string &nickname) {
			_nick_name = nickname;
		}

		std::string	getNickName() const {
			return _nick_name;
		}

		void	setPhoneNumber(const std::string &phonenumber) {
			_phone_number = phonenumber;
		}

		std::string	getPhoneNumber() const {
			return _phone_number;
		}

		void	setDarkestSecret(const std::string &darkestsecret) {
			_darkest_secret = darkestsecret;
		}

		std::string	getDarkestSecret() const {
			return _darkest_secret;
		}
};