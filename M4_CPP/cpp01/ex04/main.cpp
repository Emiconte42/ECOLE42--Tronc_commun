#include "sed.hpp"

int main(int argc, char **argv)
{
	if (argc != 4) // on check le nombre d'argument
	{
		std::cerr << "Usage: ./sed <file> <to_replace> <replace_by>" << std::endl;
		return (ERROR);
	}

	std::ifstream inFile(argv[1]); // on ouvre le fichier .txt existant
	if (checkInputFile(inFile) == ERROR)
		return (ERROR);

	std::string	s1 = argv[2]; // mot a chercher
	std::string	s2 = argv[3]; // mot a remplacer
	if (checkSearchWord(s1) == ERROR)
		return (ERROR);

	std::string	filename	= argv[1]; // on cree une variable depuis le fichier existant
	std::string	outFilename = filename + ".replace"; // on lui ajoute .replace
	std::ofstream outFile(outFilename.c_str()); // on creer le fichier <file>.replace

	if (checkOutputFile(outFile) == ERROR)
		return (ERROR);

	std::string	content = readFile(inFile);
	outFile << replaceAll(content, s1, s2);

	return (SUCCESS);
}