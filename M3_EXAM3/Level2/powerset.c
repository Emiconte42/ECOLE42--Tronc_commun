/* ============================================================================
   SOLUTION 3: POWERSET / SUBSET SUM (fichier séparé powerset.c)
   ============================================================================ 
   CONCEPT: Afficher tous les sous-ensembles qui somment à target.
   
   MAPPING:
   - state: int subset[] (les indices des éléments sélectionnés)
   - depth: position actuelle dans arr
   - target: la somme recherchée
   - choices: prendre arr[depth] ou le sauter
   - is_valid(): sum + arr[depth] <= target (élagage!)
   - apply(): subset[count] = arr[depth], sum += arr[depth]
   - undo(): sum -= arr[depth]
   
   PIÈGE: L'ordre d'input compte! [3 2 1 4] != [1 2 3 4]
*/

#include <stdlib.h>
#include <stdio.h>

int	ft_atoi(char *s)
{
	int	result;
	int	sign;
	int	i;

	result = 0;
	sign = 1;
	i = 0;
	if (s[0] == '-')
	{
		sign = -1;
		i = 1;
	}
	while (s[i] >= '0' && s[i] <= '9')
	{
		result = result * 10 + (s[i] - '0');
		i++;
	}
	return (result * sign);
}

void	solve_powerset(int *arr, int len, int target, int *subset, 
					int count, int sum, int index)
{
	int	i;

	if (sum == target)
	{
		i = 0;
		while (i < count)
		{
			printf("%d", subset[i]);
			if (i < count - 1)
				printf(" ");
			i++;
		}
		printf("\n");
		return ;
	}
	if (index == len || sum > target)
		return ;
	subset[count] = arr[index];
	solve_powerset(arr, len, target, subset, count + 1, sum + arr[index], index + 1);
	solve_powerset(arr, len, target, subset, count, sum, index + 1);
}

int	main(int argc, char **argv)
{
	int	target;
	int	*arr;
	int	len;
	int	i;

	if (argc < 3)
		return (1);
	target = ft_atoi(argv[1]);
	len = argc - 2;
	arr = malloc(sizeof(int) * len);
	i = 0;
	while (i < len)
	{
		arr[i] = ft_atoi(argv[2 + i]);
		i++;
	}
	solve_powerset(arr, len, target, malloc(sizeof(int) * len), 0, 0, 0);
	free(arr);
	return (0);
}

/*
   COMPILATION:
   gcc -o powerset powerset.c
   
   TESTS:
   ./powerset 5 2 3           // 2 3\n
   ./powerset 12 5 7 4 3 2    // 5 7\n 5 4 3\n 7 3 2\n
   ./powerset 0 1 -1 2 -2     // \n -1 1 2 -2\n -1 1\n 2 -2\n
   
   TRACE MENTALE (target=5, arr=[2,3]):
   - solve_powerset([2,3], 2, 5, [], 0, 0, 0)
   - index=0, sum=0, sum != target
   - Branche 1: prendre arr[0]=2
     - solve_powerset([2,3], 2, 5, [2], 1, 2, 1)
     - index=1, sum=2, sum != target
     - Branche 1.1: prendre arr[1]=3
       - solve_powerset([2,3], 2, 5, [2,3], 2, 5, 2)
       - sum==target → print "2 3\n"
     - Branche 1.2: sauter arr[1]
       - solve_powerset([2,3], 2, 5, [2], 1, 2, 2)
       - index==len → return
   - Branche 2: sauter arr[0]
     - solve_powerset([2,3], 2, 5, [], 0, 0, 1)
     - index=1, sum=0
     - Branche 2.1: prendre arr[1]=3
       - solve_powerset([2,3], 2, 5, [3], 1, 3, 2)
       - index==len, sum=3 != target → return
*/
