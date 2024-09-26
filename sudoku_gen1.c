#include "sudoku.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <rlib.h>
#include <pthread.h>
#include "footer.h"

unsigned int global_m_lock_count = 0;
pthread_mutex_t * global_m_lock = NULL;

#define WITH_MUTEX(src,update_footer) \
    if(global_m_lock == NULL){ \
        global_m_lock = malloc(sizeof(pthread_mutex_t)); \
        pthread_mutex_init(global_m_lock, NULL); \
    } \
    pthread_mutex_lock(global_m_lock);    \
    global_m_lock_count++; \
    if(update_footer) { \
    sprintf(footer_buffer,"l:%s:%d (%ld)",__func__,__LINE__,global_m_lock_count); \
    set_footer_text(footer_buffer); \
    } \
      src \
     if(update_footer) { \
    sprintf(footer_buffer,"u:%s:%d (%ld)",__func__,__LINE__,global_m_lock_count); \
    set_footer_text(footer_buffer); \
    } \
    pthread_mutex_unlock(global_m_lock);
    
typedef struct thread_data_t {
    int * puzzle;
    int * solution;
    pthread_t thread;
    unsigned int complexity;
    unsigned int result_complexity;
    unsigned int initial_count_minimum;
    unsigned int initial_count_maximum;
    unsigned int result_initial_count;
    unsigned int id;
    pthread_mutex_t lock;
    bool is_done;
    nsecs_t start;
    nsecs_t finish;
    nsecs_t duration;
} thread_data_t;

thread_data_t * thread_data_create(){
    thread_data_t * data = malloc(sizeof(thread_data_t));
    memset(data,0,sizeof(thread_data_t));
    return data;
}
void thread_data_free(thread_data_t * data){
    if(data->puzzle){
        free(data->puzzle);
    }
    if(data->solution){
        free(data->solution);
    }
    free(data);
}

void grid_set_random_free_cell(int grid[N][N]){
    int rn, row, col;
    
    while(true){
    WITH_MUTEX({
    rn = (rand() % N) + 1;
    row = rand() % N;
    col = rand() % N;
    
    while(grid[row][col] != 0){
        row = rand() % N;
        col = rand() % N;
    }
    },false);
   // printf("CHECK %d:%d:%d\n",row,col,rn);
        if(is_safe(grid,row,col, rn)){
           // printf("CHECKED\n");
             grid[row][col] = rn;
             break;
        }
    }
    
}
void grid_set_random_free_cells(int grid[N][N], unsigned int count){
    //grid[rand() % N][rand() % N] = rand() % N;
    for (int i =0 ; i < count; i++){
        grid_set_random_free_cell(grid);
        //print_grid(grid,true);
        //printf("Generation: %d/%d\n", i + 1, count);
    }
}


int * grid_with_minimal_complexity(unsigned int complexity, unsigned int * result_complexity, unsigned int * initial_count){
    int * grid = grid_new();
    int * grid_game = NULL;
    while(true){
        *initial_count = rand_int(1,36);
        grid_set_random_free_cells(grid,*initial_count);
        //print_grid(grid,false);
        grid_game = grid_copy(grid);
        char solving_text[1024] = {0};
        sprintf(solving_text,"Solving: %ld", *result_complexity);
        set_footer_text(solving_text);
        *result_complexity = solve(grid,false);
        if(*result_complexity >= complexity){
            break;
        }else{
            
            grid_reset(grid);
        }
    }
    free(grid);
    return grid_game;
}

void generate_game(thread_data_t * tdata){
    //unsigned int * result_complexity = (int *)calloc(sizeof(int),1);
    //unsigned int * result_initial_count = (int *)calloc(sizeof(int),1);
    //rr_disable_stdout();
   sleep(3);
   WITH_MUTEX({
    tdata->start = nsecs();


},false);
       tdata->puzzle = grid_with_minimal_complexity( 
tdata->complexity, &tdata->result_complexity, &tdata->result_initial_count);

    WITH_MUTEX({
    tdata->finish = nsecs();
    tdata->duration = tdata->finish - tdata->start;
    tdata->is_done = true;
    },false);
}

