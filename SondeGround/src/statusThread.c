#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include "queue.h"
#include "logger.h"
#include "packetDefs.h"
#include "statusThread.h"
#include "webClientThread.h"
#include "utils.h"
#include "modem.h"

int statusThreadActive		= 0;

static int activityFlag     = 0;
static pthread_cond_t 	statusCon    = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t  statusConMut = PTHREAD_MUTEX_INITIALIZER;

void *statusThreadFunc(void *x_void_ptr)
{
	int result             = 1;
	//uint8_t st0;

	QelementData statusQData;

	struct webHABPacketDataType webHABPacketData;

	printf("Start Status Thread  \n");

	statusThreadActive = 1;
	while (statusThreadActive)
	{
		pthread_mutex_lock (&statusConMut);
		pthread_cond_wait(&statusCon, &statusConMut);
		pthread_mutex_unlock (&statusConMut);
		//printf("After Wait\n");

		if(activityFlag == 0)
		{
			writeRegister(getSPID(),REG_OP_MODE, FSK_STANDBY_MODE);
			//st0 = readRegister(getSPID(),REG_OP_MODE);
			//printf("REG_OP_MODE %d\n",st0);

			writeRegister(getSPID(),REG_OP_MODE, FSK_RX_MODE);
			//st0 = readRegister(getSPID(),REG_OP_MODE);
			//printf("REG_OP_MODE %d\n",st0);
		}

		activityFlag = 0;

		sprintf((char *)webHABPacketData.webData,"$PING_GW%d", getGWID());
		statusQData.len = strlen((const char *)webHABPacketData.webData);
		if(statusQData.len > sizeof(webHABPacketData.webData))
		{
			printf("ERROR MAX_STATUS_LEN exceeded\n");
			result = 0;
		}
		else
		{
			statusQData.buf = calloc(statusQData.len,sizeof(unsigned char));
			memcpy(statusQData.buf,webHABPacketData.webData,statusQData.len);
			result = loggerThreadSend(statusQData);
			if(result != 1)
			{
				printf("ERROR statusThreadFunc Logger Failed\n");
			}

			webHABPacketData.packetType    = PING;
			webHABPacketData.webDataLen = statusQData.len;
			result = sendToGatewayServer(webHABPacketData);
			//printf("%s\n",webHABPacketData.webData);

		}

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

void setAtivityFlag()
{
	activityFlag = 1;
}


void signalStatusThread()
{
	pthread_cond_signal(&statusCon);
	//printf("signalStatusThread\n");
}
