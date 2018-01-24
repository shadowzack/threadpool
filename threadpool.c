#include "threadpool.h"

typedef enum {

	FINISH_CURRENT = 1, WAIT_FOR_QUEUE = 2
} ShutdownType;

static int counter =0;
static int T;
static int curser=0;
static int max_task_count_per_push=0;
static int broadcat_flag=0;

void * run(void *pool)
{
	thread_pool * main_thread_pool = (thread_pool *)pool;
	task * main_task;
	
	if (main_thread_pool->available_thread_count > 0)
		main_thread_pool->available_thread_count--;

	
	for (;;)
	{
		/* lock each thread and wait on con var */
		pthread_mutex_lock(&(main_thread_pool->lock));
		/* cheak if there is no tasks */
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


	
		

		main_task = (task*) Top(main_thread_pool->task_q);
		int f=0;
//here we check if this thread is in tids
	for(int z=0;z<T;z++)
	{
	
		if(pthread_equal(main_task->tids[z],pthread_self()));
			{
				
					f++;
					break;
			
			}

	}
	if(f)
	{
		//we got one
		f--;

	main_task = (task*) Dequeue(main_thread_pool->task_q);
	
	}
	else{
		//this thread not in tids of this task so we go on
		pthread_mutex_unlock(&(main_thread_pool->lock));
		continue;
	}



		if(main_task!=NULL)
			--main_thread_pool->num_of_tasks;

	
		if (main_task != NULL ) {

			if (main_task->task_f != NULL)
			{
				void* res = main_task->task_f(main_task->arg);
				task_data* trd = (task_data*)res;

				
			  int * tmp=(int *)malloc(sizeof trd->buffer);
			  tmp=*(int*)trd->buffer;
			  trd->buffer=tmp;
				main_thread_pool->results[main_thread_pool->count++] = trd;
			
				main_thread_pool->available_thread_count++;
				/*if (main_thread_pool->available_thread_count==main_thread_pool->num_of_threads) {
				pthread_cond_signal(&main_thread_pool->thread_wait_F);
			
			}*/
			}
	
			if(main_task->tids)
				free(main_task->tids);
			free(main_task);
		
			
		
		
		
		}
	pthread_mutex_unlock(&(main_thread_pool->lock));

	}

	pthread_mutex_unlock(&(main_thread_pool->lock));

	return main_thread_pool->results;
}

