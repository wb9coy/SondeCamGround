#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "queue.h"
#include <pthread.h>
#include "packetDefs.h"
#include <libwebsockets.h>
#include <webClientThread.h>

static int webClientThreadActive	             = 0;
static pthread_cond_t  webClientQcon             = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t webClientQmut             = PTHREAD_MUTEX_INITIALIZER;

static struct sched_param	param;
static pthread_attr_t 		attr;
static pthread_t webClientThread;

static pthread_mutex_t webClientThreadFuncConMut = PTHREAD_MUTEX_INITIALIZER;

static Queue *webClientQ;

static struct lws *web_socket = NULL;
static struct lws_context *context;
static struct lws_context_creation_info info;
static struct lws_client_connect_info ccinfo = {0};

int connectedToServer = 0;


void *webClientThreadFunc(void *x_void_ptr)
{
	int webClientQueueSize =10000;
	int connectStatus = 0;

	printf("Start Web Client Thread  \n");
	webClientQ = createQueue(webClientQueueSize);

	webClientThreadActive = 1;
	while (webClientThreadActive)
	{
		while(connectedToServer == 0)
		{
			connectStatus = connectToServer();
			printf("Trying to connect to %s\n",ccinfo.address);
			if(connectStatus)
			{
				lws_service( context, 10000 );
			}
			if(connectedToServer)
			{
				printf("Web Socket Connected to %s\n",ccinfo.address);
			}
			else
			{
				sleep(10);
			}
		}
		pthread_mutex_lock (&webClientThreadFuncConMut);
		pthread_cond_wait(&webClientQcon, &webClientThreadFuncConMut);
		pthread_mutex_unlock (&webClientThreadFuncConMut);

		while( (webClientQ->size > 0) && (connectedToServer == 1))
		{
			lws_callback_on_writable( web_socket );
			lws_service( context, 1000);
		}
	}

	return EXIT_SUCCESS;
}

int startwebClientThread()
{

	int 				rc =0;

	rc = pthread_attr_init(&attr);
	if(rc != 0)
	{
		fprintf(stderr, "Error webClient pthread_attr_init failed\n");
		return -1;
	}
	rc = pthread_attr_getschedparam (&attr, &param);
	if(rc != 0)
	{
		fprintf(stderr, "Error webClient pthread_attr_getschedparam failed\n");
		return -1;
	}
	param.sched_priority = 0;
	rc = pthread_attr_setschedparam(&attr, &param);
	if(pthread_create(&webClientThread, &attr, webClientThreadFunc, NULL))
	{
		fprintf(stderr, "Error creating webClient Thread\n");
		return -1;
	}

	while(webClientThreadActive != 0)
	{

	}

	return 1;
}

void stopwebClientThread()
{
	webClientThreadActive = 0;
}


int getwebClientQueueSize()
{
	return webClientQ->size;
}


static int callback_hab( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	int QStatus;
	uint8_t   packetType;
	QelementData webClientThreadQData;
	//printf("Reason %d\n",reason);
	switch( reason )
	{
		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
			break;
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable( wsi );
			connectedToServer = 1;
			break;
		case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
			lws_callback_on_writable( wsi );
			connectedToServer = 1;
			break;
		case LWS_CALLBACK_CLIENT_RECEIVE:
			/* Handle incomming messages here. */
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			struct webHABPacketDataType 	webHABPacketData;

			pthread_mutex_lock (&webClientQmut);
			QStatus = Dequeue(webClientQ,&webClientThreadQData);
			pthread_mutex_unlock (&webClientQmut);
			if(QStatus != 0)
			{
				memcpy(&packetType,&webClientThreadQData.buf[0],1);
				//printf("Packet Type %d\n", packetType);

				switch(packetType)
				{
					case GPS_GGA_2:
					case GPS_RMC_2:
						//printf("WSC GPS\n");
						memcpy(&webHABPacketData, webClientThreadQData.buf,webClientThreadQData.len);
						//printf("lws_write %s\n",webHABPacketData.webData);
						lws_write( wsi, webHABPacketData.webData, webHABPacketData.webDataLen, LWS_WRITE_TEXT );
	//					printf("lws len %d\n",webHABPacketGPSData.gpsDataLen);
	//					printf("lws %s\n",webHABPacketGPSData.gpsData);
						break;
					case INT_TEMP:
						//printf("WSC INT_TEMP\n");
						memcpy(&webHABPacketData, webClientThreadQData.buf,webClientThreadQData.len);
						//printf("lws_write %s\n",webHABPacketData.webData);
						lws_write( wsi, webHABPacketData.webData, webHABPacketData.webDataLen, LWS_WRITE_TEXT );
						break;
					case HUM:
						break;
					case PRES:
						break;
					case BATT_INFO:
						//printf("WSC BATT_INFO\n");
						memcpy(&webHABPacketData, webClientThreadQData.buf,webClientThreadQData.len);
						//printf("lws_write %s\n",webHABPacketData.webData);
						lws_write( wsi, webHABPacketData.webData, webHABPacketData.webDataLen, LWS_WRITE_TEXT );
						break;
					case RSSI_INFO:
						//printf("WSC BATT_INFO\n");
						memcpy(&webHABPacketData, webClientThreadQData.buf,webClientThreadQData.len);
						//printf("lws_write %s\n",webHABPacketData.webData);
						lws_write( wsi, webHABPacketData.webData, webHABPacketData.webDataLen, LWS_WRITE_TEXT );
						break;
				}
				free(webClientThreadQData.buf);
			}
			break;
		}

		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			connectedToServer = 0;
			break;
		case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
		case LWS_CALLBACK_LOCK_POLL:
		case LWS_CALLBACK_UNLOCK_POLL:
			break;
		default:
			break;
	}

	return 0;
}

