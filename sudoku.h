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

unsigned int rand_int(int min, int max){
    return rand() % (max - min + 1) + min;
}

int * example_grid(int identifier){
    while(identifier == 4 || identifier == 2)
       identifier = rand_int(0,9); 
    static int grid[N][N] = {
        {4, 2, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 9, 0, 0},
        {0, 0, 0, 3, 0, 0, 0, 8, 0},
        {0, 0, 0, 0, 3, 0, 0, 0, 4},
        {0, 0, 0, 0, 0, 7, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 4, 2, 0}
    };
    grid[N-1][0] = identifier;
    return &grid;
}

unsigned int count_neighbors(int grid[N][N], int row, int col) {
    unsigned int count = 0;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < N; j++){
            if(grid[row][j] != 0 && j != col)
                count++;
        }
        for(int j = 0; j < N; j++){
            if(grid[j][col] != 0 && j != row)
                count++;
        }
    }
    return count;
}

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
            if(grid[row][col] != 0){
                continue;
            }
            unsigned ncount = count_neighbors(grid,row,col);
            if(easy_neighbor_count <= ncount){
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

unsigned int _solve(int grid[N][N], int *attempts, bool draw){
    (*attempts)++;
    unsigned int row, col;
    if(!get_easiest_cell(grid,&row,&col)){
        //print_grid(grid, false);
        return attempts;
    }
    bool found = false;
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

unsigned int _solve2(int grid[N][N], int * attempts, bool draw) {
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
    int attempts = 0;
    return _solve2(grid,&attempts, draw);
}

unsigned int solve(int grid[N][N],bool draw) {
    int attempts = 0;
    return _solve(grid,&attempts, draw);
}
