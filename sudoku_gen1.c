#include "sudoku.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <rlib.h>
#include <pthread.h>
#include "footer.h"

unsigned long long global_m_lock_count = 0;
pthread_mutex_t * global_m_lock = NULL;

#define WITH_MUTEX(src,update_footer) \
    if(global_m_lock == NULL){ \
        global_m_lock = malloc(sizeof(pthread_mutex_t)); \
        pthread_mutex_init(global_m_lock, NULL); \
    } \
    pthread_mutex_lock(global_m_lock);    \
    global_m_lock_count++; \
    if(update_footer) { \
    footer_printf("l:%s:%d (%lld)",__func__,__LINE__,global_m_lock_count); \
    } \
      src \
     if(update_footer) { \
    footer_printf("u:%s:%d (%lld)",__func__,__LINE__,global_m_lock_count); \
    } \
    pthread_mutex_unlock(global_m_lock);
    

typedef struct thread_data_t {
    int puzzle[N][N];
    int solution[N][N];
    pthread_t thread;
    ulong steps;
    ulong steps_total;
    uint complexity;
    uint result_complexity;
    uint solved_count; 
    uint initial_count_minimum;
    uint initial_count_maximum;
    uint result_initial_count;
    uint id;
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
    rn = (rand() % N) + 1;
    row = rand() % N;
    col = rand() % N;
    
    while(grid[row][col] != 0){
        row = rand() % N;
        col = rand() % N;
    }
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


int * grid_with_minimal_complexity(thread_data_t * tdata){
    int * grid = grid_new();
    int * grid_game = NULL;
    while(true){
        tdata->result_initial_count = rand_int(tdata->initial_count_minimum,tdata->initial_count_maximum);
        
	grid_set_random_free_cells(grid,tdata->result_initial_count);
        //print_grid(grid,false);
        grid_game = grid_copy(grid);
        //footer_printf("Solving: %ld", tdata->result_initial_count);
        
    	tdata->start = nsecs();
	//tdata->steps = 0;
	tdata->result_complexity = rsolve(grid,&tdata->steps);
	tdata->steps_total += tdata->steps;
	tdata->steps = 0;
	tdata->solved_count++;
        if(tdata->result_complexity == 0){
            //print_grid(grid,true);
			//exit(5);
			//footer_printf("thread %d failed validation",tdata->id);
        }else{
            //footer_printf("thread %d solved: %d",tdata->id, tdata->result_complexity);
        }
	//if(tdata->solution)
	//	free(tdata->solution);
	

	
	WITH_MUTEX({
	memcpy(tdata->solution,grid,N*N*sizeof(int));
	},false);
        if(tdata->result_complexity >= tdata->complexity){
            break;
        }else{
            free(grid_game);
            grid_reset(grid);
        }
    }
    return grid_game;
}

void generate_game(thread_data_t * tdata){
    tick();
	//unsigned int * result_complexity = (int *)calloc(sizeof(int),1);
    //unsigned int * result_initial_count = (int *)calloc(sizeof(int),1);
    //rr_disable_stdout();
   
       int * puzzle =  grid_with_minimal_complexity(tdata);
    
	//if(tdata->puzzle){
		//free(tdata->puzzle);

	//}
    WITH_MUTEX({
    memcpy(tdata->puzzle,puzzle,N*N*sizeof(int));
    tdata->finish = nsecs();
    tdata->duration = tdata->finish - tdata->start;
    tdata->is_done = true;
    },false);
}

void thread_data_to_json_object(rjson_t * json ,thread_data_t * data){
	rjson_object_start(json);
	rjson_kv_int(json,"id",data->id);
	rjson_kv_int(json,"solved_count",data->solved_count);
	rjson_kv_number(json,"steps_total",data->steps_total);
	rjson_kv_number(json,"steps",data->steps);
	rjson_kv_number(json,"result_initial_count",data->result_initial_count);
	rjson_kv_duration(json,"start",data->start);
	rjson_kv_duration(json,"finish",data->finish);
	rjson_kv_duration(json,"duration",nsecs() - data->start);
	rjson_kv_string(json,"puzzle",grid_to_string(data->puzzle));
	rjson_kv_string(json,"solution",grid_to_string(data->solution));
	rjson_object_close(json);
}
char * thread_data_to_json(thread_data_t * data, int runner_count){
	rjson_t * json = rjson();
	rjson_array_start(json);
	for(int i = 0; i < runner_count; i++){
	thread_data_to_json_object(json,&data[i]);
	}
	rjson_array_close(json);
	
	char * content = strdup(json->content);
	rjson_free(json);
	return content;
	
}

char * thread_data_to_string(thread_data_t * data){
	static char result[4096];
	memset(result,0,sizeof(result));
	sprintf(result,"id:%d\ttotal solved: %d \tsteps total: %s\tsteps current: %s\tinitc: %d\ttime: %s\tDebug: %lld %lld",
		data->id,
		data->solved_count,
		rtempc(rformat_number(data->steps_total + data->steps)),
		rtempc(rformat_number(data->steps)),
		data->result_initial_count,
		format_time(nsecs() - data->start),
		data->steps_total + data->steps,
		data->steps
	);
	return result;
}

char * runner_status_to_string(thread_data_t * runners, unsigned int runner_count){
	static char result[1024*1024];
	memset(result,0,sizeof(result));
	
	for(int i = 0; i < runner_count; i++){
		strcat(result,thread_data_to_string(&runners[i]));
	       strcat(result,"\n");	
	}
	return result;

}

typedef struct serve_arguments_t {
	thread_data_t * runners;
	unsigned int runner_count;
	int port;
	nsecs_t time_winner;
	int puzzle_winner[N][N];
	int solution_winner[N][N];
	char start_timestamp[30];
} serve_arguments_t;


serve_arguments_t serve_arguments;

void get_totals(thread_data_t * runners, unsigned int runner_count, ulong * steps_total, ulong * solved_total,nsecs_t * longest_running){
	*steps_total = 0;
	*solved_total = 0;
	*longest_running = 0;
	nsecs_t end_time = nsecs();
	for(unsigned int i = 0; i < runner_count; i++){
		*steps_total += runners[i].steps_total + runners[i].steps;
		*solved_total += runners[i].solved_count;
		nsecs_t duration = runners[i].start ? end_time - runners[i].start : 0;
		if(duration > *longest_running){
			*longest_running = duration;
		}
	}
}

void http_response(rhttp_request_t * r, char * content){
	char headers[strlen(content) + 1000];
	sprintf(headers,"HTTP/1.1 200 OK\r\n"
	"Content-Length:%ld\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n\r\n%s",
	strlen(content),content);
	rhttp_send_drain(r->c,headers,0);
	close(r->c);
}

int request_handler_processes(rhttp_request_t*r){
	if(!strncmp(r->path,"/processes",strlen("/processes"))){
		WITH_MUTEX({
		char * content = thread_data_to_json(serve_arguments.runners,serve_arguments.runner_count); // runner_status_to_string(serve_arguments.runners,serve_arguments.runner_count);
		http_response(r,content);

		free(content);
		},false);
		return 1;
	}
	return 0;
}

int request_handler_root(rhttp_request_t *r){
	serve_arguments_t args = *(serve_arguments_t *)r->context;
	if(!strcmp(r->path,"/")){
	WITH_MUTEX({
		char * content = runner_status_to_string(args.runners,args.runner_count);
		ulong steps_total = 0;
		ulong solved_total = 0;
		nsecs_t longest_running = 0;
		get_totals(args.runners, args.runner_count, &steps_total, &solved_total, &longest_running);
		char html[1024*1024*2] = {0};
		sprintf(html, "<html><meta http-equiv=\"refresh\" content=\"5\"><head>"
				"<body style=\"background-color:black;color:white;\"><pre>"
				"%s"
				"Started: %s\n"
				"\n"
				"Steps total: %s\n"
				"Solved total: %s\n"
				"Steps per puzzle: %s\n"
				"Longest running: %s\n"
				"Generation time hardest puzzle: %s\n"
				"\n"
				"Hardest puzzle:\n"
				"%s\n"
				"Solution:\n"
				"%s\n"
				"</pre>"
				"</body></html>",
				content, 
				args.start_timestamp,
				rtempc(rformat_number(steps_total)),
				rtempc(rformat_number(solved_total)),
				rtempc(rformat_number(solved_total != 0 ? steps_total / (solved_total + args.runner_count) : 0)),
				rtempc(format_time(longest_running)),
				rtempc(format_time(args.time_winner)),
				rtempc(grid_to_string(args.puzzle_winner)), 
				rtempc(grid_to_string(args.solution_winner))
		);
		char response[1024*1024*3];
		memset(response,0,sizeof(response));
		sprintf(response,"HTTP/1.1 200 OK\r\n"
				"Content-Length: %d\r\n"
				"Content-Type: text/html\r\n"
				"Connection: close:\r\n\r\n%s",
				strlen(html),html);
		rhttp_send_drain(r->c, response,0);
	},true);
	return 1;
	}
	return 0;
}

int request_handler_empty(rhttp_request_t * r){
	if(!strncmp(r->path,"/empty",strlen("/empty"))){
		int grid[N][N];
		memset(grid,0,N*N*sizeof(int));
		char * content = grid_to_string(grid);
		char response[1024];
	       response[0] = 0;
       	        sprintf(
			response, 
			"HTTP/1.1 200 OK\r\nContent-Length:%ld\r\nConnection: close\r\n\r\n%s",
			strlen(content),
			content
		);	
		rhttp_send_drain(r->c,response,0);
		return 1;
	}
	return 0;
}

int request_handler_404(rhttp_request_t *r){
	char content[] = "HTTP/1.1 404 Document not found\r\nContent-Length:3\r\nConnection:close\r\n\r\n404";
	rhttp_send_drain(r->c,content,0);
	return 1;
};

int request_handler(rhttp_request_t * r){
	rhttp_request_handler_t request_handlers[] ={
		request_handler_root,
		request_handler_empty,
		request_handler_processes,
		rhttp_file_request_handler,
		request_handler_404,
		NULL 
	};
	int i = -1;
	while(request_handlers[++i])
		if(request_handlers[i](r))
			return 1;
	return 0;
}


void * serve_thread(void *arg){
	serve_arguments_t * arguments = (serve_arguments_t *)arg;
	
	rhttp_serve("0.0.0.0",arguments->port,1024,1,1,request_handler,(void *)arguments);
	return NULL;
}

unsigned int generate_games(unsigned int game_count, unsigned int timeout, unsigned int complexity){
    pthread_t thread_serve; 

    
    thread_data_t runners[game_count];
    serve_arguments.runners = runners;
    serve_arguments.runner_count = game_count;
    serve_arguments.port = 9999;
    serve_arguments.time_winner = 0;
    strcpy(serve_arguments.start_timestamp,rstrtimestamp());
    memset(serve_arguments.solution_winner,0,sizeof(serve_arguments.solution_winner));
    memset(serve_arguments.puzzle_winner,0,sizeof(serve_arguments.puzzle_winner));
    
    pthread_create(&thread_serve,0,serve_thread,(void *)&serve_arguments);
    //pthread_mutex_t lock;
    //pthread_mutex_init(&lock,NULL);
    for(unsigned int i = 0; i < game_count; i++){
        runners[i].initial_count_maximum = 30;
        runners[i].initial_count_minimum = 1;
        runners[i].complexity = complexity;
        runners[i].is_done = false;
        runners[i].id = i;
	memset(runners[i].solution,0,N*N*sizeof(int));
	memset(runners[i].puzzle,0,N*N*sizeof(int));
        runners[i].solved_count = 0;
	runners[i].duration = 0;
	runners[i].steps = 0;
	runners[i].steps_total = 0;
	//runners[i].lock = lock;
        pthread_create(&runners[i].thread,NULL,generate_game,(void *)(&runners[i]));
    }
    unsigned int highest_complexity = complexity;
    nsecs_t time_start = nsecs();
    for(unsigned int i = 0; i < timeout; i++){
	sleep(1);
	WITH_MUTEX({
        nsecs_t time_elapsed = nsecs() - time_start;
        
        footer_printf("main");
        //pthread_mutex_lock(&lock);
        for(int ithread = 0; ithread < game_count; ithread++){
            if(runners[ithread].is_done){
                pthread_join(runners[ithread].thread,NULL);
               		
		if(runners[ithread].result_complexity > highest_complexity){
         	highest_complexity = runners[ithread].result_complexity; 
                  	for(int j = 0; j < game_count; j++){
                        runners[j].complexity = highest_complexity;
                    
			}

 			printf("\r\n");
			print_grid(runners[ithread].puzzle,true);
			printf("\n");
			print_grid(runners[ithread].solution,false);
               
	
			memcpy(serve_arguments.puzzle_winner,runners[ithread].puzzle,N*N*sizeof(int));
			memcpy(serve_arguments.solution_winner,runners[ithread].solution,N*N*sizeof(int));
			serve_arguments.time_winner = runners[ithread].duration;


               	printf("Thread %d is done (%s)\n",ithread,format_time(runners[ithread].duration));
                	printf("Complexity: %ld\n",runners[ithread].result_complexity);
                	printf("Initial values: %ld\n",runners[ithread].result_initial_count);
                
                }
                runners[ithread].is_done = false;
                //free(runners[ithread].puzzle);
                //runners[ithread].puzzle = NULL;
                //free(runners[ithread].solution);
                //runners[ithread].solution = NULL;
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
    setbuf(stdout,NULL);

    srand(time(NULL));
   // setbuf(stdout,0);
    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    int threads = cores * 4 - 1;
    sprintf(footer_prefix, "Cores: %d - Threads: %d - ", cores,threads);
    // Highest: 1481563980
    generate_games(threads ,13371337,1);
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
/*0 3 0 0 0 0 0 0 0 
0 0 5 0 0 0 0 0 0 
0 0 0 0 0 0 1 0 0 
0 0 0 0 0 0 0 4 0 
0 0 0 0 9 0 0 0 0 
0 0 9 0 3 0 0 0 0 
0 0 0 0 0 6 0 0 0 
0 0 4 0 0 0 0 7 0 
0 6 7 0 0 4 0 2 0 
Thread 0 is done (2635.64s)
Complexity: 1481563980
Initial values: 14*/
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
    int grid_empty[N][N] = {
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
    
}
