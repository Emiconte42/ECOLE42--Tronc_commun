#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "utils.hpp"

int main()
{
	{
		std::cout << BOLD_YELLOW << "----- Test sujet -----" << endofline;
		const Animal* j = new Dog();
		const Animal* i = new Cat();
		delete j;
		delete i;

		const Animal* r = new Animal(); // sur l'exo suivant il ne fonctionnera pas, mais ici oui
		delete r;
	}

	{
		std::cout << BOLD_YELLOW << "\n----- Tableau d'animaux : moitie Dog, moitie Cat -----" << endofline;
		const int NB_ANIMALS = 6; // const obligatoire : en C++98 la taille d'un tableau doit etre connue a la compilation
		Animal	*animals[NB_ANIMALS];

		for (int i = 0; i < NB_ANIMALS; i++)
		{
			if (i % 2 == 0) // un sur deux : pair = Dog, impair = Cat
				animals[i] = new Dog();
			else
				animals[i] = new Cat();
		}
		for (int i = 0; i < NB_ANIMALS; i++)
			animals[i]->makeSound();
		for (int i = 0; i < NB_ANIMALS; i++)
			delete animals[i];
	}

	{
		// shallow copy : copie l'adresse du Brain -> les 2 chiens partagent le meme Brain
		// deep copy : cree un nouveau Brain avec le meme contenu -> chaque chien a le sien
		std::cout << BOLD_YELLOW << "\n----- Deep copy : constructeur de copie (Dog) -----" << endofline;
		Dog	rex;
		rex.getBrain()->setIdeas(0, "Enterrer un os dans le jardin");

		Dog	medor(rex); // deep copy : medor a son propre Brain, pas celui de rex
		medor.getBrain()->setIdeas(0, "Courir apres le facteur"); // on change l'idee de medor, celle de rex ne doit pas bouger

		std::cout << CYAN << "idea[0] de rex   : " << rex.getBrain()->getIdeas(0) << endofline;
		std::cout << CYAN << "idea[0] de medor : " << medor.getBrain()->getIdeas(0) << endofline;
		std::cout << CYAN << "Brain de rex     : " << rex.getBrain() << endofline;
		std::cout << CYAN << "Brain de medor   : " << medor.getBrain() << endofline; // si les adresses sont differentes, c'est bien une deep copy
	}

	{
		std::cout << BOLD_YELLOW << "\n----- Deep copy : operator= (Cat) -----" << endofline;
		Cat	caramel;
		Cat	minou;
		caramel.getBrain()->setIdeas(0, "Dormir au soleil");

		minou = caramel; // deep copy : minou jette son ancien Brain et recopie celui de caramel
		minou.getBrain()->setIdeas(0, "Miauler en grattant la porte"); // on change l'idee de minou, celle de caramel ne doit pas bouger

		std::cout << CYAN << "idea[0] de caramel : " << caramel.getBrain()->getIdeas(0) << endofline;
		std::cout << CYAN << "idea[0] de minou   : " << minou.getBrain()->getIdeas(0) << endofline;
		std::cout << CYAN << "Brain de caramel   : " << caramel.getBrain() << endofline;
		std::cout << CYAN << "Brain de minou     : " << minou.getBrain() << endofline; // si les adresses sont differentes, c'est bien une deep copy
	}

	{
		std::cout << BOLD_YELLOW << "\n----- Brain : index hors limites -----" << endofline;
		Cat	felix;
		felix.getBrain()->setIdeas(150, "Chasser une souris"); // ignore car il n'y a que 100 idees (de 0 a 99)
		std::cout << CYAN << "idea[150] de felix : " << felix.getBrain()->getIdeas(150) << endofline;
	}
	return (0);
}
