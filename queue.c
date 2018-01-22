#include "queue.h"
#include <stdlib.h>

Queue* CreateQueue()
{
   Queue* q = malloc(sizeof(Queue));

   if(q == NULL)
      return NULL;

   q->head = q->tail = NULL;
   
   return q;
}

void DestroyQueue(Queue* q)
{
   if(q == NULL)
      return;

   while(Dequeue(q) != NULL);
      
   free(q);
}

int IsQueueEmpty(Queue* q)
{ 
   return (q->tail == NULL && q->head == NULL);
}

void Enqueue(Queue* q, void* data)
{
   Node* node = malloc(sizeof(Node));
   
   
   node->data = data;
   node->next = NULL;
 
   if(q->tail == NULL)
   {
      q->head=q->tail=node;
      return;
   }
      
   q->tail->next = node;
   q->tail = node;

}

void* Dequeue(Queue* q)
{
   Node* previousHead;
   void* data;
   
   previousHead = q->head;
   
   if(previousHead == NULL)
      return NULL;
 
   q->head = q->head->next;
   
   if (q->head == NULL)
      q->tail = NULL;
   
   data = previousHead->data;
   free(previousHead);
   return data;
}
void * Top(Queue* q)
{
    void* data=q->head->data;
    return data;
}