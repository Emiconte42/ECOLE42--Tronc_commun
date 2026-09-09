#include <iostream>
#include "Contact.hpp"

int main()
{
	Contact c1;  // on crée un objet de type Contact, nommé c comme un int x;
	Contact c2;
	c1.setFirstName("Alice "); // appelle la méthode setFirstName sur c;
	c1.setLastName("Dupont ");
	c1.setNickName("Aliiii ");
	c1.setPhoneNumber("0503020100 ");
	c1.setDarkestSecret("test ");
	std::cout << c1.getFirstName()  << std::endl;
	std::cout << c1.getLastName() << std::endl;
	std::cout << c1.getNickName() << std::endl;
	std::cout << c1.getPhoneNumber() << std::endl;
	std::cout << c1.getDarkestSecret() << std::endl;
	c2.setFirstName("Bob "); // appelle la méthode setFirstName sur c;
	c2.setLastName("Perluy ");
	c2.setNickName("bobi ");
	c2.setPhoneNumber("0503020100 ");
	c2.setDarkestSecret("oleeee ");
	std::cout << c2.getFirstName()  << std::endl;
	std::cout << c2.getLastName() << std::endl;
	std::cout << c2.getNickName() << std::endl;
	std::cout << c2.getPhoneNumber() << std::endl;
	std::cout << c2.getDarkestSecret() << std::endl;

	Contact contacts[8];
	
	// c.setLastName("DUPONT");
	// std::cout << c.getLastName() << std::endl;
	return 0;
}
/*
• PhoneBook
◦ Elle contient un tableau de contacts.
◦ Elle peut stocker un maximum de 8 contacts. Si l'utilisateur tente d'ajouter un 9e contact,
remplacez le plus ancien par le nouveau.
◦ Veuillez noter que l'allocation dynamique est interdite.

• Contact
Dans votre code, le répertoire doit être instancié en tant qu'objet de la classe `PhoneBook`.
Il en va de même pour les contacts : chacun d'eux doit être instancié en tant qu'objet de la classe `Contact`.
Vous êtes libre de concevoir les classes comme vous le souhaitez, mais gardez à l'esprit que tout élément
destiné à être utilisé uniquement au sein d'une classe doit être déclaré `private`, tandis que tout élément
pouvant être utilisé à l'extérieur de la classe doit être `public`.
N'oubliez pas de regarder les vidéos disponibles sur l'intranet.

Au lancement du programme, le répertoire est vide et l'utilisateur est invité à saisir l'une des trois commandes suivantes.
Le programme n'accepte que les commandes ADD, SEARCH et EXIT.
• ADD : enregistrer un nouveau contact
◦ Si l'utilisateur saisit cette commande, il est invité à entrer les informations du nouveau contact, champ par champ.
Une fois tous les champs remplis, le contact est ajouté au répertoire.
◦ Les champs du contact sont : prénom, nom, surnom, numéro de téléphone et secret le plus sombre.
Un contact enregistré ne peut comporter de champs vides.
• SEARCH : afficher un contact spécifique
◦ Afficher les contacts enregistrés sous forme de liste à 4 colonnes : index, prénom, nom et surnom.
◦ Chaque colonne doit avoir une largeur de 10 caractères. Elles sont séparées par un caractère « pipe » (|).
Le texte doit être aligné à droite.
Si le texte dépasse la largeur de la colonne, il doit être tronqué et le dernier caractère affichable remplacé par un point (.).
◦ Ensuite, demander à nouveau à l'utilisateur l'index de l'entrée à afficher.
Si l'index est hors limites ou invalide, définir un comportement approprié. Sinon, afficher les informations du contact, à raison d'un champ par ligne.
• EXIT
◦ Le programme se termine et les contacts sont perdus à jamais !
• Toute autre saisie est ignorée.
Une fois une commande correctement exécutée, le programme attend la suivante.
Il s'arrête lorsque l'utilisateur saisit EXIT.
Donnez un nom pertinent à votre exécutable
http://www.cplusplus.com/reference/string/string/ and of course http://www.cplusplus.com/reference/iomanip/
*/