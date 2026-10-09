#include "sed.hpp"

int main(int argc, char **argv)
{
	if (argc != 4) // on check le nombre d'argument
	{
		std::cerr << "Usage: ./sed <file> <to_replace> <replace_by>" << std::endl;
		return (ERROR);
	}

	std::ifstream inFile(argv[1]); // on ouvre le fichier .txt existant
	if(!inFile.is_open()) // on verifie qu'il existe
	{
		std::cerr << "Error: cannot open input file" << std::endl;
		return (ERROR);
	}

	std::string	filename	= argv[1]; // on cree une variable depuis le fichier existant
	std::string	outFilename = filename + ".replace"; // on lui ajoute .replace

	std::ofstream outFile(outFilename.c_str()); // on creer le fichier <file>.replace
	if(!outFile.is_open()) // on verifie que tout est ok
	{
		std::cerr << "Error: cannot create output file" << std::endl;
		return (ERROR);
	}

	std::string	content; // permet d'etre rempli via get() pour lire un fichier
	char c; // on avance char par char
	
	while (inFile.get(c)) // tant qu'il y a du texte dans le fichier on lit char par char
		content += c; // et on stock le resultat dans content

	std::string	s1 = argv[2]; // mot a chercher
	std::string	s2 = argv[3]; // mot a remplacer

	size_t pos = content.find(s1); //dans content chercher le mot s1
	size_t start = 0; // demarre a la position 0
	while (pos != std::string::npos) // tant que pos trouve quelque chose
	{
		std::cout << "pos :" << pos << std::endl; // on affiche pos
		start = pos + s1.size(); // on avance start avec pos + taille du mot
		pos = content.find(s1, start); // on continu de chercher a partir de start
	}


}