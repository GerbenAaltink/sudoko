#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define N 9 

unsigned int rand_int(int min, int max){
    return rand() % (max - min + 1) + min;
}


unsigned int count_neighbors(int grid[N][N], int row, int col) {
    unsigned int count = 0;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < 9; j++){
            if(grid[row][j] != 0 && j != col)
                count++;
        }
        for(int j = 0; j < 9; j++){
            if(grid[j][col] != 0 && j != row)
                count++;
        }
    }
    return count;
}


void print_grid(int grid[N][N],bool clear) {
    if(clear)
        printf("\033[2J\033[H");
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
}

bool is_safe(int grid[N][N], int row, int col, int num) {
    // Check row
    for (int x = 0; x < N; x++) {
        if (grid[row][x] == num) {
            return false;
        }
    }

    // Check column
    for (int x = 0; x < N; x++) {
        if (grid[x][col] == num) {
            return false;
        }
    }

    // Check box
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }

    return true;
}

void grid_reset(int grid[N][N]){
    memset(grid,0,N*N*sizeof(int));
}
int * grid_copy(int grid[N][N]){
    int * new_grid = malloc(N*N*sizeof(int));
    memcpy(new_grid,grid,N*N*sizeof(int));
    return new_grid;
}

int * grid_new(){
    return (int *)calloc(sizeof(int),N*N);
}

bool get_easiest_cell(int grid[N][N], unsigned int * easy_row, unsigned int * easy_col){
    unsigned int easy_neighbor_count = 0;
    bool found = true;
    for(int row = 0; row < N; row++){
        for (int col = 0; col < N; col++){
            unsigned ncount = count_neighbors(grid,row,col);
            if(easy_neighbor_count < ncount){
                easy_neighbor_count = ncount;
                *easy_row = row;
                *easy_col = col;
                found = true;
                return true;
            }
        }
    }
    return false;
}

bool empty_spot_is_available(int grid[N][N]){
    for (unsigned int row = 0; row < N; row++) {
        for (unsigned int col = 0; col < N; col++) {
            if (grid[row][col] == 0) {
                return true;
            }
        }
    }
    return false;
}

unsigned int _solve(int grid[N][N], int * attempts, bool draw) {
    (*attempts)++;
    unsigned int row, col;
    bool emptySpot = false;

    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            if (grid[row][col] == 0) {
               // if(count_neighbors(grid,row,col) == 8){
                    
                //    print_grid(grid,true);
                //    printf("Found neighbors\n");
                //    exit(0);
                //}
                emptySpot = true;
                break;
            }
        }
        if (emptySpot) {
            break;
        }
    }
    if(!emptySpot)
        return true;
    /*
    if (!empty_spot_is_available(grid)) {
        return true;
    }*/

    for (int num = 1; num <= 9; num++) {
        //unsigned int * easy_row = calloc(sizeof(int),1);
        //unsigned int * easy_col = calloc(sizeof(int), 1);
        //if(get_easiest_cell(grid,easy_row,easy_col)){
        //     row = *easy_row;
        //     free(easy_row);
        //     col = *easy_col;
        //     free(easy_col);

        //}
        if (is_safe(grid, row, col, num)) {
            grid[row][col] = num;
            if(draw)
                print_grid(grid,true);
            
            if (_solve(grid,attempts,draw)) {
                return *attempts;
            }

            grid[row][col] = 0;
        }
    }
    
    return 0;
}

unsigned int solve(int grid[N][N],bool draw) {
    int attempts = 0;
    return _solve(grid,&attempts, draw);
}