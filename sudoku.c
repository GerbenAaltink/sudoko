#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define PUZZLE_ROWS 9
#define PUZZLE_COLS 9
#define PUZZLE_SIZE PUZZLE_ROWS*PUZZLE_COLS

#define ANSII_BLUE_BG "\033[44m"
#define ANSII_RED_BG "\033[41m"
#define ANSII_GREEN_BG "\033[42m"
#define ANSII_CLEAR "\033[0m"

typedef struct sudoku_cell_t
{
    char c;
    bool predefined;
    bool sure;
    bool empty;
} sudoku_cell_t;

sudoku_cell_t *get_cell(sudoku_cell_t *puzzle, unsigned int row, unsigned int column)
{
    sudoku_cell_t *cell = &puzzle[row * PUZZLE_COLS + column];
    return cell;
}

void set_cell(sudoku_cell_t *puzzle, unsigned int row, unsigned int column, bool predefined, bool sure, char c)
{
    sudoku_cell_t *cell = get_cell(puzzle, row, column);
    cell->predefined = predefined;
    cell->sure = predefined ? true : sure;
    cell->c = c;
    cell->empty = c == 0;
}
void set_cellc(sudoku_cell_t *puzzle, char column, unsigned int row, bool predefined, bool sure, char c)
{
    unsigned int irow = row - 1;
    unsigned int icol = column - 65;
    set_cell(puzzle, irow, icol, predefined, sure, c);
}
void set_predefinedc(sudoku_cell_t *puzzle, char row, unsigned int column, char c)
{
    set_cellc(puzzle, row, column, true, true, c);
}
void set_sure(sudoku_cell_t *puzzle, char row, unsigned int column, char c)
{
    set_cell(puzzle, row, column, false, true, c);
}
void set_guess(sudoku_cell_t *puzzle, char row, unsigned int column, char c)
{
    set_cell(puzzle, row, column, false, false, c);
}
void set_guessc(sudoku_cell_t *puzzle, char row, unsigned int column, char c)
{
    set_cellc(puzzle, row, column, false, false, c);
}
void set_zero(sudoku_cell_t *puzzle, char row, unsigned int column)
{
    set_cellc(puzzle, row, column, false, false, 0);
}

void init_puzzle(sudoku_cell_t *puzzle, unsigned int rows, unsigned int cols)
{
    for (unsigned int i = 0; i < rows * cols; i++)
    {
        puzzle[i].c = 0;
        puzzle[i].empty = true;
        puzzle[i].predefined = false;
        puzzle[i].sure = true;
    }
}

void draw_cell(sudoku_cell_t *cell)
{
    if (cell->predefined)
    {
        printf("%s", ANSII_BLUE_BG);
    }
    else if (cell->empty)
    {
        printf("%s", ANSII_CLEAR);
    }
    else if (cell->sure)
    {
        printf("%s", ANSII_GREEN_BG);
    }
    else if (!cell->empty)
    {
        printf("%s", ANSII_RED_BG);
    }
    printf(" %c ", cell->c == 0 ? ' ' : cell->c);
    printf("%s", ANSII_CLEAR);
}

void draw_puzzle(sudoku_cell_t *puzzle, unsigned int rows, unsigned int cols)
{
    for (unsigned int i = 0; i < cols; i++)
    {
        for (unsigned int j = 0; j < rows; j++)
        {

            sudoku_cell_t *cell = &puzzle[i * cols + j];
            draw_cell(cell);
        }
        printf("\n");
    }
}

sudoku_cell_t *get_col(sudoku_cell_t *puzzle, unsigned int rows, unsigned int col)
{
    static sudoku_cell_t box[4096];
    for (unsigned int i = 0; i < rows; i++)
    {
        box[i] = *get_cell(puzzle, i, col);
    }
    return box;
}

sudoku_cell_t *get_row(sudoku_cell_t *puzzle, unsigned int cols, unsigned int row)
{
    static sudoku_cell_t box[4096];
    for (unsigned int i = 0; i < cols; i++)
    {
        box[i] = *get_cell(puzzle, row, i);
    }
    return box;
}

sudoku_cell_t *get_box(sudoku_cell_t *puzzle, unsigned int rows, unsigned int cols, unsigned int row, unsigned int col)
{
    unsigned int col_box_column = col / 3;

    unsigned int row_box_row = row / 3;

    static sudoku_cell_t box[4096];
    unsigned int counter = 0;

    for (unsigned int i = row_box_row * (rows / 3); i < (row_box_row * rows / 3) + (rows / 3); i++)
    {
        for (unsigned int j = col_box_column * (cols / 3); j < (col_box_column * cols / 3) + (cols / 3); j++)
        {
            box[counter] = *get_cell(puzzle, i, j);
            counter++;
        }
    }
    return box;
}

