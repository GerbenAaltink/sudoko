#ifndef SUDOKU_ALGORITHM_H
#define SUDOKU_ALGORITHM_H
#include "sudoku.h"
#include "rlib.h"
#include <stdint.h>
#define uin unsigned int
unsigned int rand_int(int min, int max){
    return rand() % (max - min + 1) + min;
}




double count_neighbors(grid_t * grid, uint row, uint col) {
    double count = 0.0;
    for(uint i = 0; i < row; i++){
        for(uint j = 0; j < grid->size; j++){
            if(grid->get(grid,row,j)->value != 0 && j != col)
                count += 1; //grid[row][j].initial ? 1.1 : 1.0;
        }
        for(uint j = 0; j < grid->size; j++){
            if(grid->get(grid,j,col)->value!= 0 && j != row)
                count += 1; //grid[j][col].initial ? 1.1 : 1.0;
        }
    }
    return count;
}




#endif
