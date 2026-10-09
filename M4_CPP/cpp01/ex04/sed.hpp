#ifndef SED_HPP
#define SED_HPP

#include <iostream>
#include <string>
#include <fstream> // outil qui sert à ouvrir et lire un fichier

enum validate
{
	SUCCESS = 0,
	ERROR = 1,
};

int	checkInputFile(std::ifstream &file);
int	checkSearchWord(const std::string &s1);
int	checkOutputFile(std::ofstream &file);
std::string	readFile(std::ifstream &file);
std::string replaceAll(const std::string &content, std::string s1, std::string s2);

#endif