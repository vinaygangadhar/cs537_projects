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

#define DEBUG
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
int numfill=0, fillptr=0, useptr=0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER, full = PTHREAD_COND_INITIALIZER;
algo_t sched_alg;																	//this is enumerated variable to decide the scheduling algorithm

typedef struct{
	int fd;
	char* filename;
	//int filesize;
}reqfile_t;
reqfile_t *req;

int do_get(){
	int tmp =0;
	switch(sched_alg){
		case FIFO:
			tmp= connfd[useptr];
			useptr = (useptr + 1 ) % buffer_size;
			numfill--;
			break;
		case SFNF:
			//struct reqfile req2;
			//while(connfd[tmp]){
				//if(req)
			printf("SFNF");
			break;
		case SFF:
				printf("SFF");
			break;
		default:
				fprintf(stderr,"Unkonown algorith to select the scheduling");
				exit(0);
	}
	return tmp;
}

void* worker_routine(void* argv){
	while(1){
		Pthread_mutex_lock(&m);
		while(numfill==0){
			#ifdef DEBUG
				printf("Worker: Going to sleep, numfill=%d\n", numfill);
			#endif
			Pthread_cond_wait(&full,&m);
		}

		int tmp = do_get();	
		#ifdef DEBUG
			printf("Worker: Woke up and handling the request, numfill=%d\n", numfill);
		#endif
		
		Pthread_cond_signal(&empty);
		
		requestHandle(tmp);
		Close(tmp);
		Pthread_mutex_unlock(&m);
	}
}

void parseFile(int fd){
		char* buf = (char*) malloc(1000);
		//int rc = read(fd, buf, sizeof(buf));
		int rc = recv(fd, buf, 1000 * sizeof(char), MSG_PEEK);
		#ifdef DEBUG
			printf("Producer:File Content: %s", buf);
		#endif

		if(rc < 0)
			fprintf(stderr,"Error opening the connfd");
		strtok(buf, " ");
		//req->filename = (char*) malloc(100*sizeof(char));
		req = (reqfile_t*)malloc(sizeof(*req));

		req->filename = strtok(NULL, " ");
		req->fd = fd;
		#ifdef DEBUG
			printf("Producer: File Name received is %s\n", req->filename);
		#endif
		//	req->filesize = ;
		//Free the temporary variable buffer
		free(buf);
}


int main(int argc, char *argv[])
{
  int listenfd, clientlen, num_threads;
  struct sockaddr_in clientaddr;
	int port;
	
	
	//Arguments are parsed with this function
	getargs(&port, &num_threads, &buffer_size, &sched_alg, argc, argv);
 	
	connfd = (int*) malloc(buffer_size * sizeof(int));			//Array that stores the file descriptor of the request
	pthread_t worker_id[num_threads];												//we want to create number of worker threads
  
	int i;
	for(i=0; i<num_threads; i++)																		//Create and deploy worker threads
		Pthread_create(&worker_id[i], NULL,(void*) worker_routine, NULL);
	
	#ifdef DEBUG
		printf("Producer: Created %d worker threads \n", num_threads);
	#endif
	
	listenfd = Open_listenfd(port);
 
	//This the producer routine of the producer-consumer solution 
	while (1) {
		clientlen = sizeof(clientaddr);
		int dummy = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
	  #ifdef DEBUG
			printf("Producer: Received a request from client. Acquiring a lock now, numfill=%d\n", numfill);
		#endif
		
		Pthread_mutex_lock(&m);
#ifdef DEBUG
		printf("Producer: acquired lock\n");
#endif
		while(numfill == buffer_size){				//Producer will go to sleep when buffer is full and waits for emptry signal
			Pthread_cond_wait(&empty, &m);
		}
#ifdef DEBUG
		printf("Producer: Parsing file to PraseFile routine\n");
#endif
	
		parseFile(dummy);
	#ifdef DEBUG
		printf("Producer: Filename:%s, File FD:%d\n", req->filename, req->fd);
#endif
		
		connfd[fillptr] = dummy;
		fillptr = (fillptr +1) % buffer_size;
		numfill++;
		Pthread_cond_signal(&full);
#ifdef DEBUG
		printf("Producer:Fillptr=%d, numfill = %d\n Releasing lock\n", fillptr, numfill);
#endif
	
		Pthread_mutex_unlock(&m);
	}
return 0;
}
