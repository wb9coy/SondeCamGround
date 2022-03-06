#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "queue.h"
#include <pthread.h>
#include "packetDefs.h"
#include "imageFileManager.h"
#include "rxThread.h"
#include "sensor.h"
#include "gps.h"
#include "habInfo.h"
#include "callSign.h"
#include "batt.h"
#include "rssi.h"
#include "modem.h"

static int rxThreadActive	     = 0;
static pthread_mutex_t rxQmut    = PTHREAD_MUTEX_INITIALIZER;

static struct sched_param	param;
static pthread_attr_t 		attr;
static pthread_t rxThread;

static pthread_cond_t  rxQcon    = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t rxQconMut = PTHREAD_MUTEX_INITIALIZER;
static Queue *rxQ;

void *rxThreadFunc(void *x_void_ptr)
{
	int rxQueueSize =10000;

	int rssiTic =0;

	int QStatus;
	int dispatchStatus = 0;
	QelementData rxThreadQData;

	struct rxThreadDataType rxThreadData;

	printf("Start RX Thread  \n");
	rxQ = createQueue(rxQueueSize);
	rxThreadActive = 1;
	while (rxThreadActive)
	{
		pthread_mutex_lock (&rxQconMut);
		pthread_cond_wait(&rxQcon, &rxQconMut);
		pthread_mutex_unlock (&rxQconMut);

		while(rxQ->size > 0)
		{
			pthread_mutex_lock (&rxQmut);
			memset( &rxThreadQData, '\0', sizeof(rxThreadQData) );
			QStatus = Dequeue(rxQ,&rxThreadQData);
			pthread_mutex_unlock (&rxQmut);

			if(QStatus != 0)
			{
				memcpy(&rxThreadData,rxThreadQData.buf,rxThreadQData.len);

				rssiTic++;
				if(rssiTic % 15 == 0)
				{
					processRSSI(rxThreadData.rssi);

				}

				dispatchStatus = rxThreadDispatchPacket(rxThreadData.buf,rxThreadData.size);
				if(dispatchStatus != 1)
				{
					printf("ERROR failed rxThreadDispatchPacket\n");
				}

				free(rxThreadQData.buf);
			}
		}
	}

	return EXIT_SUCCESS;
}

int startRxThread()
{
	int 				rc =0;

	rc = pthread_attr_init(&attr);
	if(rc != 0)
	{
		fprintf(stderr, "Error Rx pthread_attr_init failed\n");
		return -1;
	}
	rc = pthread_attr_getschedparam (&attr, &param);
	if(rc != 0)
	{
		fprintf(stderr, "Error Rx pthread_attr_getschedparam failed\n");
		return -1;
	}
	param.sched_priority = 0;
	rc = pthread_attr_setschedparam(&attr, &param);
	if(pthread_create(&rxThread, &attr, rxThreadFunc, NULL))
	{
		fprintf(stderr, "Error creating Rx Thread\n");
		return -1;
	}

	while(rxThreadActive != 0)
	{

	}

	return 1;
}

void stopRxThread()
{
	rxThreadActive = 0;
}

int rxQueuePacket(struct rxThreadDataType rxThreadData)
{
	int QStatus 	= 1;
	QelementData rxQData;

	rxQData.len = sizeof(rxThreadData);
	rxQData.buf = calloc(rxQData.len,sizeof(unsigned char));
	memcpy(rxQData.buf,&rxThreadData,rxQData.len);
	//printf("rxQueuePacket\n");
//	for(int i=0;i<rxQData.len;i++)
//	{
//		printf("%x ",rxQData.buf[i]);
//	}
//	printf("\n");
	pthread_mutex_lock (&rxQmut);
	QStatus = Enqueue(rxQ, rxQData);
	pthread_cond_signal(&rxQcon);
	pthread_mutex_unlock (&rxQmut);

	return QStatus;
}


int rxThreadDispatchPacket(unsigned char *rxThreadDispatchPacketBuf,int len)
{

	int status 						   = 1;
	uint8_t  packetType;

	memcpy(&packetType,rxThreadDispatchPacketBuf,1);
	switch(packetType)
	{
		case START_IMAGE:
		case IMAGE_DATA:
		case END_IMAGE:
			status = processImageFilePacket(rxThreadDispatchPacketBuf,len,packetType);
			if(status != 1)
			{
				printf("ERROR Failed processImageFilePacket type %d\n",packetType);
			}
			break;
		case GPS_GGA_1:
		case GPS_GGA_2:
		case GPS_RMC_1:
		case GPS_RMC_2:
			//printf("GPS Packet Type %d\n",packetType);
			status = processGPSPacket(rxThreadDispatchPacketBuf,len,packetType);
			if(status != 1)
			{
				printf("ERROR Failed processGPSPacket type %d\n",packetType);
			}
			break;
		case INT_TEMP:
			//printf("INT TEMP\n");
			status = processInternalTempPacket(rxThreadDispatchPacketBuf,len,packetType);
			if(status != 1)
			{
				printf("ERROR Failed processSensorPacket type %d\n",packetType);
			}
			break;
		case HUM:
			printf("HUM\n");
			break;
		case PRES:
			printf("PRES\n");
			break;
		case INFO_DATA:
			status = processInfoPacket(rxThreadDispatchPacketBuf,len);
			if(status != 1)
			{
				printf("ERROR Failed processInfoPacketStatus type %d\n",packetType);
			}
			break;
		case CW_ID:
			status = processCallSignPacket(rxThreadDispatchPacketBuf,len);
			if(status != 1)
			{
				printf("ERROR Failed processcwPacket type %d\n",packetType);
			}
			break;
		case BATT_INFO:
			status = processBattInfoDataPacket(rxThreadDispatchPacketBuf,len);
			if(status != 1)
			{
				printf("ERROR Failed processcwPacket type %d\n",packetType);
			}
			break;

		default:
			status = 0;
			printf("ERROR rxThreadDispatchPacket invalid packetType %d\n",packetType);
	}

	return status;
}

int getRxQueueSize()
{
	return rxQ->size;
}