unsigned int generate_games(unsigned int game_count, unsigned int timeout, unsigned int complexity){
    thread_data_t runners[game_count];
    //pthread_mutex_t lock;
    //pthread_mutex_init(&lock,NULL);
    for(unsigned int i = 0; i < game_count; i++){
        runners[i].initial_count_maximum = 30;
        runners[i].initial_count_minimum = 17;
        runners[i].complexity = complexity;
        runners[i].is_done = false;
        runners[i].id = i;
        //runners[i].lock = lock;
        pthread_create(&runners[i].thread,NULL,generate_game,(void *)(&runners[i]));
    }
    unsigned int highest_complexity = complexity;
    nsecs_t time_start = 0;
    WITH_MUTEX({
        time_start = nsecs();
    },true);
    for(unsigned int i = 0; i < timeout; i++){
        sleep(1);
        WITH_MUTEX({
        nsecs_t time_elapsed = nsecs() - time_start;
        char status_text[1024] = {0};
        sprintf(status_text,"\rRunning %s",format_time(time_elapsed));
        set_footer_text(status_text);
        //pthread_mutex_lock(&lock);
        for(int ithread = 0; ithread < game_count; ithread++){
            if(runners[ithread].is_done){
                pthread_join(runners[ithread].thread,NULL);
                printf("\r");
                print_grid(runners[ithread].puzzle,true);
                if(runners[ithread].result_complexity > highest_complexity){
                    highest_complexity = runners[ithread].result_complexity; 
                    for(int j = 0; j < game_count; j++){
                        runners[j].complexity = highest_complexity;
                    }
                }
                printf("Thread %d is done (%s)\n",ithread,format_time(runners[ithread].duration));
                printf("Complexity: %ld\n",runners[ithread].result_complexity);;
                printf("Initial values: %ld\n",runners[ithread].result_initial_count);
                runners[ithread].is_done = false;
                free(runners[ithread].puzzle);
                printf("\n");
                pthread_create(&runners[ithread].thread,NULL,generate_game,(void *)(&runners[ithread]));
            
            }
        }   
        //pthread_mutex_unlock(&lock);
    },true);
    }
    
    for(unsigned int i = 0; i < game_count; i++){
        
        pthread_cancel(runners[i].thread);
        
    }
    pthread_testcancel();
}

int main() {
    srand(time(NULL));
   // setbuf(stdout,0);
    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    int threads = cores - 1;
    sprintf(footer_prefix, "Cores: %d - Threads: %d - ", cores,threads);
    generate_games(threads ,3600,100000);
    exit(0);

/*
0 0 9 0 0 0 0 0 0 
0 0 0 0 0 0 3 0 0 
6 0 1 5 0 2 0 0 0 
0 0 0 0 0 0 0 0 0 
0 0 0 0 0 8 0 0 0 
9 0 4 7 0 0 0 0 5 
0 0 7 8 0 0 0 0 0 
0 0 8 0 0 0 0 0 0 
0 4 0 0 3 0 0 6 2

Attempts: 
213212476


*/
/*
0 0 0 0 0 0 0 0 0 
0 0 0 5 0 0 0 0 0 
0 0 0 7 0 2 0 0 0 
0 0 0 0 0 1 0 0 0 
0 0 4 0 0 0 0 9 3 
0 0 0 0 0 8 0 0 0 
0 0 0 4 0 5 0 0 0 
0 0 0 0 0 0 0 0 2 
0 0 8 3 0 0 0 0 5 
Thread 2 is done (459.04s)
Complexity: 125004041
Initial values: 14
*/
/*
0 0 0 0 0 0 0 0 0 
0 8 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 6 5 0 
0 0 4 0 0 0 9 0 0 
0 0 0 2 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 
0 0 0 0 4 0 0 9 1 
0 0 0 0 6 5 0 0 0 
Thread 2 is done (1528.38s)
Complexity: 748250144
Initial values: 11
*/
/*
GROOTSTE!   
Complexity: 774858414
0 0 0 4 0 0 9 0 0 
0 0 0 0 0 0 0 0 0 
0 2 0 5 0 8 0 0 0 
5 0 2 0 0 0 0 0 3 
0 0 0 0 0 0 0 0 0 
0 7 0 0 0 0 0 0 0 
0 9 0 0 0 0 4 3 0 
2 0 0 0 5 7 0 6 0 
0 0 7 1 0 0 2 0 0 
Generated in: 1393.95s
*/
/*
ONEINDIG?
0 2 0 0 0 0 0 0 0 
0 0 4 0 0 0 0 0 0 
6 0 1 8 0 0 0 0 0 
3 0 6 0 0 0 8 0 0 
0 0 0 0 3 0 2 4 0 
0 4 0 1 0 2 5 0 0 
7 0 0 2 0 0 1 6 8 
0 3 0 0 0 0 7 2 4 
8 0 0 0 0 6 0 0 5
*/

/*
0 0 0 0 3 7 0 0 0 
0 0 8 0 0 2 5 0 3 
0 2 0 8 4 0 0 0 0 
0 0 6 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 7 
0 0 0 2 0 0 0 6 0 
0 0 0 0 0 0 6 0 5 
0 0 0 6 8 0 0 4 0 
4 0 0 1 0 0 0 0 0 
*/
    int grid[N][N] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    set_random_free_cells(grid,9);
   // unsigned int attempts = solve(grid);
   // printf("Matches: %d\n",attempts);
    int * test_grid = grid_copy(grid);
    print_grid(test_grid,true);
   // exit(0);
    unsigned int attempts = solve(grid,false);
    print_grid(grid,false);
    printf("Attempts: %d\n",attempts);
    print_grid(test_grid,false);
    
}