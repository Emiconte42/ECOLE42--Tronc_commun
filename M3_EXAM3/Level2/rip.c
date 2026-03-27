/* ============================================================================
   SOLUTION 4: RIP (Remove Invalid Parenthesis) (fichier séparé rip.c)
   ============================================================================ 
   CONCEPT: Trouver toutes les manières de balancer une chaîne de parenthèses
            en remplaçant (pas supprimant) les caractères par des espaces.
   
   MAPPING:
   - state: char result[] (copie modifiable de str)
   - depth: position dans result
   - target: len(result) (on traite chaque caractère)
   - choices: garder le caractère OU le remplacer par espace
   - is_valid(): check si le prefix est valide (open >= close à chaque étape)
   - apply(): result[depth] = ' ' (ou garder le caractère)
   - undo(): result[depth] = original
   
   PIÈGE: Vérifier PENDANT qu'on construit, pas après!
   */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int	ft_strlen(char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	is_valid_partial(char *str, int depth)
{
	int	open;
	int	close;
	int	i;

	open = 0;
	close = 0;
	i = 0;
	while (i < depth)
	{
		if (str[i] == '(')
			open++;
		else if (str[i] == ')')
			close++;
		if (close > open)
			return (0);
		i++;
	}
	return (1);
}

int	is_balanced(char *str)
{
	int	open;
	int	close;
	int	i;

	open = 0;
	close = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '(')
			open++;
		else if (str[i] == ')')
			close++;
		if (close > open)
			return (0);
		i++;
	}
	return (open == close);
}

void	solve_rip(char *result, int depth, int len)
{
	if (!is_valid_partial(result, depth))
		return ;
	if (depth == len)
	{
		if (is_balanced(result))
			printf("%s\n", result);
		return ;
	}
	solve_rip(result, depth + 1, len);
	char temp = result[depth];
	result[depth] = ' ';
	solve_rip(result, depth + 1, len);
	result[depth] = temp;
}

int	main(int argc, char **argv)
{
	int	len;
	char	*result;

	if (argc != 2)
		return (1);
	len = ft_strlen(argv[1]);
	result = malloc(len + 1);
	strcpy(result, argv[1]);
	solve_rip(result, 0, len);
	free(result);
	return (0);
}

/*
   COMPILATION:
   gcc -o rip rip.c
   
   TESTS:
   ./rip "(())"     // "( ( ) )\\n" ou vide si déjà balanced
   ./rip "(()"      // "_ ( )\n" ou "( _ )\n"
   ./rip "()())()"  // solutions équilibrées
   
   TRACE MENTALE (input "(()" — problème: 2 open, 1 close):
   - solve_rip("(()", 0, 3)
   - is_valid_partial("(()", 0) → open=0, close=0 → return 1
   - depth != 3
   - Choix 1: garder result[0]='('
     - solve_rip("(()", 1, 3)
     - is_valid_partial("(()", 1) → open=1, close=0 → return 1
     - depth != 3
     - Choix 1.1: garder result[1]='('
       - solve_rip("(()", 2, 3)
       - is_valid_partial("(()", 2) → open=2, close=0 → return 1
       - Choix 1.1.1: garder result[2]=')'
         - solve_rip("(()", 3, 3)
         - depth==3, is_balanced("(()") → open=2, close=1 → return 0
       - Choix 1.1.2: remplacer result[2]=' ' → "(_ "
         - solve_rip("(( ", 3, 3)
         - depth==3, is_balanced("(( ") → open=2, close=0 → return 0
     - Choix 1.2: remplacer result[1]=' '
       - solve_rip("( ()", 2, 3)
       - is_valid_partial("( ()", 2) → open=2, close=0 → return 1
       - Choix 1.2.1: garder result[2]=')'
         - solve_rip("( ()", 3, 3)
         - is_balanced("( ()") → open=2, close=1 → return 0
       - Choix 1.2.2: remplacer result[2]=' '
         - solve_rip("( ( ", 3, 3)
         - is_balanced("( ( ") → open=2, close=0 → return 0
   - Choix 2: remplacer result[0]=' '
     - solve_rip(" ()", 1, 3)
     - is_valid_partial(" ()", 1) → open=0, close=0 → return 1
     - Choix 2.1: garder result[1]='('
       - solve_rip(" ()", 2, 3)
       - is_valid_partial(" ()", 2) → open=1, close=0 → return 1
       - Choix 2.1.1: garder result[2]=')'
         - solve_rip(" ()", 3, 3)
         - is_balanced(" ()") → open=1, close=1 → return 1 → PRINT " ()\n"
*/

