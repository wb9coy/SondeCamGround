#include <stdlib.h>
#include <pthread.h>
#include "queue.h"

/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
a Queue according to it and returns a pointer to the Queue. 
*/

Queue * createQueue(int maxElements)
{
	/* Create a Queue */
	Queue *Q;
	Q = (Queue *)calloc(1,sizeof(Queue));
	/* Initialize its properties */
	Q->size = 0;
	Q->capacity = maxElements;
	Q->front = NULL;
	Q->rear =  NULL;

	/* Return the pointer */
	return Q;
}

int Dequeue(Queue *Q, QelementData *data)
{

	/* If Queue size is zero then it is empty. So we cannot pop */
	if (Q->size == 0)
	{
		return 0;
	}
	/* Removing an element is equivalent to incrementing index of front by one */
	else
	{
		element 	*Qelement;

		Qelement 	= Q->front;

		data->buf = Qelement->data.buf;
		data->len = Qelement->data.len;

		if(Q->front == Q->rear)
		{

			Q->front = NULL;
			Q->rear = NULL;

		}
		else
		{

			Q->front = Q->front->prev;
			Q->front->next = NULL;
		}

		Q->size--;
		free(Qelement);

		return 1;

	}
}

int  Enqueue(Queue *Q, QelementData data)
{
	element *node = NULL;
	if(Q->size == Q->capacity)
	{
		return 0;
	}
	else
	{
		node = (element*)calloc(1,sizeof(element));
		node->data.buf 	= data.buf;
		node->data.len 	= data.len;
		node->prev		= NULL;
		if(Q->rear == Q->front)
		{
			if(Q->front == NULL)
			{
				Q->front	= node;
			}
			else
			{
				Q->front->prev	= node;
			}
			Q->front->next	= NULL;
;
		}
		else
		{
			Q->rear->prev 	= node;
		}

		node->next 		= Q->rear;

		Q->rear			= node;

		Q->size++;

		return 1;
	}
}
