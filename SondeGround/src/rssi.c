#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packetDefs.h"
#include "queue.h"
#include "logger.h"
#include "rssi.h"
#include "utils.h"
#include "webClientThread.h"
#include "SX1278FSK.h"
#include "modem.h"


    //printf("RSSI = %.1f\n",RSSI);

int processRSSI(float RSSI)
{
	int result = 1;
	uint8_t gwID;

	struct webHABPacketDataType webHABPacketData;
	QelementData                rssiQData;

	gwID    =  getGWID();
	sprintf((char *)webHABPacketData.webData,"$RSSI_GW%d %.1f", gwID,RSSI);
	rssiQData.len = strlen((const char *)webHABPacketData.webData);
	if(rssiQData.len > sizeof(webHABPacketData.webData))
	{
		printf("ERROR MAX RSSI Len exceeded\n");
	}
	else
	{
		rssiQData.buf = calloc(rssiQData.len,sizeof(unsigned char));
		memcpy(rssiQData.buf,webHABPacketData.webData,rssiQData.len);
		result = loggerThreadSend(rssiQData);

		webHABPacketData.packetType = RSSI_INFO;
		webHABPacketData.webDataLen = rssiQData.len;
		result = sendToGatewayServer(webHABPacketData);
		printf("                      %s\n",webHABPacketData.webData);
	}

	return result;
}

