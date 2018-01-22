#ifndef TASKFEEDER_H
#define TASKFEEDER_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "threadpool.h"
static int max_task=0;
static int task_counter=0;

thread_pool* tp;

void init(int I,int N,int T ,int F){
	set_I(I);
	if ((pthread_mutex_init(&(wait_for_tp_mutex), NULL) != 0) || (pthread_mutex_init(&(wait_for_tp), NULL) != 0))
	{

		return NULL;
	}	
	max_task=F;
	tp=create(N,T,F);
	printf("num of threads is: %d threads\n",N);

}
void add_task(void(*func)(void*), void* z){

	/*task_counter++;
	if(task_counter<max_task)
	{**/
	thread_pool_insert_task(tp,func,z);
	/*}
	else
	{
		
		pthread_mutex_lock(&(wait_for_tp_mutex));
		
		pthread_cond_wait(&(wait_for_tp),&(wait_for_tp_mutex));
		task_counter=0;
		
		pthread_mutex_unlock(&(wait_for_tp_mutex));
	}
	
//	printf("Thread: %u Got a task\n",(int)pthread_self());
*/
}

void destroy(){

	destroy_threadpool(tp,1);
}






#endif
