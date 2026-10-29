#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "utils.hpp"

int main()
{
	{
		std::cout << "Tests du sujet : " << std::endl;
		const Animal* meta = new Animal();
		const Animal* j = new Dog();
		const Animal* i = new Cat();

		std::cout << std::endl;
		std::cout << j->getType() << " " << std::endl;
		std::cout << i->getType() << " " << std::endl;

		i->makeSound();
		j->makeSound();
		meta->makeSound();
		std::cout << std::endl;
		delete (meta);
		delete (i);
		delete (j);
	}
	
	std::cout << "\n" << BOLD_YELLOW << "===== Autres tests =====" << RESET << std::endl;

	{
		std::cout << "\n" << BOLD_YELLOW << "--- Test 1 : WrongAnimal* -> WrongAnimal ---" << RESET << std::endl;
		const WrongAnimal* k = new WrongAnimal(); // Reference : son de WrongAnimal
		std::cout << "k = " << k->getType() << std::endl;
		k->makeSound();
		delete (k);
	}

	{
		std::cout << "\n" << BOLD_YELLOW << "--- Test 2 : WrongAnimal* -> WrongCat (pas de virtual) ---" << RESET << std::endl;
		const WrongAnimal* l = new WrongCat(); // Pas de virtual : le type du pointeur decide -> son de WrongAnimal
		std::cout << "l = " << l->getType() << std::endl;
		l->makeSound();
		delete (l);
	}

	{
		std::cout << "\n" << BOLD_YELLOW << "--- Test 3 : WrongCat* -> WrongCat ---" << RESET << std::endl;
		const WrongCat* m = new WrongCat(); // Pointeur WrongCat* -> son de WrongCat
		std::cout << "m = " << m->getType() << std::endl;
		m->makeSound();
		delete (m);
	}

	{
		std::cout << "\n" << BOLD_YELLOW << "--- Test 4 : constructeur par copie de Cat ---" << RESET << std::endl;
		const Cat* x = new Cat();
		const Cat* y = new Cat(*x); // Nouvel objet -> constructeur par copie
		std::cout << "y = " << y->getType() << std::endl;
		y->makeSound();
		delete (x);
		delete (y);
	}
	
	return 0;
}