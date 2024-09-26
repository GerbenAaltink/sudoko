# Sudoku

## Sudoku solver
NOTE: not all puzzles in source are valid!

## Sudoku generator


This one is resolvable in 227 attempts:
```
{9, 0, 0, 0, 0, 0, 0, 0, 6},
{0, 8, 0, 0, 0, 0, 0, 5, 0},
{0, 0, 7, 0, 0, 0, 4, 0, 0},
{0, 0, 0, 6, 0, 3, 0, 0, 0},
{7, 6, 4, 0, 5, 0, 0, 0, 0},
{0, 0, 0, 1, 0, 4, 0, 0, 0},
{0, 1, 6, 0, 0, 0, 3, 0, 0},
{0, 7, 0, 0, 0, 0, 0, 2, 0},
{8, 0, 0, 0, 0, 0, 0, 0, 1}
```
// 227 attempts
```
{9, 0, 0, 0, 0, 0, 0, 0, 6},
{0, 8, 0, 0, 0, 0, 0, 5, 0},
{0, 0, 7, 0, 0, 0, 4, 0, 0},
{0, 0, 0, 6, 0, 3, 0, 0, 0},
{7, 6, 4, 0, 5, 0, 0, 0, 0},
{0, 0, 0, 1, 0, 4, 0, 0, 0},
{0, 1, 6, 0, 0, 0, 3, 0, 0},
{0, 7, 0, 0, 0, 0, 0, 2, 0},
{8, 3, 0, 0, 0, 0, 0, 0, 1}
```
// 320220 attempts
```
Generation: 18/18
155.34s
Solution: 220388352
7 2 1 3 4 5 6 8 9 
4 6 8 1 9 7 2 5 3 
3 9 5 2 8 6 1 7 4 
2 5 3 4 6 9 7 1 8 
1 7 6 8 2 3 4 9 5 
9 8 4 5 7 1 3 2 6 
6 3 9 7 5 2 8 4 1 
5 4 7 6 1 8 9 3 2 
8 1 2 9 3 4 5 6 7
```


// WRONG
int grid[N][N] = {
    {9, 0, 0, 0, 0, 0, 0, 0, 6},
    {0, 8, 0, 0, 0, 0, 0, 5, 0},
    {0, 0, 7, 0, 0, 0, 4, 0, 0},
    {0, 0, 0, 6, 0, 3, 0, 0, 0},
    {0, 0, 0, 0, 5, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 4, 0, 0, 0},
    {0, 0, 6, 0, 0, 0, 3, 0, 0},
    {0, 7, 0, 0, 0, 0, 0, 2, 0},
    {8, 0, 0, 0, 0, 0, 0, 0, 2}
};
602.85s
Attempts: 
1663211698

No solution exists