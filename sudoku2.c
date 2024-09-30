#include "sudoku.h"


int main() {
    setbuf(stdout,0);
    
    int grid[N][N] = {//13456789
    //{9, 4, 3, 5, 2, 7, 8, 1, 6},
        {9, 0, 0, 0, 0, 0, 0, 0, 6},
        {0, 8, 0, 0, 0, 0, 0, 5, 0},
        {0, 0, 7, 0, 0, 0, 4, 0, 0},
        {0, 0, 0, 6, 0, 3, 0, 0, 0},
        {7, 6, 4, 0, 5, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 4, 0, 0, 0},
        {0, 1, 6, 0, 0, 0, 3, 0, 0},
        {0, 7, 0, 0, 0, 0, 0, 2, 0},
        {8, 3, 0, 0, 0, 0, 0, 0, 1}
    };

    RBENCH(1,{
        unsigned int attempts = solve2(grid,false);
        if (!attempts) {
            printf("No solution exists\n");
        }
        printf("Attempts: %d\n", attempts);
    });
    return 0;
}
