#include <rlib.h>
#include <pthread.h>

int request_handler(rhttp_request_t * r) {
	rhttp_send_drain(r->c, "HTTP/1.1 200 OK\r\n"
			"Content-Length: 1\r\n"
			"Connection: close\r\n\r\n"
			"A",0);
	close(r->c);
}

void * serve_thread(void * arg){
	int port = *(int*)arg;
	printf("Serving on port: %d\n", port);
	rhttp_serve("0.0.0.0", port, 1024, 1, 1, request_handler);
	return NULL;
}

pthread_t server_thread;
void serve(int port){
	printf("Serving on port: %d\n", port);
	int pport = port;

}

int main() {
	int port = 8889;
	//serve(port);;
	
	
	pthread_create(&server_thread, 0, serve_thread,(void *)&port);
	pthread_join(server_thread, NULL);
	while(true){
		printf("Loop\n");
		sleep(1);
	}
	return 0;
}

