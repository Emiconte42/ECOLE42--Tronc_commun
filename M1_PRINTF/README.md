# ft_printf

*Ce projet a ete cree dans le cadre du programme 42 par emiconte.*

## Description

Le projet consiste a recoder la fonction **printf** de base.

### CE QUE FAIT LA FONCTION

Elle formate des donnees (elle met tout en forme) selon une chaine donnee et ca ecrit le resultat sur la sortie standard.

### OBJECTIF PRINCIPAL 

Recoder printf nous permet de comprendre comment utiliser les fonctions variadiques, gerer les conversions, le formatage, les bases numeriques. 
Une fonction variadique est une fonction qui **accepte un nombre variable d'argument.**

Dans un premier temps la fonction printf parcourt la string caractere par caractere.
Si tout est normal, elle ecrit sur la sortie standard.
Si elle rencontre un %, alors la fonction delegue l'ecriture a ce qu'on appelle un specificateur.

### CONVERSIONS & SPECIFICATEURS 

- `%c` : Affichage d'un caractere
- `%s` : Affichage d'une chaine de caracteres
- `%p` : Affichage d'un pointeur en format hexadecimal
- `%d` / `%i` : Affichage d'un nombre decimal (base 10) signe
- `%u` : Affichage d'un nombre decimal (base 10) non signe
- `%x` : Affichage d'un nombre en hexadecimal (base 16) minuscule
- `%X` : Affichage d'un nombre en hexadecimal (base 16) majuscule
- `%%` : Affichage du symbole pourcentage

## Instructions

### Compilation

Pour compiler la bibliotheque `libftprintf.a`, il faut ouvrir le terminal a la racine et taper :

``` bash
make
```

Cela creera le fichier `libftprintf.a` qui contient toutes les fonctions compilees.

### Nettoyage

Pour supprimer les fichiers objets .o:

```bash
make clean
```

Pour supprimer les fichiers objets .o et la bibliotheque :

```bash
make fclean
```

Pour recompiler entierement le projet :

```bash
make re
```

### Utilisation

Compiler le programme avec la bibliotheque :
```bash
gcc -Wall -Wextra -Werror le_fichier.c libftprintf.a
```

Cela creera un executable `a.out` que vous pouvez executer avec `./a.out`.

### Exemple de main

```c
#include "printf.h"

int main(void)
{
// Affichage de chaines et caracteres
ft_printf("Bonjour %s, comment %cas-tu?\n", "Toi", 'v');
// Sortie : Bonjour Toi, comment v-tu?

// Affichage de nombres
ft_printf("Entier: %d, Non signe: %u\n", -42, 42);
// Sortie : Entier: -42, Non signe: 42

// Affichage hexadecimal
ft_printf("Hexa minuscule: %x, Hexa majuscule: %X\n", 255, 255);
// Sortie : Hexa minuscule: ff, Hexa majuscule: FF

// Affichage pourcentage
ft_printf("Pourcentage: %%\n");
// Sortie : Pourcentage: %
    return (0);
}
```

## Architecture du projet

### Structure des fichiers

- **ft_printf.c** : Fonction principale et dispatcher de formats
- **printf_outils.c** : Fonctions utilitaires pour l'affichage de base
- **printf_outils_nbr.c** : Fonctions pour la conversion numerique
- **printf.h** : Header contenant les prototypes et includes
- **Makefile** : Script de compilation automatique

### Fonctions principales

|         Fonction         		| 							Description 							|
|-------------------------------|-------------------------------------------------------------------|
| `ft_printf`              		| Point d'entree principal											|
| `ft_format` 					| Fonction qui appelle la bonne fonction selon le specificateur 	|
| `ft_putchar` 					| Affiche un caractere												|
| `ft_putstr` 					| Affiche une chaîne de caracteres 									|
| `ft_putnbr` 					| Affiche un entier signe (negatif + positif)						|
| `ft_putnbr_unsigned`			| Affiche un entier non signe (uniquement positif)					|
| `ft_putnbr_hexa_lowercase`	| Affiche un nombre en hexadecimal minuscule 						|
| `ft_putnbr_hexa_uppercase`	| Affiche un nombre en hexadécimal majuscule						|
|-------------------------------|-------------------------------------------------------------------|

Les fonctions `ft_putnbr`, `ft_putnbr_unsigned`, et les fonctions hexadecimales utilisent **la recursivite** pour extraire et afficher les chiffres.
Chaque fonction d'affichage retourne le nombre de caracteres ecrits, permettant a `ft_printf` de compter le total (count).

## Ressources

Utilisation d'internet et de diverses sources de tutoriels et explications : 

- https://42-cursus.gitbook.io/guide/1-rank-01/ft_printf
- https://www.youtube.com/@n1kito

### Utilisation de l'IA

Dans le cadre de ce projet, l'IA a servi d'**outil pedagogique** et de **comprehension**, mais la logique et l'implementation principale restent un travail personnel.