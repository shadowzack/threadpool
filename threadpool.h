#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "queue.h"

pthread_cond_t wait_for_tp;
pthread_mutex_t wait_for_tp_mutex;
typedef struct TaskRetData
{
    void *buffer;
    int size;

} task_data;

typedef struct ThreadPool
{
    pthread_mutex_t lock;
    pthread_cond_t notify;

    pthread_mutex_t thread_wait_mutex;
    pthread_mutex_t thread_wait_mutex_infi;
    pthread_t wait_thread;
    pthread_cond_t ultranotify;
    pthread_cond_t thread_wait_F;
    Queue * task_q;
    task_data ** results;
    int count;
    char * mem_pool;
    int mem_pool_offset;
    pthread_t *threads;
    pthread_t join_thread;
    int num_of_threads;
    int is_distroy;
    int available_thread_count;
    int num_of_tasks;

} thread_pool;

typedef struct Task
{
    void *(*task_f)(void *);
    void *arg;
    pthread_t *tids;
    int thread_count;
} task;

typedef struct data_ret{
int incircle;
}data_ret;
thread_pool *create(int num_threads,int threads_per_task,int max_task_count);
int destroy_threadpool(thread_pool *tp,int wait_task_flag);
int thread_pool_insert_task(thread_pool *tp, void (*task_f)(void *), void *arg);
int free_threadpool(thread_pool *tp);
void thread_pool_wait(thread_pool* pool);
#endif