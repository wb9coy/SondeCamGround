#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include "utils.h"
#include "logger.h"


int loggerThreadActive		= 0;
static pthread_cond_t  loggerQcon  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t loggerQmut  = PTHREAD_MUTEX_INITIALIZER;

static struct sched_param	param;
static pthread_attr_t 		attr;
static pthread_t loggerThread;

static pthread_mutex_t loggerConmut=PTHREAD_MUTEX_INITIALIZER;

static Queue *loggerQ;

void *loggerThreadFunc(void *x_void_ptr)
{
	struct timeval tv;
	time_t t;
	struct tm *localTime;
	char *localTimeAsc;
	char *localTimeNoLF;

	char dataToLog[200];
	int lenDataToLog;

	int loggerQueueSize =10000;

	int QStatus;
	QelementData loggerData;

	FILE *fptr = NULL;

	printf("Start Logger Thread  \n");
	loggerQ = createQueue(loggerQueueSize);

	createDataFilePathName();

	fptr = fopen(getImageFilePathName(),"w");
	if (fptr)
	{
	   fclose(fptr);
	}

	localTimeNoLF = calloc(50,sizeof(unsigned char));
	localTimeAsc  = calloc(50,sizeof(unsigned char));

	loggerThreadActive = 1;
	while (loggerThreadActive)
	{
		pthread_mutex_lock (&loggerConmut);
		pthread_cond_wait(&loggerQcon, &loggerConmut);
		pthread_mutex_unlock (&loggerConmut);

		while(loggerQ->size > 0)
		{
			pthread_mutex_lock (&loggerQmut);
			QStatus = Dequeue(loggerQ,&loggerData);
			pthread_mutex_unlock (&loggerQmut);

			if(QStatus != 0)
			{
				fptr = fopen(getDataFilePathName(),"a");
				if (fptr == NULL)
				{
					printf("Fatal Error could not open log file %s for appen\n",getDataFilePathName());
				}
				else
				{
					gettimeofday(&tv, NULL);
					t = tv.tv_sec;

					localTime = localtime(&t);
					localTimeAsc = asctime(localTime);
					memcpy(localTimeNoLF,localTimeAsc,strlen(localTimeAsc)-1);

				    sprintf (dataToLog, "%s %s\n", localTimeNoLF,loggerData.buf);
				    lenDataToLog = strlen(dataToLog);

					fwrite(dataToLog, 1, lenDataToLog, fptr);
					fclose(fptr);
					fptr = NULL;
				}

				free(loggerData.buf);
			}
		}
	}

	return EXIT_SUCCESS;
}

int startLoggerThread()
{

	int 				rc =0;
	rc = pthread_attr_init(&attr);
	if(rc != 0)
	{
		fprintf(stderr, "Error pthread_attr_init failed\n");
		return -1;
	}
	rc = pthread_attr_getschedparam (&attr, &param);
	if(rc != 0)
	{
		fprintf(stderr, "Error pthread_attr_getschedparam failed\n");
		return -1;
	}
	param.sched_priority = 0;
	rc = pthread_attr_setschedparam(&attr, &param);
	if(pthread_create(&loggerThread, &attr, loggerThreadFunc, NULL))
	{
		fprintf(stderr, "Error creating Logger Thread\n");
		return -1;
	}

	while(loggerThreadActive !=0)
	{

	}

	return 1;
}

void stopLoggerThread()
{
	loggerThreadActive = 0;
}


int loggerThreadSend(QelementData data)
{
	int QStatus 	= 0;

	if(loggerQ)
	{

		pthread_mutex_lock (&loggerQmut);
		QStatus = Enqueue(loggerQ, data);
		pthread_cond_signal(&loggerQcon);
		pthread_mutex_unlock (&loggerQmut);
	}

	return QStatus;
}

int getLoggerQueueSize()
{
	return loggerQ->size;
}
