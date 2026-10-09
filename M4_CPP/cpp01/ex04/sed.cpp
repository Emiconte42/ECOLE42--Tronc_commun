#include "sed.hpp"

int	checkInputFile(std::ifstream &file)
{
	if(!file.is_open()) // on verifie qu'il existe
	{
		std::cerr << "Error: cannot open input file" << std::endl;
		return (ERROR);
	}
	return (SUCCESS);
}

int	checkSearchWord(const std::string &s1)
{
	if (s1.empty())
	{
		std::cerr << "Error: argv[2] cannot be empty" << std::endl;
		return (ERROR);
	}
	return (SUCCESS);
}

int	checkOutputFile(std::ofstream &file)
{
	if(!file.is_open())
	{
		std::cerr << "Error: cannot create output file" << std::endl;
		return (ERROR);
	}
	return (SUCCESS);
}

std::string	readFile(std::ifstream &file)
{
	std::string	content; // permet d'etre rempli via get() pour lire un fichier
	char c; // on avance char par char
	while (file.get(c)) // tant qu'il y a du texte dans le fichier on lit char par char
		content += c; // et on stock le resultat dans content
	return (content);
}

std::string replaceAll(const std::string &content, std::string s1, std::string s2)
{
	std::string	result;
	size_t position = content.find(s1); //dans content chercher le mot s1
	size_t start = 0; // demarre a la position 0
	while (position != std::string::npos) // tant que pos trouve quelque chose
	{
		std::string	before = content.substr(start, position - start); // tout ce qui est avant ou entre les mots a chercher
		result += before;
		result += s2;
		// std::cout << "before :" << before << std::endl; 
		start = position + s1.size(); // on avance start avec pos + taille du mot
		position = content.find(s1, start); // on continu de chercher a partir de start
	}
	result += content.substr(start);
	return (result);
}