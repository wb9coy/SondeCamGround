#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include "modem.h"
#include "packetDefs.h"
#include "statusThread.h"
#include "logger.h"

int statusThreadActive		= 0;

static pthread_cond_t 	statusCon    = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t  statusConMut = PTHREAD_MUTEX_INITIALIZER;

void *statusThreadFunc(void *x_void_ptr)
{
	printf("Start Status Thread  \n");
	statusThreadActive = 1;
	while (statusThreadActive)
	{
		pthread_mutex_lock (&statusConMut);
		pthread_cond_wait(&statusCon, &statusConMut);
		pthread_mutex_unlock (&statusConMut);

	}

	return EXIT_SUCCESS;
}

int startStatusThread()
{

	static struct sched_param	statusParam;
	static pthread_attr_t 		statusAttr;

	static pthread_t statusThread;
	int 				rc =0;

	rc = pthread_attr_init(&statusAttr);
	if(rc != 0)
	{
		fprintf(stderr, "Error startStatusThread pthread_attr_init failed\n");
		return -1;
	}
	rc = pthread_attr_getschedparam (&statusAttr, &statusParam);
	if(rc != 0)
	{
		fprintf(stderr, "Error startStatusThread pthread_attr_getschedparam failed\n");
		return -1;
	}
	statusParam.sched_priority = 0;
	rc = pthread_attr_setschedparam(&statusAttr, &statusParam);
	if(pthread_create(&statusThread, &statusAttr, statusThreadFunc, NULL))
	{
		fprintf(stderr, "Error startStatusThread creating status Thread\n");
		return -1;
	}

	while(statusThreadActive != 0)
	{

	}

	return 1;
}

void stopStatusThread()
{
	statusThreadActive = 0;
}

void signalStatusThread()
{
	pthread_cond_signal(&statusCon);
}
