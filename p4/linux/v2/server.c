#include "cs537.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too

//#define DEBUGp
//#define DEBUGw
/******************************************************
 * Wrappers used
 * ***************************************************/
void Pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg){
	int rc =	pthread_create(thread, attr, start_routine, arg);
	if(rc<0){
		fprintf(stderr, "Error creting the thread");
		exit(1);
	}
}

void Pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
	int rc = pthread_mutex_init(mutex, attr);
	if(rc < 0){
		fprintf(stderr, "Error Initialising Mutex lock");
		exit(1);
	}
}

void Pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr){
	int rc = pthread_cond_init(cond,attr);
	if(rc < 0){
		fprintf(stderr, "Error initialising pthread conditional variable");
		exit(1);
	}
}

void Pthread_mutex_lock(pthread_mutex_t *mutex){
 int p = pthread_mutex_lock(mutex);
 if(p<0){
   fprintf(stderr, "Lock not obtained..\n");
   exit(1);
 }
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex){
 int p = pthread_mutex_unlock(mutex);
 if(p<0){
   fprintf(stderr, "Unlock could not be performed\n");
   exit(1);
 }
}

void Pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex){
 int p = pthread_cond_wait(cond, mutex);
 if(p<0){   
     fprintf(stderr, "Cond wait could not be performed\n");
     exit(1);
 } 
}     
   
void Pthread_cond_signal(pthread_cond_t *cond){
 int p = pthread_cond_signal(cond);
 if(p<0){ 
     fprintf(stderr, "Cond signal could not be performed\n");
     exit(1);
 }
}

typedef enum {FIFO, SFNF, SFF} algo_t;

void getargs(int *port, int *threads, int *buffers, algo_t *schedalg, int argc, char *argv[])
{
    if (argc != 5) {
			fprintf(stderr, "Usage: %s <port> <threads> <buffers> <schedalg>\n", argv[0]);
			exit(1);
    }
		
		*port = atoi(argv[1]);
		*threads = atoi(argv[2]);
		*buffers = atoi(argv[3]);
		
		if(strcmp("FIFO",argv[4])==0)
				*schedalg = FIFO;
				else if(strcmp("SFF",argv[4])==0)
					*schedalg = SFF;
					else if(strcmp("SFNF",argv[4])==0)
						*schedalg = SFNF;
						else{
						fprintf(stderr, "Wrong value for scheduling policy\n");
						exit(1);
						}
	  	
		if(*port<1200){
			fprintf(stderr, "Port number must be more than 1200\n");
			exit(1);
		}
		if(*threads<1){
			fprintf(stderr, "Threads must be more than 0\n");
			exit(1);
		}
		if(*buffers<1){
			fprintf(stderr,"Buffers must be more than 0\n");
			exit(1);
		}

		#ifdef DEBUG
			printf("Arguments received: Port = %d, Threads = %d, Buffers= %d, Sched Algo = %d\n", *port, *threads, *buffers, *schedalg);
		#endif
}

//Global Variables

int buffer_size=0;
int* connfd;
volatile int tail, service=0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER, full = PTHREAD_COND_INITIALIZER;
algo_t sched_alg;																	//this is enumerated variable to decide the scheduling algorithm

typedef struct{
	int fd;
	char* filename;
	int filesize;
}reqfile_t;
reqfile_t *req;

void dump(){
	int k;
	for(k=0; k < tail; k++){
		printf("DUMP: %s\t	%d\n", req[k].filename, req[k].filesize);
	}
}

int do_get(){
	int tmp =0;
	int itr=0,index;
	switch(sched_alg){
		case FIFO:
			#ifdef DEBUGw
				printf("Worker: Selected FIFO algorithm\n");
			#endif
			index = 0;
			break;
		
		case SFNF:	
			#ifdef DEBUGw
				printf("Worker: Selected SFNF algorithm, tail=%d\n", tail);
			#endif
				index =0;
			for(itr=0; itr < tail; itr++){
				#ifdef DEBUGw
					printf("Worker: Comparing %s and %s\n", req[itr].filename,req[index].filename);
				#endif
				if(strlen(req[itr].filename) <= strlen(req[index].filename))
					index = itr;
			}
			break;
		
		case SFF:
			#ifdef DEBUGw
				printf("Worker: Selected SFF algorithm, tail:%d, tmp:%d\n", tail, tmp);
			#endif
				index=0;
			for(itr=0; itr < tail; itr++){
				if(req[itr].filesize <= req[index].filesize){
					#ifdef DEBUGw
						printf("Worker: Comparing %s(%d) and %s(%d)\n", req[itr].filename,req[itr].filesize,req[index].filename, req[itr].filesize);
					#endif
					index = itr;
				}
			}
			break;
		
		default:
			fprintf(stderr,"Unkonown algorithm to select the scheduling\n");
			exit(0);
	}
	tmp = connfd[index];
	#ifdef DEBUGw
		dump();
		printf("Worker: Selected %s(%d)\n", req[index].filename, req[index].filesize);
	#endif

	for(itr=index; itr < (tail-1); itr ++){
		connfd[itr] = connfd[itr+1];
	}
	#ifdef DEBUGw
		printf("Worker: After shifting, Selected %s\n", req[index].filename);
	#endif
	tail--;
	return tmp;
}

