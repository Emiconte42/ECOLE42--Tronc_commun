/* ============================================================================
   SOLUTION 2: PERMUTATIONS
   ============================================================================
   CONCEPT: Afficher toutes les permutations d'une chaîne en ordre alpha.
   
   MAPPING:
   - state: char result[] (la permutation en construction)
   - depth: position dans result (0 à len-1)
   - target: strlen(str)
   - choices: chaque caractère non encore utilisé
   - is_valid(): !used[i]
   - apply(): result[depth] = str[i], used[i] = 1
   - undo(): used[i] = 0
   
   PIÈGE: Alphabetical order = tri les premiers caractères à chaque niveau!
*/
#include <stdlib.h>
#include <stdio.h>

int	ft_strlen(char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	solve_perm(char *str, char *result, int *used, int depth, int len)
{
	int	i;

	if (depth == len)
	{
		printf("%s\n", result);
		return ;
	}
	i = 0;
	while (i < len)
	{
		if (!used[i])
		{
			result[depth] = str[i];
			used[i] = 1;
			solve_perm(str, result, used, depth + 1, len);
			used[i] = 0;
		}
		i++;
	}
}

int	main(int argc, char **argv)
{
	int	len;
	int	*used;
	char	*result;
	int	i;

	if (argc != 2)
		return (1);
	len = ft_strlen(argv[1]);
	if (len == 0)
		return (1);
	result = malloc(len + 1);
	result[len] = '\0';
	used = malloc(sizeof(int) * len);
	i = 0;
	while (i < len)
	{
		used[i] = 0;
		i ++;
	}
	solve_perm(argv[1], result, used, 0, len);
	free(result);
	free(used);
	return (0);
}

/*
   COMPILATION:
   gcc -o permutations permutations.c
   
   TESTS:
   ./permutations a         // a\n
   ./permutations ab        // ab\n ba\n
   ./permutations abc       // abc\n acb\n bac\n bca\n cab\n cba\n
   
   TRACE MENTALE (input "ab"):
   - solve_perm("ab", "??", [0,0], 0, 2)
   - depth=0, i=0: str[0]='a', used[0]=1
     - result[0]='a'
     - solve_perm("ab", "a?", [1,0], 1, 2)
     - depth=1, i=1: str[1]='b', used[1]=0
       - result[1]='b'
       - solve_perm("ab", "ab", [1,1], 2, 2)
       - depth=2 == len → print "ab\n"
     - used[1]=0
   - used[0]=0
   - i=1: str[1]='b', used[1]=0
     - result[0]='b'
     - used[1]=1
     - solve_perm("ab", "b?", [0,1], 1, 2)
     - depth=1, i=0: str[0]='a', used[0]=0
       - result[1]='a'
       - solve_perm("ab", "ba", [1,1], 2, 2)
       - depth=2 == len → print "ba\n"
*/
