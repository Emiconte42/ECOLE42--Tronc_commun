*Ce projet a ete realise dans le cadre du cursus 42 par emiconte.*

# get_next_line

## Description

get_next_line est une fonction qui permet de lire un fichier ligne par ligne a partir d’un file descriptor.

A chaque appel de la fonction, on doit retourner une seule ligne du fichier. La fonction est ensuite rappelee pour recuperer la suivante, jusqu’a la fin du fichier.

L’interet du projet est de comprendre plusieurs notions importantes en C :

- la lecture avec read
- la gestion d’un buffer
- l’allocation dynamique avec malloc
- la gestion propre de la memoire
- conserver un etat entre plusieurs appels de fonction

Le projet oblige aussi a gerer plusieurs cas particuliers :

- lignes tres longues
- fichiers sans \n final
- lecture par morceaux avec BUFFER_SIZE

L’objectif est donc d’ecrire une fonction robuste, capable de lire correctement un fichier quelle que soit la taille des lignes.

## Instructions

### Compilation

Pour compiler le projet, vous pouvez utiliser par exemple :

```
gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c -o gnl_test
```

BUFFER_SIZE definit la taille du buffer utilise pour lire le fichier.
Vous pouvez modifier sa valeur pour tester differents comportements.

### Utilisation

La fonction peut etre utilisee de cette maniere :

```c
#include "get_next_line.h"

int fd = open("fichier.txt", O_RDONLY);
char *line;

while ((line = get_next_line(fd)) != NULL)
{
    printf("%s", line);
    free(line);
}

close(fd);
```

Chaque appel de get_next_line retourne la prochaine ligne du fichier.

Quand la fin du fichier est atteinte, la fonction retourne NULL.

## Explication de l’algorithme

Le principe est le suivant.

La fonction lit le fichier par morceaux de taille BUFFER_SIZE.
Chaque lecture est stockee dans un buffer temporaire.

Le contenu lu est ensuite ajoute a une zone memoire appelee stash.
Cette stash contient tout ce qui a ete lu mais qui n’a pas encore ete retourne.

Ensuite :

- on lit un morceau avec read
- on ajoute ce morceau a la stash
- on verifie si la stash contient \n

Si un \n est trouve, cela signifie que nous avons une ligne complete.

La fonction :

- extrait cette ligne
- met a jour la stash avec ce qu’il reste
- retourne la ligne

Si aucun \n n’est trouve, la fonction continue de lire jusqu’a :

- trouver un \n
- atteindre la fin du fichier

La stash est declaree statique, ce qui permet de conserver les donnees entre les appels de get_next_line.

Cette approche permet de :

- lire efficacement les fichiers
- gerer des lignes de taille arbitraire
- ne jamais relire les memes caracteres

## Ressources

- [Descripteurs de fichiers en C](https://man7.org/linux/man-pages/man2/open.2.html)
- [Allocation dynamique en C](https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/)
- [Video youtube - Comprendre GNL](https://www.youtube.com/watch?v=-Mt2FdJjVno)
- [Video youtube - Coder GNL](https://www.youtube.com/watch?v=xgDSXNOHTIA)

**Utilisation de l'IA :**
L'IA a ete utilisee pour la mise en forme de ce README et la clarification de certains points de redaction. Aucun code n'a ete genere par l'IA pour l'implementation.
