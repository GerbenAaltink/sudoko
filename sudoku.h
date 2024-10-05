#ifndef SUDOKU_H
#define SUDOKU_H

#include "rlib.h"
#include "rsolve.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ANSII_BLUE_BG "\033[34m"
#define ANSII_RED_BG "\033[31m"
#define ANSII_GREEN_BG "\033[32m"
#define ANSII_CLEAR "\033[0m"

#define N 9







void draw_cell(int c)
{
    if (c > 0 && c <= 3)
    {
        printf("%s", ANSII_BLUE_BG);
    }
    else if (c > 0 && c <= 6)
    {
        printf("%s", ANSII_GREEN_BG);
    }
    else if (c > 0 && c <= 9)
    {
        printf("%s", ANSII_RED_BG);
    }
    if(c){
        printf("%s%d ", c > 9 ? "" :" ", c );
    }else{
        printf(" 0 ");
    }    
    printf("%s", ANSII_CLEAR);
}

char * grid_to_string(int grid[N][N]){
	static char result[1024];
	result[0] = 0;
	for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
        	char chunk[4];
	    	chunk[0] = 0;
	        sprintf(chunk,"%d ",grid[row][col]);
	        strcat(result,chunk);	
        }
        strcat(result,"\n");
    }
    return result;
}

void print_grid(int grid[N][N],bool clear) {
    if(clear)
        printf("\033[2J\033[H");
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            draw_cell(grid[row][col]);
            ///printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
}

int count_neighbors2(int grid[N][N],int row, int col){
	// Check row
    int num = -1;
     int neighbors = 0;
    for (int x = 0; x < N; x++) {
        if(grid[row][x])
		neighbors++;
	if (grid[row][x] == num) {
            return 0;
        }
    }

    // Check column
    for (int x = 0; x < N; x++) {
        if(grid[x][col])
		neighbors++;
	if (grid[x][col] == num) {
            return 0;
        }
    }

    // Check box
    int startRow = row - row % (N / 3), startCol = col - col % (N / 3);
    for (int i = 0; i < N / 3; i++) {
        for (int j = 0; j < N / 3; j++) {
	    if(grid[i + startRow][j + startCol])
		neighbors++;
            if (grid[i + startRow][j + startCol] == num) {
                return 0;
            }
        }
    }
    return neighbors;
}

int is_safe(int grid[N][N], int row, int col, int num) {
    //if(count_neighbors(grid, row,col) < 4)
	  //  return false;
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
    int startRow = row - row % (N / 3), startCol = col - col % (N / 3);
    for (int i = 0; i < N / 3; i++) {
        for (int j = 0; j < N / 3; j++) {
            if (grid[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }

    return true;
}

void grid_reset(int * grid){
    memset(grid,0,N*N*sizeof(int));
}
int * grid_copy(int * grid){
    int * new_grid = malloc(N*N*sizeof(int));
    memcpy(new_grid,grid,N*N*sizeof(int));
    return new_grid;
}

int * grid_new(){
    return (int *)calloc(sizeof(int),N*N);
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

unsigned long long _solve(int grid[N][N], unsigned long long *attempts, bool draw){
    (*attempts)++;
    unsigned int row, col;
    if(!get_easiest_cell(grid,&row,&col)){
        //print_grid(grid, false);
        return *attempts;
    }
    for(int num = 1; num < N + 1; num++){
        if(is_safe(grid,row,col,num)){
            grid[row][col] = num;
            //print_grid(grid,true);
            if(_solve(grid,attempts,draw))
            {
                return *attempts;
            }
            grid[row][col] = 0;
        }
    }
    return 0;
}

unsigned int _solve2(int grid[N][N], unsigned long long * attempts, bool draw) {
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
            
            if (_solve2(grid,attempts,draw)) {
                return *attempts;
            }

            grid[row][col] = 0;
        }
    }
    
    return 0;
}
unsigned int solve2(int grid[N][N], bool draw){
    unsigned long long attempts = 0;
    return _solve(grid,&attempts, draw);
}

unsigned int solve(int grid[N][N],bool draw) {
    unsigned long long attempts = 0;
    return _solve2(grid,&attempts, draw);
}

#endif

