#include <string>
#include <iostream>

void	byPtr(std::string* str)
{
	*str += "et les poneys";
}

void byConstPtr(std::string const * str)
{
	std::cout << *str << std::endl;
}

void	byRef(std::string& str)
{
	str += "et les vaches";
}

void	byConstRef(std::string const & str)
{
	std::cout << str << std::endl;
}

int main()
{
	std::string str = "j'aime les papillons ";

	std::cout << str << std::endl;
	byPtr(&str);
	byConstPtr(&str);

	str = "j'aime aussi les oiseaux ";

	std::cout << str << std::endl;
	byRef(str);
	byConstRef(str);

	return(0);
}