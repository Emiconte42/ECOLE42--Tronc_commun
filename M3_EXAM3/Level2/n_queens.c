/* ============================================================================
   SUJET 1: N-QUEENS
   ============================================================================ 
   CONCEPT: Placer N reines sur un échiquier N×N sans qu'elles ne s'attaquent.
   
   MAPPING TEMPLATE:
   - state: int queens[] où queens[row] = col
   - depth: row (ligne actuelle)
   - target: n (nombre total de reines)
   - choices: col de 0 à n-1
   - is_valid(): pas de conflit colonne/diagonale
   - apply(): queens[row] = col
   - undo(): queens[row] = -1
   
   PIÈGE: Diagonales! queens[i] - col == i - row (diag1) 
                        queens[i] - col == row - i (diag2)
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int	is_valid(int *queens, int row, int col, int n)
{
	int	i;

	i = 0;
	while (i < row)
	{
		if (queens[i] == col)
			return (0);
		if (queens[i] - col == i - row || queens[i] - col == row - i)
			return (0);
		i++;
	}
	return (1);
}

void	solve(int *queens, int row, int n)
{
	int	col;

	if (row == n)
	{
		col = 0;
		while (col < n)
		{
			printf("%d", queens[col]);
			if (col < n - 1)
				printf(" ");
			col++;
		}
		printf("\n");
		return ;
	}
	col = 0;
	while (col < n)
	{
		if (is_valid(queens, row, col, n))
		{
			queens[row] = col;
			solve(queens, row + 1, n);
			queens[row] = -1;
		}
		col++;
	}
}

int	main(int argc, char **argv)
{
	int	n;
	int	*queens;
	int	i;

	if (argc != 2)
		return (1);
	n = atoi(argv[1]);
	if (n <= 0)
		return (1);
	queens = malloc(sizeof(int) * n);
	i = 0;
	while (i < n)
		queens[i++] = -1;
	solve(queens, 0, n);
	free(queens);
	return (0);
}

/* 
   COMPILATION:
   gcc -o n_queens n_queens.c
   
   TESTS:
   ./n_queens 1      // 1\n
   ./n_queens 4      // 1 3 0 2\n 2 0 3 1\n
   
   TRACE MENTALE (n=1):
   - solve(queens, 0, 1)
   - row=0, n=1, row != n
   - col=0
   - is_valid(queens, 0, 0) → pas de reine avant, return 1
   - queens[0] = 0
   - solve(queens, 1, 1)
     - row=1, n=1, row == n → print "0\n"
   - queens[0] = -1
   - col=1, loop end
*/

