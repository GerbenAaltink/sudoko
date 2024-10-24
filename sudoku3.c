#include "rlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct field_t {
	unsigned int value;
	unsigned int selected;
	unsigned int initial;
	unsigned int column;
	unsigned int row;
	unsigned int index;
	unsigned char box;
} field_t;

field_t * new_field(){
	field_t * field = (field_t *)calloc(1,sizeof(field_t));
	return field;
}

typedef struct row_t {
	unsigned int index;
	field_t * columns;
	unsigned int column_count;
} row_t; 

row_t * new_row(unsigned int size) {
	row_t * row = (row_t *)calloc(1, sizeof(row));
	row->columns = calloc(size,sizeof(field_t));
	row->index = 0;
	return row;
}

typedef struct grid_t {
	unsigned int size;
	unsigned int row_count;
	unsigned int field_count;
	row_t * rows;
	field_t * fields;
	field_t * (*get)(struct grid_t * grid, unsigned int row, unsigned int col);
	field_t * (*get_empty_field)(struct grid_t * grid);
} grid_t;

field_t * grid_get(grid_t * grid, unsigned int row, unsigned int col){
	return &grid->rows[row].columns[col];
}
field_t * grid_get_empty_field(grid_t *  grid){
	for(int i = 0; i < grid->size * grid->size; i++){
		printf("AT:%d\n",i);
		printf("V:%d\n",grid->fields[i].value);
		printf("V2:%d\n",grid->rows[i].columns[i].value);
		printf("V2:%d\n",grid->get(grid,i,i)->value);
		if(grid->fields[i].value == 0)
			return &grid->fields[i];
	}
	return NULL;
}

char get_box_letter(grid_t * grid, unsigned int row, unsigned int col){
	//char box_letter = 65 + (row)*grid->size / grid->size / 3 + row;  // (row*grid->size+col)+(grid->size / 3);// + grid->size % (grid->size / 3));
	char box_letter = 65 + ((row % (grid->size / 3))) + grid->size % (grid->size / 3);// (row-1)*grid->size+(col-1)+(row-1)+(col-1); 
		//(65 + row * (grid->size / 3) % (grid->size / 3)+col);  /// (grid->size / 3));
	return box_letter;
}


grid_t * new_grid(unsigned int size){
	grid_t * grid = (grid_t *)calloc(1,sizeof(grid_t));
	grid->get = grid_get;
	grid->get_empty_field = grid_get_empty_field;
	grid->size = size;
	grid->row_count = 0;
	grid->rows = (row_t *)calloc(size, sizeof(row_t));
	grid->fields = (field_t *)calloc(size*size, sizeof(field_t));
	for(unsigned int irow = 0; irow < size; irow++){
		row_t * row = new_row(size);
		row->index = grid->row_count;
		row->column_count = size;
		grid->row_count++;
		grid->rows[row->index] = *row;
		for(unsigned int icol = 0; icol < size; icol++){
			unsigned int field_index = irow * size + icol;
			field_t * field = new_field();
			field->column = icol;
			field->row = irow;
			field->index = field_index;
			field->box = get_box_letter(grid, irow, icol);
			row->columns[icol] = *field;
			grid->fields[field_index] = *field;
			grid->field_count++;
		}
	}
	return grid;
}

char * field_to_string_human(field_t * field){
	static char result[4] = {0};
	result[0] = 0;
	sprintf(result,"%d, ",field->value);
	return sbuf(result);
}

char * field_to_string(field_t * field){
	static char result[4] = {0};
	result[0] = 0;
	sprintf(result,"%d",field->value);
	return sbuf(result);
}
char * row_to_string(row_t * row,char * field_string_fn(field_t * field)){
	static char result[100] = {0};
	result[0] = 0;
	for(unsigned int i = 0; i < row->column_count; i++){
		strcat(result, field_string_fn(&row->columns[i]));
	}
	return sbuf(result);
}
char * row_to_string_human(row_t * row,char * field_string_fn(field_t * field)){
	static char result[100] = {0};
	result[0] = 0;
	for(unsigned int i = 0; i < row->column_count; i++){
		strcat(result, field_string_fn(&row->columns[i]));
	}
	strcat(result, "\n");
	return sbuf(result);
}
char * row_to_string_c(row_t * row){
	static char result[1000] = {0};
	result[0] = 0;
		strcat(result,"{");
	for(int i = 0; i < row->column_count; i++){
		strcat(result, field_to_string_human(&row->columns[i]));
	}
	result[strlen(result)-2] = 0;
	strcat(result,"},\n");
	return sbuf(result);
}

char * grid_to_string(grid_t * grid, char * row_to_string_fn(row_t * row,char * (field_t *)), char * field_string_fn(field_t * field)){
	static char result[1000] = {0};
	result[0] = 0;
	for(unsigned int irow = 0; irow < grid->size; irow++){
		row_t row = grid->rows[irow];
		row.column_count++;
		strcat(result,row_to_string_fn(&row,field_string_fn));
	}
	result[strlen(result)-1] = 0;
	return sbuf(result);
}
char * grid_to_string_c(grid_t * grid){
	char result[1000] = {0};
	result[0] = 0;
	strcat(result,"{\n");
	for(int i = 0; i < grid->size; i++){
		strcat(result,"  ");
		strcat(result,row_to_string_c(&grid->rows[i]));
		
	}
	result[strlen(result)-2] = 0;
	strcat(result,"\n}");
	return sbuf(result);
}
char * grid_to_string_human(grid_t * grid){
	return sbuf(grid_to_string(grid, row_to_string_human, field_to_string_human));
}


int main(){	

	grid_t * grid = new_grid(9);
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
	printf("**%d**\n",grid->get(grid,5,5)->index);
	printf("^^%d^^\n",grid->get(grid,5,5)->value);
	printf("$$%d$$\n", grid->fields[0].index);
	printf("!!%d!!\n", grid->get_empty_field(grid)->index);
	printf("<%s>\n",grid_to_string(grid,row_to_string,field_to_string));
	printf("<%s>\n",grid_to_string_human(grid));
	printf("<%s>\n",grid_to_string_c(grid));

	return 0;
}