void* worker_routine(void* argv){
	while(1){
		Pthread_mutex_lock(&m);
		#ifdef DEBUGw
			printf("Worker: Acquired LOCK\n");
		#endif
			
		while(tail==0){
			#ifdef DEBUGw
				printf("Worker: Going to sleep, tail=%d\n", tail);
			#endif
			Pthread_cond_wait(&full,&m);
		}

		int filedesc = do_get();	
		#ifdef DEBUGw
			printf("Worker: Woke up and handling the request, tail=%d\n", tail);
		#endif
		
		Pthread_cond_signal(&empty);
		service++;
		#ifdef DEBUGw
			printf("Worker: Number of requests serviced = %d\n\n", service);
		#endif
		
		Pthread_mutex_unlock(&m);
		requestHandle(filedesc);
		Close(filedesc);
	}
}

void parseFile(int fd){
		char* buf = (char*) malloc(1000);
		//int rc = read(fd, buf, sizeof(buf));
		int rc = recv(fd, buf, 1000 * sizeof(char), MSG_PEEK);
		#ifdef DEBUG
			printf("Producer: File Content: %s", buf);
		#endif

		if(rc < 0)
			fprintf(stderr,"Error opening the connfd");
		strtok(buf, " ?");
		//req->filename = (char*) malloc(100*sizeof(char));
		//req = (reqfile_t*)malloc(sizeof(*req));

		req[tail].filename = strtok(NULL, "/ ?");
		//req[tail].filename = strcat(".", req[tail].filename);
		req[tail].fd = fd;
		
		struct stat filestat;
		int sd = stat(req[tail].filename, &filestat);
		if(sd<0) 
			fprintf(stderr,"Error stating file %s\n", req[tail].filename);
		req[tail].filesize = filestat.st_size;
		
		#ifdef DEBUGp
			printf("Producer: File Name received is %s, SIZE:%d\n", req[tail].filename, req[tail].filesize);
		#endif
		//Free the temporary variable buffer
		free(buf);
}


int main(int argc, char *argv[])
{
  setbuf(stdout, NULL);
	int listenfd, clientlen, num_threads;
  struct sockaddr_in clientaddr;
	int port;
	//initialize tail =0;
	tail =0;
	//Arguments are parsed with this function
	getargs(&port, &num_threads, &buffer_size, &sched_alg, argc, argv);
  
	req = (reqfile_t*)malloc(buffer_size * sizeof(reqfile_t));	
	connfd = (int*) malloc(buffer_size * sizeof(int));			//Array that stores the file descriptor of the request
	pthread_t worker_id[num_threads];												//we want to create number of worker threads
  
	int i;
	for(i=0; i<num_threads; i++)																		//Create and deploy worker threads
		Pthread_create(&worker_id[i], NULL,(void*) worker_routine, NULL);
	
	#ifdef DEBUGp
		printf("Producer: Created %d worker threads \n", num_threads);
	#endif
	
	listenfd = Open_listenfd(port);
 
	//This the producer routine of the producer-consumer solution 
	while (1) {
		clientlen = sizeof(clientaddr);
		int dummy = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
	  #ifdef DEBUGp
			printf("Producer: Received a request from client. Acquiring a lock now, tail=%d\n", tail);
		#endif
		
		Pthread_mutex_lock(&m);
		#ifdef DEBUGp
			printf("Producer: Acquired LOCK\n");
		#endif
		while(tail >= buffer_size){				//Producer will go to sleep when buffer is full and waits for emptry signal
			Pthread_cond_wait(&empty, &m);
		}
		#ifdef DEBUGp
			printf("Producer: Parsing file to PraseFile routine\n");
		#endif
	
		parseFile(dummy);
		#ifdef DEBUGp
			printf("Producer: Filename:%s, File FD:%d\n", req[0].filename, req[0].fd);
		#endif
		
		connfd[tail] = dummy;
		#ifdef DEBUGp
			printf("Producer: Writing the FD to array of connfd's, connfd[%d] = %d\n", tail, connfd[tail]);
		#endif

		tail++;
		Pthread_cond_signal(&full);
		#ifdef DEBUGp
			printf("Producer: Releasing lock; tail=%d\n\n",tail);
		#endif
	
		Pthread_mutex_unlock(&m);
	}
return 0;
}
