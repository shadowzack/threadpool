#ifndef QUEUE_H
#define QUEUE_H


typedef struct node
{
	struct node* next;
	void* data;
}Node;

typedef struct queue
{
   Node *head, *tail;  
   
}Queue;

Queue* CreateQueue();

void DestroyQueue(Queue* queue);

int IsQueueEmpty(Queue* queue);

void Enqueue(Queue* queue, void* data);

void* Dequeue(Queue* queue);

void* Top(Queue* queue);

#endif
