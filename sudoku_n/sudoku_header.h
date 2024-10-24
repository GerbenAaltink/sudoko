#ifndef SUDOKU_HEADER_H
#define SUDOKU_HEADER_H
#include "rlib.h"
#include <stdlib.h>
#include <stdio.h>
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
	row_t * row = (row_t *)calloc(sizeof(row_t),1);
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
	for(uint i = 0; i < grid->size * grid->size; i++){
		if(grid->fields[i].value == 0)
			return &grid->fields[i];
	}
	return NULL;
}
/*
char get_box_letter(grid_t * grid, unsigned int row, unsigned int col){
	//char box_letter = 65 + (row)*grid->size / grid->size / 3 + row;  // (row*grid->size+col)+(grid->size / 3);// + grid->size % (grid->size / 3));
	char box_letter = 65 + ((row % (grid->size / 3))) + grid->size % (grid->size / 3);// (row-1)*grid->size+(col-1)+(row-1)+(col-1); 
		//(65 + row * (grid->size / 3) % (grid->size / 3)+col);  /// (grid->size / 3));
	return box_letter;
}*/


grid_t * new_grid(unsigned int size){
	grid_t * grid = (grid_t *)calloc(1,sizeof(grid_t));
	grid->get = grid_get;
	grid->get_empty_field = grid_get_empty_field;
	grid->size = size;
	grid->row_count = 0;
	grid->rows = (row_t *)calloc(size, sizeof(row_t));
	grid->fields = (field_t *)calloc(size*size, sizeof(field_t));
	unsigned int field_index = 0;
	for(unsigned int irow = 0; irow < size; irow++){
		row_t * row = new_row(size);
		row->index = grid->row_count;
		row->column_count = size;
		grid->row_count++;
		grid->rows[row->index] = *row;
		for(unsigned int icol = 0; icol < size; icol++){
			field_t * field = new_field();
			field->column = icol;
			field->row = irow;
			field->index = field_index;
			//field->box = get_box_letter(grid, irow, icol);
			row->columns[icol] = *field;
			grid->fields[field_index] = *field;
			grid->field_count++;
			field_index++;
		}
	}
	return grid;
}
#endif
