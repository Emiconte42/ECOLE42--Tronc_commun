#include <iostream>

int	main()
{
	// ==================================================
	// PARTIE 1 : LA REFERENCE (un surnom)
	// ==================================================
	int	pommes = 5;
	int	poires = 10;

	int	&surnom = pommes;	// "surnom" est un 2e nom pour "pommes"

	std::cout << "=== REFERENCE ===" << std::endl;
	std::cout << "1. Depart : pommes = " << pommes
		<< ", poires = " << poires
		<< ", surnom = " << surnom << std::endl;
	// Attendu : pommes = 5, poires = 10, surnom = 5

	surnom = 99;	// Je change le surnom = je change "pommes"
	std::cout << "2. Apres surnom = 99 : pommes = " << pommes << std::endl;
	// Attendu : pommes = 99

	surnom = poires;	// ATTENTION : ca veut dire "pommes = poires"
						// Le surnom NE colle PAS a poires !
	std::cout << "3. Apres surnom = poires : pommes = " << pommes << std::endl;
	// Attendu : pommes = 10

	poires = 20;	// Je change "poires"
	std::cout << "4. Apres poires = 20 : pommes = " << pommes
		<< ", surnom = " << surnom << std::endl;
	// Attendu : pommes = 10, surnom = 10 (rien n'a bouge : surnom n'est pas lie a poires)

	return (0);
}