#include "sudoku.h"


static char * gridchars  = "abcdefghijklmnopqrstuvwxyz";

int * get_box(char c){

     
     unsigned static int result[N];
     unsigned int index = 0;
     unsigned int offset = strchr(gridchars,c) - gridchars;
     offset = offset * N / 3;
     for(unsigned int i = offset; i < offset + N/3;i++){
	result[index] = i;
	index++;
     }
     for(unsigned int i = offset + N; i < offset + N +  N/3;i++){
	result[index] = i;
	index++;
     }
     for(unsigned int i = offset + N + N; i < offset + N + N +  N/3;i++){
	result[index] = i;
	index++;
     }
     return result;
}
int * grid_to_array(int grid[N][N]){
	static int result[N*N];
	int index = 0;
	for(uint row = 0; row < N; row++){
	    for(uint col = 0; col < N; col++){
		result[index] = grid[row][col];
		index++;
	    }
	}
	return result;
}

char get_box_char(int row, int col){
	unsigned int index = 0;
	unsigned int cell_index = row * N + col;
	int char_index = cell_index / N;
	
	while(true){
		char c = gridchars[index];
		int * numbers = get_box(c);
		for(int i = 0; i < N; i++){
			if(numbers[i] == cell_index){
				return c;
			}
		}
		index++;
	}
	return 0;
}

int * get_box_values(int grid[N][N],char c){
     int * box = get_box(c);
     static unsigned int values[N];
     int * array = grid_to_array(grid);
     //printf("HIERR\n");
     for(unsigned int i = 0; i < N; i++){
	 printf("FROM ARRAY: %d\n", box[i]);
	     values[i] = array[box[i]];
     }
     return values;
}

void print_box(int * box){
     unsigned int cols = 0;
     for(int i = 0; i < N; i++){
	printf("%d", box[i]);
	cols++;
	if(cols == N / 3){
		printf("\n");
		cols = 0;
	}
     }
}

int main(){
    int grid[N][N];  // = grid_new();
    memset(grid,0,N*N*sizeof(int));
    int * box = get_box('d');
    for(int i = 0; i < N; i++){
	printf("%d\n",box[i]);
    }
    grid[4][2] = 9;
    int * box_values = get_box_values(grid,'d');
    print_box(box_values);

    int row = 4;
    int col = 0;
    grid[row][col] = 8;
    print_grid(grid,false);
    
    printf("%c",get_box_char(row,col));
    return 0;

}
