#include "rlib.h"
#include "sudoku.h"

#include <pthread.h>
char  * xx  = "aa";

void * thread_worker(){
	tick();


	for(uint i = 0; i< 1000; i++){

		grid_t * grid = new_grid(18);
		grid_to_string_c(grid);
//		printf("%d\n",i);
		sbuf(xx);
		//strcpy(xx,"AAA");
	}
	return NULL;
}

void bench_rtemp_c(){
	grid_t * grid = new_grid(18);
	char * to_copy = sbuf(grid_to_string_c(grid));

	uint times_bench = 10;
	uint times_function = 10000000;
	printf("Benchmark %d*%d with/without thread lock.\n",times_bench,times_function);
	RBENCH(times_bench,{
		printf("with:   ");
		RBENCH(times_function,{
			sbuf(sbuf(sbuf(to_copy)));
		});
	
		rtempc_use_mutex = false;
		printf("witout: ");
		RBENCH(10000000,{
			sbuf(sbuf(sbuf(to_copy)));
		});
	});
	
}

int main(){
	//rtempc_use_mutex = false;
	bench_rtemp_c();
	rtempc_use_mutex = true;
	uint worker_count = 4;
	pthread_t workers[worker_count];
	printf("Threading: \n");
	RBENCH(1,{
		for(uint i = 0; i < worker_count; i++){
			pthread_create(&workers[i],0,thread_worker,NULL);
		}
		for(uint i = 0; i < worker_count; i++){
			pthread_join(workers[i],NULL);
		}
	});
	printf("Without threading: \n");
	RBENCH(1,{
		for(uint i = 0; i < worker_count; i++){
			thread_worker();
		}
	});
	return 0;
}
