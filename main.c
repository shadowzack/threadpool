#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "taskfeeder.h"
#include "threadpool.h"

#define SEED 35791246

//int count=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*
long incircle = 0;
long points_per_thread;
void *Rand(void *a);
void *runner() {
    printf("Hello from the new thread, my pid is %d\n", pthread_self());
    long incircle_thread = 0;

    unsigned int rand_state = rand();
    long i;
    for (i = 0; i < points_per_thread; i++) {
        /* Was initially using random(), but didn't appear to get performance
         * improvement with threading. random() apparently uses some global state
         * that is shared between threads, and isn't guaranteed to be threadsafe. */
  /*      double x = rand_r(&rand_state) / ((double)RAND_MAX + 1) * 2.0 - 1.0;
        double y = rand_r(&rand_state) / ((double)RAND_MAX + 1) * 2.0 - 1.0;

        if (x * x + y * y < 1) {
            incircle_thread++;
        }
    }
    
    //pthread_mutex_lock(&mutex);
  //  incircle += incircle_thread;
    pthread_mutex_lock(&mutex);
    incircle += incircle_thread;
    pthread_mutex_unlock(&mutex);
   // pthread_mutex_unlock(&mutex);
}
*/
//int main_count=0;
void *func(void * a)
{
    //pthread_mutex_lock(&mutex);
    int* count = (int*)malloc(sizeof(int));
    *count = 0;    
    double x,y,z;
    x=(double)rand()/RAND_MAX;
    y=(double)rand()/RAND_MAX;
    z=x*x+y*y;
    if(z<=1){;
    //main_count++;
    (*count)++;//==main_count;
    }
   // int tmp=count;
   // pthread_exit(tmp);
  // return(void *)tmp;
  struct TaskRetData* trd = (struct TaskRetData*)malloc(sizeof(struct TaskRetData));
  trd->buffer = count;
  trd->size = sizeof(*count);
  return trd;
    //pthread_mutex_unlock(&mutex);
    //printf("thread :%d  doing task\n",pthread_self());
    // return &dr;
   //return &tmp;
}
int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf(" WRONG ARGS (FORMAT: I N T F)\n");
        return -1;
    }
int itreations=atoi(argv[1]);

int N,T,F;
N=atoi(argv[2]);
T=atoi(argv[3]);
F=atoi(argv[4]);
//scanf( "%d %d %d", &N, &T, &F);
double pi;
srand(SEED);
printf("program will have %d itreations\n",itreations);
init(itreations,N,T,F);


for(int i=0;i<itreations;i++)
{
    add_task((void*)func,NULL);
}

destroy();
int count = 0;
/*
int* pool = (int*)thread_pool_manager_get_pool();


for (int i = 0; i < num_results / 2; ++i)
{
    int size = pool[i];
    int value = pool[i + 1];
    count += value;
}*/

pi=(double)count/itreations*4;
printf("pi :%f\n\n",pi);

}


/*
long totalpoints = atol(argv[1]);
int thread_count = atoi(argv[2]);
points_per_thread = totalpoints / thread_count;

init(N,T,F);


srand(SEED); //initilaze random numbers


printf("Pi: %f\n", (4. * (double)incircle) / ((double)points_per_thread * thread_count));


destroy();
printf("suprise motha fucka");
return 0;
}
*/
