#include "threadpool.h"

typedef enum {

	FINISH_CURRENT = 1, WAIT_FOR_QUEUE = 2
} ShutdownType;

static int counter =0;
static int T;
static int curser=0;
static int max_task_count_per_push=0;
static int itr=0;
static int broadcat_flag=0;
void set_I(int itreations)
{
	itr=itreations;
}
void * run(void *pool)
{
	thread_pool * main_thread_pool = (thread_pool *)pool;

	task * main_task;
	
	if (main_thread_pool->available_thread_count > 0)
		main_thread_pool->available_thread_count--;

	for (;;)
	{
		/* Lock must be taken to wait on conditional variable */
		pthread_mutex_lock(&(main_thread_pool->lock));

		/* Wait on condition variable, check for spurious wakeups.
		 When returning from pthread_cond_wait(), we own the lock. */
		while (((main_thread_pool->num_of_tasks == 0) && (!main_thread_pool->is_distroy))) 
		{
			
		pthread_cond_wait(&(main_thread_pool->notify), &(main_thread_pool->lock));
		
		}

		if ((main_thread_pool->is_distroy == FINISH_CURRENT)
				|| ((main_thread_pool->is_distroy == WAIT_FOR_QUEUE)
						&& (main_thread_pool->num_of_tasks == 0))) 
						{

			break;
						}


	
		
						
			main_task = (task*) Dequeue(main_thread_pool->task_q);



		if(main_task!=NULL)
			--main_thread_pool->num_of_tasks;

		

		pthread_mutex_unlock(&(main_thread_pool->lock));

        //printf("were here \n\n");
	
	
		if (main_task != NULL ) {
		//	printf("Thread: %u\n", (int)pthread_self());
			if (main_task->task_f != NULL)
			{
				main_task->task_f(main_task->arg);
				//--main_thread_pool->num_of_tasks;
				//main_thread_pool->is_distroy=0;
				main_thread_pool->available_thread_count++;
			}
			curser-=T;
			if(main_task->tids)
				free(main_task->tids);
			free(main_task);
		
			
		}
	

	}

	pthread_mutex_unlock(&(main_thread_pool->lock));
	pthread_exit(NULL);

	return (NULL);
}

thread_pool * create(int num_threads,int threads_per_task,int max_task_count){
	T=threads_per_task;
	max_task_count_per_push=max_task_count;
	//printf("%d",itr/max_task_count_per_push);
	//printf("%d\n%d\n",T,threads_per_task);

	//counter++;
	if (num_threads <= 0)
	{
		return NULL;
	}
	thread_pool *tp = malloc(sizeof(*tp));
	if (!tp)
	{
		perror("malloc");
		return NULL;
	}
	tp->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
	if (!tp->threads)
	{
		perror("malloc");
		free_threadpool(tp);
		return NULL;
	}

	tp->task_q = CreateQueue();
	if (!tp->task_q)
	{
		free_threadpool(tp);
		perror("queue creation");
		return NULL;
	}

	//init for mutex and con varibles
	if ((pthread_mutex_init(&(tp->lock), NULL) != 0) || (pthread_mutex_init(&(tp->notify), NULL) != 0))
	{
		free_threadpool(tp);
		//free_threadpool(tp->threads);
		perror("create():pthread init");
		return NULL;
	}
	tp->num_of_threads = num_threads;
	tp->is_distroy = 0;
	tp->available_thread_count = num_threads;
	tp->num_of_tasks = 0;

	for (int i = 0; i < num_threads; i++)
	{
		if (pthread_create(&(tp->threads[i]), NULL, run, (void *)tp) != 0)
		{
			free_threadpool(tp);
			return NULL;
		}
	
	}

	/* Wait for threads to initialize */
//	while (tp->num_of_threads != num_threads) {}

	return tp;
}
void set_T(int threads_per_task)
{
T=threads_per_task;
}

int free_threadpool(thread_pool *pool){

		if(pool==NULL)
			return 0;
		if(pool->threads)
			free(pool->threads);
	
			DestroyQueue(pool->task_q);


		pthread_mutex_lock(&(pool->lock));
		pthread_mutex_destroy(&(pool->lock));
		pthread_cond_destroy(&(pool->notify));

	
		free(pool);
		return 0;
	
	}



	int destroy_threadpool(thread_pool *pool,int wait_task_flag){
		
		int flag = 0;
			if(pool==NULL)
				return -1;
		
			if(pthread_mutex_lock(&(pool->lock))!=0)
				return -2;
		
			
				do{

					if (wait_task_flag == 0) {
						pool->is_distroy = FINISH_CURRENT;
					} else {
						pool->is_distroy = WAIT_FOR_QUEUE;
			
					}
					/* 	 up all worker threads */
					if ((pthread_cond_broadcast(&(pool->notify)) != 0)
							|| (pthread_mutex_unlock(&(pool->lock)) != 0)) {
						flag = 1;
						break;
					}
			
			
					/* Join all worker thread */
					void* ret;
					for (int i = 0;i< pool->num_of_threads; i++) {
						if (pthread_join(pool->threads[i], &ret) != 0) {
							flag = 2;
							
						}
						printf("Bar thread exited with status:%d\n", (int)ret);
						
						}
						
				}while(0);
				
		if(!flag)
			free_threadpool(pool);
			
		return 0;
		}

int thread_pool_insert_task(thread_pool *pool, void(*task_f)(void*), void *arg){
			
	

	if (!pool || !task_f || pool->is_distroy!=0) {
		printf("error\n");
		return -1;
	}

	if (pthread_mutex_lock(&(pool->lock)) != 0) {
		printf("error2\n");
		return -1;
	}

	do {
		task * t = malloc(sizeof( task));//*
		if (!t) {
			printf("error3\n");
			return -1;
		}
		t->thread_count=0;
		t->task_f = task_f;
		t->arg = arg;
		t->tids=(pthread_t *)malloc(sizeof(pthread_t) * T);
		
	
		Enqueue(pool->task_q, t);
	
		pool->num_of_tasks++;
		if (pthread_cond_broadcast(&(pool->notify)) != 0) {
			printf("pthread_cond_signal fails\n");
			break;
		
		
		
		}
	} while (0);

	if (pthread_mutex_unlock(&pool->lock) != 0) {
					printf("pthread_mutex_unlock fails\n");

		return -1;
	}
	
	return 0;
	
}
			