enum protocols
{
	PROTOCOL_HAB = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
{
	{
		"hav-protocol",
		callback_hab,
		0,
		HAB_RX_BUFFER_BYTES,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

int connectToServer()
{

	int connectStatus = 0;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	context = lws_create_context( &info );

	ccinfo.context = context;
	ccinfo.address = getGatewayServerIPAddress();
	ccinfo.port = getGatewayServerPort();
	ccinfo.path = "/";
	ccinfo.host = lws_canonical_hostname( context );
	ccinfo.origin = "origin";
	ccinfo.protocol = protocols[PROTOCOL_HAB].name;
	//printf("connectToServer Before lws_client_connect_via_info(&ccinfo) \n");
	web_socket = lws_client_connect_via_info(&ccinfo);
	ccinfo.parent_wsi = web_socket;
	//printf("connectToServer Before lws_service( context, 10000 \n");
	lws_service( context, 1000);

	if(web_socket)
	{
		connectStatus = 1;
	}


//	lws_service( context, /* timeout_ms = */ 250 );
//
//	strcpy(geneBuff,"Hello World");
//	while( loop)
//	{
//		//lws_write( web_socket, geneBuff, 11, LWS_WRITE_TEXT );
//		lws_callback_on_writable( web_socket );
//		lws_service( context, /* timeout_ms = */ 250 );
//	}
//
//	lws_context_destroy( context );

	return connectStatus;
}

int sendToGatewayServer(struct webHABPacketDataType webHABPacketData)
{
	int status =1;
	int QStatus;
	QelementData webClientThreadQData;

	//printf("In sendToGatewayServer%s\n",webHABPacketData.webData);

	if(webClientQ)
	{
		if(connectedToServer)
		{
			webClientThreadQData.len = sizeof(webHABPacketData);
			webClientThreadQData.buf = calloc(webClientThreadQData.len,sizeof(unsigned char));
			//printf("BEFORE sendToGatewayServer memcpy(webClientThreadQData.buf,sendToGatewayServerBuf,webClientThreadQData.len)\n");
			memcpy(webClientThreadQData.buf,&webHABPacketData,webClientThreadQData.len);
			//printf("AFTER sendToGatewayServer memcpy(webClientThreadQData.buf,sendToGatewayServerBuf,webClientThreadQData.len)\n");
			pthread_mutex_lock (&webClientQmut);
			//printf("AFTER sendToGatewayServer pthread_mutex_lock\n");

			QStatus = Enqueue(webClientQ, webClientThreadQData);
			//printf("Enqueue %s\n",webClientThreadQData.buf);
			//printf("AFTER Enqueue(webClientQ, webClientThreadQData)\n");
			if(QStatus != 1)
			{
				printf("ERROR sendToGatewayServer Failed enqueue\n");
			}
			//printf("BEFORE sendToGatewayServer pthread_cond_signal(&webClientQcon)\n");
			pthread_cond_signal(&webClientQcon);
			pthread_mutex_unlock (&webClientQmut);
			//printf("AFTER pthread_mutex_unlock (&webClientQmut)\n");
		}
	}

	return status;
}
