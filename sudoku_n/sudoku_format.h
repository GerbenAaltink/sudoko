#ifndef SUDOKU_FORMAT_H
#define SUDOKU_FORMAT_H
#include "sudoku_header.h"
#include <string.h>
#include "rlib.h"
#define SUDOKU_FORMAT_BUFFER_SIZE 90000 

char * field_to_string_human(field_t * field){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	sprintf(result,"%d ",field->value);
	return sbuf(result);
}

char * field_to_string_comma(field_t * field){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	sprintf(result,"%d, ",field->value);
	return sbuf(result);
}

char * field_to_string(field_t * field){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	sprintf(result,"%d",field->value);
	return sbuf(result);
}
char * row_to_string(row_t * row,char * field_string_fn(field_t * field)){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	for(uint i = 0; i < row->column_count; i++){
		strcat(result, field_string_fn(&row->columns[i]));
	}
	return sbuf(result);
}
char * row_to_string_human(row_t * row,char * field_string_fn(field_t * field)){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	for(uint i = 0; i < row->column_count; i++){
		strcat(result, field_string_fn(&row->columns[i]));
	}
	result[strlen(result)-2] = 0;
	strcat(result, "\n");
	return sbuf(result);
}
char * row_to_string_c(row_t * row){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
		strcat(result,"{");
	for(uint i = 0; i < row->column_count; i++){
		strcat(result, field_to_string_comma(&row->columns[i]));
	}
	result[strlen(result)-2] = 0;
	strcat(result,"},\n");
	return sbuf(result);
}

char * grid_to_string(grid_t * grid, char * row_to_string_fn(row_t * row,char * (field_t *)), char * field_string_fn(field_t * field)){
	static char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	for(uint irow = 0; irow < grid->size; irow++){
		row_t row = grid->rows[irow];
		row.column_count++;
		strcat(result,row_to_string_fn(&row,field_string_fn));
	}
	result[strlen(result)-1] = 0;
	return sbuf(result);
}
char * grid_to_string_c(grid_t * grid){
	char result[SUDOKU_FORMAT_BUFFER_SIZE] = {0};
	result[0] = 0;
	strcat(result,"{\n");
	for(uint i = 0; i < grid->size; i++){
		strcat(result,"  ");
		strcat(result,row_to_string_c(&grid->rows[i]));
		
	}
	result[strlen(result)-2] = 0;
	strcat(result,"\n}");
	return sbuf(result);
}
char * grid_to_string_human(grid_t * grid){
	return grid_to_string(grid, row_to_string_human, field_to_string_human);
}

#endif