unsigned int *get_options(sudoku_cell_t *puzzle, unsigned int rows, unsigned int cols, unsigned int row, unsigned int col)
{
    static unsigned int options[4096];
    memset(options, 0, sizeof(options));
    for (unsigned int i = 0; i < cols; i++)
    {
        options[i] = true;
    }
    for (unsigned int i = 0; i < cols; i++)
    {
        sudoku_cell_t *c = get_col(puzzle, rows, col);
        for (unsigned int j = 0; j < cols; j++)
        {
            if (!c[j].empty)
                options[c[i].c - '0' - 1] = false;
        }
        c = get_row(puzzle, cols, row);
        for (unsigned int j = 0; j < cols; j++)
        {
            if (!c[j].empty)
                options[c[j].c - '0' - 1] = false;
        }
        c = get_box(puzzle, rows, cols, row, col);
        for (unsigned int j = 0; j < cols * cols; j++)
        {
            if (!c[j].empty)
                options[c[j].c - '0' - 1] = false;
        }
    }
    return options;
}

unsigned int get_option(unsigned int *options, unsigned int offset)
{
    unsigned int result = 0;
    unsigned int current_offset = 0;
    for (unsigned int i = 0; i < PUZZLE_COLS; i++)
    {
        if (options[i])
        {
            if (current_offset == offset)
            {
                return i;
            }
            offset++;
        }
    }
    return result;
}

unsigned int get_options_count(unsigned int *options)
{
    unsigned int result = 0;
    for (unsigned int i = 0; i < PUZZLE_COLS; i++)
    {
        if (options[i])
        {
            result++;
        }
    }
    return result;
}

unsigned int count_empty(sudoku_cell_t * puzzle, unsigned int size){
    unsigned int count = 0;
    for(unsigned int i = 0; i < size;i++){
        if(puzzle[i].empty){
            count++;
        }
    }
    return count;
}

unsigned int set_sures(sudoku_cell_t *puzzle, unsigned int rows, unsigned int cols, unsigned int accuracy)
{

    for (unsigned int i = 0; i < rows; i++)
    {
        for (unsigned int j = 0; j < cols; j++)
        {
            sudoku_cell_t *cell = get_cell(puzzle, i, j);
            unsigned int *options = get_options(puzzle, rows, cols, i, j);
            if (cell->empty && get_options_count(options) == accuracy)
            {
                unsigned int option = get_option(options, 0);
                if (accuracy > 1)
                {
                    set_guess(puzzle, i, j, option + '0' + 1);
                }
                else
                {
                    set_sure(puzzle, i, j, option + '0' + 1);
                }
                set_sures(puzzle, rows, cols, accuracy);
                return count_empty(puzzle,rows*cols);
            }
        }
    }
    return count_empty(puzzle,rows*cols);
}

unsigned int solve(char *charpuzzle)
{
    sudoku_cell_t puzzle[PUZZLE_SIZE];
    init_puzzle(puzzle, PUZZLE_ROWS, PUZZLE_COLS);
    for (unsigned int i = 0; i < PUZZLE_ROWS; i++)
    {
        for (unsigned int j = 0; j < PUZZLE_COLS; j++)
        {
            if (charpuzzle[j * PUZZLE_COLS + i] != ' ')
                set_predefinedc(puzzle, i + 65, j + 1, charpuzzle[j * PUZZLE_COLS + i]);
        }
    }
    set_sures(puzzle, PUZZLE_ROWS, PUZZLE_COLS, 1);
    unsigned int result = set_sures(puzzle, PUZZLE_ROWS, PUZZLE_COLS, 2);
    if(result){
        printf("Unresolved. %d cells left:\n",result);
    }else{
        printf("Succesfully resolved:\n");
    }
    draw_puzzle(puzzle, PUZZLE_ROWS, PUZZLE_COLS);
    return result;
}

int main()
{
    solve(
        "913   5  "
        "6 7    24"
        " 5  8  7 "
        " 79      "
        "  2 9  43"
        "     4 9 "
        " 4   19  "
        "7 6  9  5"
        "  1  64 7");
    printf("\n");
    solve(
        "5 3   7  "
        "6    195 "
        " 98    6 "
        "8   6   3"
        "4  8  3  "
        "7   2   6"
        " 6    28 "
        " 419    5"
        "   8   79");
    printf("\n");
    solve(
        " 4  8 3  "
        "  1 9   5"
        "7   2 6  "
        " 5  7   2"
        "    4    "
        "9   5  7 "
        "  2 6   8"
        "6   3  1 "
        "  8 7  9 ");
    printf("\n");
    solve(
        "  7429 8 "
        "2    83  "
        "896 351 4"
        " 1   6 9 "
        "7   1 2 6"
        "6  973  1"
        "       6 "
        "5 8 9    "
        "46 58  1 ");
    printf("\n");
    solve(
        "2  5 74 6"
        "    31   "
        "      23 "
        "    2    "
        "86 31    "
        " 45      "
        "  9   7  "
        "  695   2"
        "  1  6  8");

    printf("\n");
    return 0;
}