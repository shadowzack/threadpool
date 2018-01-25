#ifndef TASKFEEDER_H
#define TASKFEEDER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "threadpool.h"
static int max_task=0;
static int task_counter=0;

thread_pool* tp;

void init(int N,int T ,int F){
	max_task=F;
	tp=create(N,T);
	printf("num of threads is: %d threads\n",N);

}
void add_task(void(*func)(void*), void* z){

	/*we check if the user inserted F tasks if so we push
	the insertaion until the the thread pool finishes the 
	tasks in the queue*/
	if(task_counter!=max_task){
	thread_pool_insert_task(tp,func,z);
	task_counter++;
	}
	else{
		//printf("waiting for thread pool to finish %d tasks\n",max_task);
		 thread_pool_wait(tp);
		 task_counter=0;
		 thread_pool_insert_task(tp,func,z);
	}

}

void destroy(){

	destroy_threadpool(tp);
}
char* get_threadpool_memory_pool(){
		join_threadpool(tp,1);
	
}
int get_memory_pool_size()
{
	return tp->count*2;
}


#endif
