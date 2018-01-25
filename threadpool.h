#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "queue.h"

typedef struct TaskRetData
{
    void *buffer;
    int size;

} task_data;

typedef struct ThreadPool
{
    pthread_mutex_t lock;           //for locking
    pthread_cond_t notify;          //notify threads on insert task
    pthread_cond_t thread_wait_F;   //notify when queue is empty
    Queue * task_q;                 //task queue
    task_data ** results;           //results returned from each task
    int count;
    char * mem_pool;                //our memory pool for results
    int mem_pool_offset;            //offset inide memory pool
    pthread_t *threads;             //array of theads
    int num_of_threads;
    int shutdown;                   //checks if we should shut down
    int thread_pool_free_flag;      //free data structure data flag
    int available_thread_count;     //threads that arent working
    int num_of_tasks;

} thread_pool;

typedef struct Task
{
    void *(*task_f)(void *);
    void *arg;
    pthread_t *tids;
} task;


//initalizing the thread pool
thread_pool *create(int num_threads,int threads_per_task);
//destroing the thread pool
void destroy_threadpool(thread_pool *tp);
//retrun a memory pool with the results
char* join_threadpool(thread_pool *tp,int wait_task_flag);
//inserting new tasks
int thread_pool_insert_task(thread_pool *tp, void (*task_f)(void *), void *arg);
//freing thread pool data mutex con var....
int free_threadpool(thread_pool *tp);
//makes the thread pool wait unltil  the Queue is empty to enter the next F tasks
void thread_pool_wait(thread_pool* pool);

#endif