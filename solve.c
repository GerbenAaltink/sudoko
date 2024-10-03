#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <rlib.h>
#include "rsolve.h"
#define N 9

int is_safe(int grid[N][N], int row, int col, int num)
{
	if (num == 0)
		return true;
	for (int x = 0; x < N; x++)
	{

		if (grid[row][x] == num && col != x)
		{
			return false;
		}
	}
	for (int x = 0; x < N; x++)
	{
		if (grid[x][col] == num && row != x)
		{

			return false;
		}
	}

	// Check box
	int startRow = row - row % (N / 3), startCol = col - col % (N / 3);
	for (int i = 0; i < N / 3; i++)
	{
		for (int j = 0; j < N / 3; j++)
		{
			if (grid[i + startRow][j + startCol] == num && row != i + startRow && col != j + startCol)
			{

				return false;
			}
		}
	}

	return true;
}

void clear()
{
	printf("\e[2J");
	fflush(stdout);
}
void print_grid(int grid[N][N])
{
	for (int row = 0; row < N; row++)
	{
		for (int col = 0; col < N; col++)
		{
			printf("%d ", grid[row][col]);
		}
		printf("\n");
	}
}

bool validate_grid(int grid[N][N])
{
	for (int row = 0; row < N; row++)
	{
		for (int col = 0; col < N; col++)
		{
			if (!is_safe(grid, row, col, grid[row][col]))
			{
				return false;
			}
		}
	}
	return true;
}
unsigned int solve2(int grid[N][N], unsigned long long *attempts)
{
	(*attempts)++;
	// if(*attempts % 10000000 == 0)
	//           print_grid(grid);
	unsigned int row, col;
	bool emptySpot = false;
	for (row = 0; row < N; row++)
	{
		for (col = 0; col < N; col++)
		{
			if (grid[row][col] == 0)
			{
				emptySpot = true;
				break;
			}
		}
		if (emptySpot)
		{
			break;
		}
	}
	if (!emptySpot)
		return true;
	for (int num = 1; num <= 9; num++)
	{
		if (is_safe(grid, row, col, num))
		{
			grid[row][col] = num;

			if (solve2(grid, attempts))
			{
				return *attempts;
			}

			grid[row][col] = 0;
		}
	}

	return 0;
}
int solve(int grid[N][N], unsigned long long *steps)
{
	*(steps)++;

	// if(*steps % 1000000){
	//	printf("%lld\n",*steps);
	// }

	int col;
	int row;
	bool found_empty = false;
	for (row = 0; row < N; row++)
	{
		for (col = 0; col < N; col++)
		{
			if (!grid[row][col])
			{
				found_empty = true;
			}
		}
		if (found_empty)
			break;
	}
	if (!found_empty)
		return true;
	for (int i = 1; i < 10; i++)
	{

		if (is_safe(grid, row, col, i))
		{
			grid[row][col] = i;

			if (solve(grid, steps))
			{
				print_grid(grid);

				return true;
			}
			grid[row][col] = 0;
		}
	}
	return false;
}

int get_next_int(FILE *f)
{
	char c;
	while ((c = getc(f)) && c != EOF)
	{
		if (c < '0' || c > '9')
		{
			continue;
		}
		return c - '0';
	}
	return -1;
}

bool grid_read(int grid[N][N])
{
	for (int row = 0; row < N; row++)
	{
		for (int col = 0; col < N; col++)
		{
			grid[row][col] = get_next_int(stdin);
			clear();
			print_grid(grid);
			if (grid[row][col] == -1)
			{
				return false;
			}
		}
	}
	return true;
} /*
 1 2 3 4 5 6 9 7 8
 4 5 6 7 8 9 1 2 3
 7 8 9 3 2 1 6 4 5
 9 4 7 8 3 2 5 1 6
 2 6 8 9 1 5 7 3 4
 5 3 1 6 7 0 0 0 0
 0 0 0 0 0 0 0 0 0
 0 0 0 0 0 0 0 0 0
 0 0 0 0 0 0 0 0 0 */

/*
0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  9
 0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0
 0  0  2  0  0  0  0  0  0
 0  0  0  0  0  0  0  2  0
 1  0  0  3  0  7  0  0  0
*/
bool is_empty(int grid[N][N])
{
	for (int i = 0; i < N * N; i++)
	{
		if (grid[i / 9][i % 9] != 0)
			return false;
	}
	return true;
}

/* 27 secs with rsolve. Good test:
 2  0  0  0  0  8  0  0  0
 0  0  0  0  0  0  0  9  0
 0  0  8  0  2  0  0  0  5
 0  0  0  0  0  7  0  0  0
 0  4  0  0  0  0  9  0  0
 0  0  0  3  0  5  0  0  0
 0  2  0  0  0  1  0  0  0
 0  9  0  0  0  0  8  0  0
 0  8  0  0  0  2  0  0  0
 */



int main()
{
	setbuf(stdout, 0);
	int grid[N][N];
	memset(grid, 0, N * N * sizeof(int));
	clear();
	printf("Paste grid in this format:\n");
	print_grid(grid);
	// grid[8][8] = 9;
	//	grid[8][8] = 8;
	while (true)
	{
		if (is_empty(grid))
			while (!grid_read(grid))
			{
				printf("Invalid grid\n");
			}
		if (validate_grid(grid))
		{
			break;
		}
		printf("Corrupt grid..\n");
		memset(grid, 0, N * N * sizeof(int));
	}
	clear();
	int grid_original[N][N];
	memcpy(grid_original, grid, N * N * sizeof(int));
	printf("Grid valid. Solving...\n");
	unsigned long long steps = 0;
	nsecs_t start = nsecs();
	RBENCH(1, {
		memcpy(grid, grid_original, N * N * sizeof(int));
		steps = 0;
		if (rsolve(grid, &steps))
		{
			print_grid(grid);
			printf("%s", "1");
		}
		
		
	});
	printf("Steps: %u\n", steps);;
		

	RBENCH(1, {
		memcpy(grid, grid_original, N * N * sizeof(int));
		steps = 0;
		if (solve2(grid, &steps)){
			printf("%s","2");
		}
		memcpy(grid, grid_original, N * N * sizeof(int));
	});
	printf("Steps: %u\n", steps);;
		
	nsecs_t end = nsecs();
		
	printf("Duration: %s\n", format_time(end - start));
	return 0;
}
