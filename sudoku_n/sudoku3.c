
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"

int main(){	

	grid_t * grid = new_grid(48);
	printf("%d \n",grid->rows[3].index);
	printf("%d \n",grid->fields[30].index);
	printf("%d \n",grid->rows[6].columns[6].index);
	field_t * f = grid->get(grid, 0,3);	
	printf("%d \n", f->index);	
	//for(int i = 0; i < grid->size*grid->size;i++){
//		printf("%c",grid->fields[i].box);
//	}
	grid->get(grid,5,5)->value = 3;
	grid->get(grid,0,0)->value = 1;
	grid->fields[0].value = 4;
	printf("**%d**\n",grid->get(grid,5,5)->index);
	printf("^^%d^^\n",grid->get(grid,0,0)->index);
	printf("!!%d!!\n", grid->get_empty_field(grid)->index);
	printf("Computer format:\n%s\n",grid_to_string(grid,row_to_string,field_to_string));
	printf("Human format:\n%s\n",grid_to_string_human(grid));
	printf("C format:\n%s\n",grid_to_string_c(grid));
	grid = generate_sudoku(48);
	printf("C format:\n%s\n",grid_to_string_c(grid));
	
	return 0;
}
