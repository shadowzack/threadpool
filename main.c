#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "taskfeeder.h"
#include "threadpool.h"

#define RANDOM_NUMBERS 35791246

//our task that would be handeled to tp
/*we iniltalize random numbers and check if the numbers are inside the raduis
 of the unit circle so if z<=1 then its a circle point so we incremnt the counter*/
void *func(void * a)
{
    int* count = (int*)malloc(sizeof(int));
    *count = 0;    
    double x,y,z;
    x=(double)rand()/RAND_MAX;
    y=(double)rand()/RAND_MAX;
    z=x*x+y*y;
    if(z<=1){
    (*count)++;
    }

    struct TaskRetData* trd = (struct TaskRetData*)malloc(sizeof(struct TaskRetData));//taskretdata to save our numbers
    trd->buffer = count;
    trd->size = sizeof(*count);
  return trd;

}
int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf(" WRONG ARGS (FORMAT: I N T F)\n");
        return -1;
    }


int itreations=atoi(argv[1]);
printf("program will have %d itreations\n",itreations);

int N,T,F;
N=atoi(argv[2]);
T=atoi(argv[3]);
F=atoi(argv[4]);

double pi;
srand(RANDOM_NUMBERS);

//initlaizing our thread pool
init(N,T,F);

//inserting tasks
for(int i=0;i<itreations;i++)
{
    add_task((void*)func,NULL);
}
      

int* newpool = (int*)get_threadpool_memory_pool();//the thread pool memory pool
int pool_size=get_memory_pool_size();//the pool size

destroy();

int count = 0;
      
for (int i = 0; i <pool_size ; i+=2)
{
    int size = newpool[i];
    int value = newpool[i + 1];
    count += value;
}
                 
pi=(double)count/itreations*4;
printf("pi :%f\n\n",pi);

}


