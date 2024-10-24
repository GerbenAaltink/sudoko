#ifndef SUDOKU_GENERATE_H
#define SUDOKU_GENERATE_H
#include <time.h>
#include <stdlib.h>
#include "sudoku_header.h"
#include "sudoku_algorithm.h"


field_t * grid_get_random_empty_field(grid_t * grid){
    while(true){
    unsigned int row = rand_int(0,grid->size-1);
    unsigned int col = rand_int(0,grid->size-1);
        field_t * found = grid_get(grid,row,col);
        if(found->value == 0)
            return found;
    }
}

void grid_set_random_field(grid_t * grid){
    field_t * field = grid_get_random_empty_field(grid);
    field->value = rand_int(1,grid->size);
    field->initial = true;
}



grid_t * generate_sudoku(unsigned int size){
    srand(time(NULL));
    grid_t * grid = new_grid(size);

    for(unsigned int i = 0; i < rand_int(17,20); i++){
        grid_set_random_field(grid);
    }
    return grid;

}

#endif
