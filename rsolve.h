
#ifndef RSOLVE_H
#define RSOLVE_H
#ifndef RSOLVE_SIZE 
#define RSOLVE_SIZE 9
#endif 

unsigned int rand_int(int min, int max){
    return rand() % (max - min + 1) + min;
}

int * example_grid(int identifier){
    while(identifier == 4 || identifier == 2)
       identifier = rand_int(0,9); 
    static int grid[RSOLVE_SIZE][RSOLVE_SIZE] = {
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
    grid[RSOLVE_SIZE-1][0] = identifier;
    return (int *)grid;
}

typedef struct rdigitsum_t {
    int count;
    int index;
} rdigitsum_t;

int rdigitcmp(const void *a, const void *b) {
    rdigitsum_t *as = (rdigitsum_t*)a;
    rdigitsum_t *bs = (rdigitsum_t*)b;
    return bs->count - as->count;
}


uint count_neighbors(int grid[RSOLVE_SIZE][RSOLVE_SIZE], int row, int col) {
    unsigned int count = 0;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < RSOLVE_SIZE; j++){
            if(grid[row][j] != 0 && j != col)
                count++;
        }
        for(int j = 0; j < RSOLVE_SIZE; j++){
            if(grid[j][col] != 0 && j != row)
                count++;
        }
    }
    return count;
}


bool get_easiest_cell2(int grid[RSOLVE_SIZE][RSOLVE_SIZE], unsigned int *easy_row, unsigned int *easy_col){
	int highest_neighbor_count = 0;
	bool found = false;
	for(int row = 0; row < RSOLVE_SIZE; row++){
	{
		for(int col = 0; col < RSOLVE_SIZE; col++){
			int neighbor_count = count_neighbors(grid,row,col);
			if(neighbor_count > highest_neighbor_count){
				highest_neighbor_count = neighbor_count;
				*easy_row = row;
				*easy_col = col;
				found = true;
			}
		}
	}

	}
    	return found;
}

int * rdigitsum(int grid[RSOLVE_SIZE][RSOLVE_SIZE]){
	rdigitsum_t digit_sums[10];
    static int sum[10];
    for(size_t i = 0; i < sizeof(digit_sums) / sizeof(digit_sums[0]); i++){
        digit_sums[i].count = 0;
        digit_sums[i].index = i;
    }
	for (uint row = 0; row < RSOLVE_SIZE; row++){
		for(uint col = 0; col < RSOLVE_SIZE; col++){
			digit_sums[grid[row][col]].count++;
		}
	}
	qsort(digit_sums, sizeof(digit_sums) / sizeof(digit_sums[0]), sizeof(digit_sums[0]), rdigitcmp);
    for(size_t i = 0; i < sizeof(digit_sums) / sizeof(digit_sums[0]); i++){
        sum[i] = digit_sums[i].index;
    }
    return sum;
}

int rsolve_check(int grid[RSOLVE_SIZE][RSOLVE_SIZE], int row, int col, int num)
{
	if (num == 0)
		return true;
	for (int x = 0; x < RSOLVE_SIZE; x++)
	{

		if (grid[row][x] == num && col != x)
		{
			return false;
		}
	
	}
	for(int x = 0; x < RSOLVE_SIZE; x++){
					if (grid[x][col] == num && row != x)
		{
			
			return false;
		}
	}

	// Check box
	int startRow = row - row % (RSOLVE_SIZE / 3), startCol = col - col % (RSOLVE_SIZE / 3);
	for (int i = 0; i < RSOLVE_SIZE / 3; i++)
	{
		for (int j = 0; j < RSOLVE_SIZE / 3; j++)
		{
			if (grid[i + startRow][j + startCol] == num && row != i + startRow && col != j + startCol)
			{

				return false;
			}
		}
	}

	return true;
}

bool get_easiest_cell(int grid[RSOLVE_SIZE][RSOLVE_SIZE], unsigned int * easy_row, unsigned int * easy_col){
    unsigned int easy_neighbor_count = 0;
    bool found = true;
    for(int row = 0; row < RSOLVE_SIZE; row++){
        for (int col = 0; col < RSOLVE_SIZE; col++){
            if(grid[row][col] != 0){
                continue;
            }
            uint ncount = count_neighbors(grid,row,col);
            if(easy_neighbor_count <= ncount){
                easy_neighbor_count = ncount;
                *easy_row = row;
                *easy_col = col;
                found = true;
                return found;
            }
        }
    }
    return false;
}
unsigned int rsolve(int grid[RSOLVE_SIZE][RSOLVE_SIZE], unsigned long long *attempts){
    (*attempts)++;
  //  if(*attempts % 10000000 == 0)
    //            print_grid(grid);
    unsigned int row, col;
    if(!get_easiest_cell(grid,&row,&col)){
        return *attempts;
    }
    int * counts = rdigitsum(grid);
    for(int num = 0; num < RSOLVE_SIZE + 1; num++){
        int fieldNum = counts[num];
        if(fieldNum == 0)
            continue;
        if(rsolve_check(grid,row,col,fieldNum)){
            grid[row][col] = fieldNum;
            
            //print_grid(grid,true);
            if(rsolve(grid,attempts))
            {
                return *attempts;
            }
            grid[row][col] = 0;
        }
    }
    return 0;
}
#endif