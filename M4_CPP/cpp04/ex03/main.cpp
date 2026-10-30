#include "AMateria.hpp"
#include "IMateriaSource.hpp"
#include "MateriaSource.hpp"
#include "ICharacter.hpp"
#include "Character.hpp"
#include "Ice.hpp"
#include "Cure.hpp"
#include "utils.hpp"

/*
	Le sujet vient du jeu Final Fantasy VII. Une Materia est une petite boule magique.
	On la range dans l'equipement d'un personnage, et elle lui donne un sort.

	AMateria : Un sort « en general ». Il n'existe jamais tout seul.
	On a toujours un sort precis. Donc la classe est abstraite.

	Ice et Cure : Deux sorts precis. Ice attaque (« shoots an ice bolt »).
	Cure soigne (« heals … wounds »).

	Character : Un heros avec un sac de 4 emplacements.
	Il equipe des Materias, les pose par terre, et les lance sur une cible.

	MateriaSource : Un livre de recettes. Il apprend jusqu'a 4 modeles de sorts.
	Ensuite, on lui dit « fais-moi un ice » et il fabrique une copie neuve.
*/
int main()
{
	IMateriaSource* src = new MateriaSource();
	src->learnMateria(new Ice());
	src->learnMateria(new Cure());

	ICharacter* me = new Character("me");

	AMateria* tmp;
	tmp = src->createMateria("ice");
	me->equip(tmp);
	tmp = src->createMateria("cure");
	me->equip(tmp);

	ICharacter* bob = new Character("bob");

	me->use(0, *bob);
	me->use(1, *bob);

	delete bob;
	delete me;
	delete src;

	return 0;
}