#include <iostream>

int	main()
{
	// ==================================================
	// PARTIE 2 : LE POINTEUR (une fleche)
	// ==================================================
	int	chats = 5;
	int	chiens = 10;

	int	*fleche = &chats;	// "fleche" montre "chats"

	std::cout << std::endl << "=== POINTEUR ===" << std::endl;
	std::cout << "1. Depart : chats = " << chats
		<< ", chiens = " << chiens
		<< ", *fleche = " << *fleche << std::endl;
	// Attendu : chats = 5, chiens = 10, *fleche = 5

	*fleche = 99;	// Avec l'etoile : je change ce que la fleche montre
	std::cout << "2. Apres *fleche = 99 : chats = " << chats << std::endl;
	// Attendu : chats = 99

	fleche = &chiens;	// Sans l'etoile : je change la CIBLE de la fleche
	std::cout << "3. Apres fleche = &chiens : chats = " << chats
		<< ", *fleche = " << *fleche << std::endl;
	// Attendu : chats = 99 (pas touche), *fleche = 10

	*fleche = 50;	// La fleche montre chiens maintenant
	std::cout << "4. Apres *fleche = 50 : chats = " << chats
		<< ", chiens = " << chiens << std::endl;
	// Attendu : chats = 99 (inchange), chiens = 50

	return (0);
}