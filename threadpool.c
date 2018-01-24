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


	
		
	/*	int z=0;
		//printf("%d",main_thread_pool->num_of_threads);
		for(z=0;z<main_thread_pool->num_of_threads;z++)
		{
			if(pthread_equal(main_thread_pool->threads[z],pthread_self()));
			{
					printf("weed dit it ");
					break;
			
			}
			//printf("%d\n",z);
		}
		*/
		main_task = (task*) Top(main_thread_pool->task_q);
		int f=0;

	for(int z=0;z<T;z++)
	{
		//printf("%d\n",main_task->tids[z]);
		if(pthread_equal(main_task->tids[z],pthread_self()));
			{
				//printf("f: %lld\n",pthread_self());
					f++;
					//printf("this ")
					break;
			
			}

	}
	if(f)
	{
		f--;
	/*for(int z=0;z<T;z++)
	{
		printf("%lld\n",main_task->tids[z]);
	}
		printf("this thread :"); */	
	main_task = (task*) Dequeue(main_thread_pool->task_q);
	//printf("%lld is inside tids\n",pthread_self());
	
	}
	else{
		pthread_mutex_unlock(&(main_thread_pool->lock));
		continue;
	}
			//main_task = (task*) Dequeue(main_thread_pool->task_q);
			/*if (main_thread_pool->available_thread_count==main_thread_pool->num_of_threads) {
				pthread_cond_signal(&main_thread_pool->thread_wait_F);
			//	printf("okkkkkkkkkkkkkk\n");
			}*/


		if(main_task!=NULL)
			--main_thread_pool->num_of_tasks;

		//pthread_create(&tp->join_thread, NULL, run, (void *)tp);
		
	/*	if (main_thread_pool->available_thread_count==main_thread_pool->num_of_threads) {
				pthread_cond_signal(&main_thread_pool->thread_wait_F);
		}
		*/
	/*	void* ret;
		printf("hey1\n");
		if(pthread_join(main_thread_pool->threads[z],&ret)==0){
		printf("hey2\n");
		}
		
		data_ret* dr=(data_ret*)ret;
		if(dr->incircle!=NULL)
		printf("hey\n");
						printf("   value is : %d \n",dr->incircle);*/

        //printf("were here \n\n");
	
		if (main_task != NULL ) {
		//	printf("Thread: %u\n", (int)pthread_self());
			if (main_task->task_f != NULL)
			{
				void* res = main_task->task_f(main_task->arg);
				task_data* trd = (task_data*)res;

				printf(" size :%d\n",trd->size);
				printf(" buffer :%d\n",*(int*)(trd->buffer));
			  int * tmp=(int *)malloc(sizeof trd->buffer);
			  tmp=*(int*)trd->buffer;
			  trd->buffer=tmp;
			  printf("size of buffer %d ,size of tmp %d\n",trd->buffer,tmp);
				main_thread_pool->results[main_thread_pool->count++] = trd;
			
				main_thread_pool->available_thread_count++;
				if (main_thread_pool->available_thread_count==main_thread_pool->num_of_threads) {
				pthread_cond_signal(&main_thread_pool->thread_wait_F);
			//	printf("okkkkkkkkkkkkkk\n");
			}
			}
		//	curser-=T;
			if(main_task->tids)
				free(main_task->tids);
			free(main_task);
		
			
		
		
		
		}
	pthread_mutex_unlock(&(main_thread_pool->lock));

	}

	pthread_mutex_unlock(&(main_thread_pool->lock));
	//pthread_exit(NULL);

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
		tp->count=0;
		tp->results=(task_data*)malloc(sizeof(char)*10000000);

	/*pthread_cond_init(&tp->thread_wait_F, NULL);
	pthread_mutex_init(&tp->thread_wait_mutex, NULL);
	pthread_create(&(tp->wait_thread), NULL, check_wait, (void *)tp);
	pthread_mutex_init(&tp->thread_wait_mutex_infi, NULL);*/
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
	tp->mem_pool_offset=0;
	tp->count=0;
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
						//printf("Bar thread exited with status:%d\n", (int)ret);
						
						}

//printf("reference %d\n",main_thread_pool->results[main_thread_pool->count-1]->size);

			task_data** trd=(task_data**)malloc(sizeof(char)*10000000);

				 trd = (task_data**)ret;
				//printf("second size :%d\n",trd->size);
			//	printf("second buffer :%d\n",*(int*)(trd->buffer));
				//printf("second buffer :%d\n",(trd[0]->size));
				//pool->results[pool->count++] = *trd;
				//main_thread_pool->results[main_thread_pool->count++] = *(int*)(trd->buffer);

			/*	printf("pool counter :%d\n\n\n",pool->count);
				for(int z=0;z<pool->count;z++)
				{
					
					printf("second size :%d        \n",trd[z]->size);
					printf("%d\n",sizeof trd[z]->size);
					printf(" size of buffer :%d\n",sizeof trd[z]->size);
					printf("second buffer :%d\n",trd[z]->buffer);

					///from here 
					int x=trd[z]->size;//cheked the value is ok
					
					memcpy(&(pool->mem_pool[pool->mem_pool_offset]),x,4);
					pool->mem_pool_offset+=4;
					
					//to here

					memcpy(pool->mem_pool[pool->mem_pool_offset],trd[z]->buffer,trd[z]->size);
					pool->mem_pool_offset +=trd[z]->size;
					printf("offset   %d\n",pool->mem_pool_offset);
				}
				
*/
				printf("pool counter :%d\n\n\n",pool->count);
                for(int z=0;z<pool->count;z++)
                {
                    
                    /*printf("second size :%d        \n",trd[z]->size);
                    printf("%d\n",sizeof trd[z]->size);
                    printf(" size of buffer :%d\n",sizeof trd[z]->size);
                    printf("second buffer :%d\n",trd[z]->buffer);
*/
                    ///from here 
                    int* x=(int*)malloc(sizeof(trd[z]->size));
                    
                    x=trd[z]->size;//cheked the value is ok
                    
                    memcpy(&pool->mem_pool[pool->mem_pool_offset],&x,4);
                    pool->mem_pool_offset+=4;
                    printf("x=%d\n",x);
                    //to here

                    memcpy(&pool->mem_pool[pool->mem_pool_offset],&trd[z]->buffer,x);
                    pool->mem_pool_offset +=trd[z]->size;
                    printf("offset   %d\n",pool->mem_pool_offset);
                }
                
            int* newpool = (int*)pool->mem_pool;

int count=0;
for (int i = 0; i < pool->count*2; i+=2)
{
    int size = newpool[i];
    //printf("new size=%d    i=%d\n",size,i);
    int value = newpool[i + 1];
    printf("new value=%d    i=%d\n",value,i);
    count += value;
}
                    printf("%d",count); 
					
						
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
			











