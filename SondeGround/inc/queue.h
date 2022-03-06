#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct QelementData
{
	unsigned char	*buf;
	unsigned int	len;
}QelementData;

typedef struct element
{
	struct QelementData	data;
	struct element		*next;
	struct element		*prev;
}element;


/*Queue has five properties. capacity stands for the maximum number of elements Queue can hold.
Size stands for the current size of the Queue and elements is the array of elements. front is the
index of first element (the index at which we remove the element) and rear is the index of last element
(the index at which we insert the element) */
typedef struct Queue
{
	int			capacity;
	int			size;
	element		*front;
	element		*rear;
}Queue;

Queue *createQueue(int maxElements);
int Dequeue(Queue *Q, 	QelementData *data);
int Enqueue(Queue *Q, 	QelementData data);

#endif //#define TDCM_QUEUE_H_