//void* joinThread
/*
void thread_pool_wait(thread_pool* pool){
	//pthread_mutex_lock(&pool->thread_wait_mutex);
	while ((pool->num_of_tasks!=0) || ((pool->num_of_threads)-(pool->available_thread_count))!=0) {
		printf("finishing tasks in queue\n");
		pthread_mutex_lock(&pool->thread_wait_mutex);
		pthread_cond_wait(&pool->thread_wait_F, &pool->thread_wait_mutex);

		pthread_mutex_unlock(&pool->thread_wait_mutex);
		break;
	}
	//pthread_mutex_unlock(&pool->thread_wait_mutex);
}
void * check_wait(void * tp)
{
	thread_pool * pool = (thread_pool *)tp;
	pthread_mutex_lock(&pool->thread_wait_mutex_infi);
	while(1){
		if (pool->available_thread_count==pool->num_of_threads) {
				pthread_cond_signal(&pool->thread_wait_F);
				//printf("okkkkkkkkkkkkkk\n");
			}
	}
	pthread_mutex_unlock(&pool->thread_wait_mutex_infi);
}
*/
thread_pool * create(int num_threads,int threads_per_task,int max_task_count){

	T=threads_per_task;
	max_task_count_per_push=max_task_count;

	if (num_threads <= 0)
	{
		return NULL;
	}
	thread_pool *tp = malloc(sizeof(*tp));
	if (!tp)
	{
		perror("create():tp malloc");
		return NULL;
	}
	tp->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
	if (!tp->threads)
	{
		perror("create():thread array malloc");
		free_threadpool(tp);
		return NULL;
	}

	tp->task_q = CreateQueue();
	if (!tp->task_q)
	{
		free_threadpool(tp);
		perror("create():queue creation");
		return NULL;
	}
		
	/*pthread_cond_init(&tp->thread_wait_F, NULL);
	pthread_mutex_init(&tp->thread_wait_mutex, NULL);
	pthread_create(&(tp->wait_thread), NULL, check_wait, (void *)tp);
	pthread_mutex_init(&tp->thread_wait_mutex_infi, NULL);*/

	//init for mutex and con varibles
	if ((pthread_mutex_init(&(tp->lock), NULL) != 0) || (pthread_mutex_init(&(tp->notify), NULL) != 0))
	{
		free_threadpool(tp);
		perror("create():pthread init");
		return NULL;
	}

	

	tp->num_of_threads = num_threads;
	tp->is_distroy = 0;
	tp->available_thread_count = num_threads;
	tp->num_of_tasks = 0;
	tp->mem_pool_offset=0;
	tp->count=0;
	tp->thread_pool_free_flag=0;
	tp->results=(task_data*)malloc(sizeof(char)*10000000);
	tp->mem_pool=(char*)malloc(sizeof(char)*10000000);
	if(!tp->results || !tp->mem_pool)
	{
		free_threadpool(tp);
		perror("create():results malloc");
		return NULL;
	}

	for (int i = 0; i < num_threads; i++)
	{
		if (pthread_create(&(tp->threads[i]), NULL, run, (void *)tp) != 0)
		{
			free_threadpool(tp);
			perror("create():pthread_create");
			return NULL;
		}
	
	}


	return tp;
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



	char* join_threadpool(thread_pool *pool,int wait_task_flag){
		
			if(pool==NULL)
				return NULL;
		
			if(pthread_mutex_lock(&(pool->lock))!=0)
				return NULL;
		
			
				do{

					if (wait_task_flag == 0) {
						pool->is_distroy = FINISH_CURRENT;
					} else {
						pool->is_distroy = WAIT_FOR_QUEUE;
			
					}
					/* 	call all threads */
					if ((pthread_cond_broadcast(&(pool->notify)) != 0)
							|| (pthread_mutex_unlock(&(pool->lock)) != 0)) {
								pool->thread_pool_free_flag=1;
				
						break;
					}
			
			
					/* Join all thread and get the return value */
					void* ret;
					for (int i = 0;i< pool->num_of_threads; i++) {
						if (pthread_join(pool->threads[i], &ret) != 0) {
							pool->thread_pool_free_flag=2;
				
						}
						}


			task_data** trd=(task_data**)malloc(sizeof(char)*10000000);
					trd = (task_data**)ret;
	
                for(int z=0;z<pool->count;z++)
                {
                    int* x=(int*)malloc(sizeof(trd[z]->size));
                    x=trd[z]->size;
                    memcpy(&pool->mem_pool[pool->mem_pool_offset],&x,4);
                    pool->mem_pool_offset+=4;
			
                    memcpy(&pool->mem_pool[pool->mem_pool_offset],&trd[z]->buffer,x);
                    pool->mem_pool_offset +=trd[z]->size;

                }
          
					
						
				}while(0);
				
	
			
		return pool->mem_pool;
		}

	void destroy_threadpool(thread_pool *pool){
		/*we check if flag is still zero so we have to free flages can change in join threads func*/
			if(!pool->thread_pool_free_flag)
				free_threadpool(pool);

	}


int thread_pool_insert_task(thread_pool *pool, void(*task_f)(void*), void *arg){
			
	

	if (!pool || !task_f || pool->is_distroy!=0) {
		printf("insert():error\n");
		return -1;
	}

	if (pthread_mutex_lock(&(pool->lock)) != 0) {
		printf("insert():lock\n");
		return -1;
	}


	do {
		task * t = malloc(sizeof( task));//*
		if (!t) {
			printf("insert():task malloc\n");
			return -1;
		}
	
		t->task_f = (void*)task_f;
		t->arg = arg;
		t->tids=(pthread_t *)malloc(sizeof(pthread_t) * T);
		
		
			for(int f=0;f<T;f++)
			{
				t->tids[f]=pool->threads[curser];

				if(curser==pool->num_of_threads)
						curser=0;
				curser++;
			}
		Enqueue(pool->task_q, t);
	
		pool->num_of_tasks++;
		if (pthread_cond_broadcast(&(pool->notify)) != 0) {
			printf("insert():pthread_cond_signal fails\n");
			break;
		
		
		
		}
	} while (0);

	if (pthread_mutex_unlock(&pool->lock) != 0) {
		printf("insert():pthread_mutex_unlock fails\n");

		return -1;
	}
	
	return 0;
	
}
			











