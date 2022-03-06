#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packetDefs.h"
#include "callSign.h"
#include "queue.h"
#include "logger.h"
#include "checksum.h"
#include "webClientThread.h"

int processBattInfoDataPacket(unsigned char * infoPacket, int len)
{
	int result = 1;
    uint16_t  crc16;
	struct HABPacketBattInfoDataType HABPacketBattInfoData;
	struct webHABPacketDataType webHABPacketData;

	QelementData battInfoQData;

	if(len > sizeof(HABPacketBattInfoData))
	{
		printf("ERROR processBattInfoDataPacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketBattInfoData, '\0', sizeof(HABPacketBattInfoData) );
	    memcpy(&HABPacketBattInfoData,infoPacket,len);
		crc16 = crc_16((unsigned char *)&HABPacketBattInfoData,len - sizeof(HABPacketBattInfoData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketBattInfoData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketBattInfoData, len);
			crc16 = crc_16((unsigned char *)&HABPacketBattInfoData,len - sizeof(HABPacketBattInfoData.crc16) - NPAR);
			if(crc16 != HABPacketBattInfoData.crc16)
			{
				printf("Bad HABPacketBattInfoData\n");
				result = 0;
			}
		}

	    if(result)
	    {
			sprintf((char *)webHABPacketData.webData,"$BATT %.1fV", HABPacketBattInfoData.battInfoData);
			battInfoQData.len = strlen((const char *)webHABPacketData.webData);
			if(battInfoQData.len > sizeof(webHABPacketData.webData))
			{
				printf("ERROR MAX Batt Data Len exceeded\n");
			}
			else
			{
				battInfoQData.buf = calloc(battInfoQData.len,sizeof(unsigned char));
				memcpy(battInfoQData.buf,webHABPacketData.webData,battInfoQData.len);
				result = loggerThreadSend(battInfoQData);

				webHABPacketData.packetType = HABPacketBattInfoData.packetType;
				webHABPacketData.webDataLen = battInfoQData.len;
				result = sendToGatewayServer(webHABPacketData);
				printf("%s\n",webHABPacketData.webData);
			}
	    }
	}

	return result;
}
