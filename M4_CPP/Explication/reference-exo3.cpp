#include <string>
#include <iostream>

class	Student
{
	private :
		std::string _login;
	public:
		Student(std::string const & login) : _login(login)
		{
		}

		std::string&	getLoginRef()
		{
			return this->_login;
		}

		std::string const & getLoginRefConst() const
		{
			return this->_login;
		}

		std::string*	getLoginPtr()
		{
			return &(this->_login);
		}

		std::string const * getLoginPtrConst() const
		{
			return &(this->_login);
		}

};

int main()
{
	Student			bob = Student("Bob");
	Student const	jim = Student("Jim");

	std::cout << bob.getLoginRefConst() << " " << *(bob.getLoginPtrConst()) << std::endl;
	std::cout << jim.getLoginRefConst() << " " << *(jim.getLoginPtrConst()) << std::endl;

	bob.getLoginRef() = "Bobby reference";
	std::cout << bob.getLoginRefConst() << std::endl;

	*(bob.getLoginPtr()) = "Bobby pointeur";
	std::cout << bob.getLoginRefConst() << std::endl;

	return(0);
}