#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packetDefs.h"
#include "callSign.h"
#include "queue.h"
#include "logger.h"
#include "checksum.h"
#include "webClientThread.h"

int processCallSignPacket(unsigned char * infoPacket, int len)
{
	int result = 1;
    uint16_t  crc16;
	struct HABPacketCallSignDataType HABPacketCallSignData;
	struct webHABPacketDataType      webHABPacketData;

	QelementData callsignQData;

	if(len > sizeof(HABPacketCallSignData))
	{
		printf("ERROR processCallSignPacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketCallSignData, '\0', sizeof(HABPacketCallSignData) );
	    memcpy(&HABPacketCallSignData,infoPacket,len);
		crc16 = crc_16((unsigned char *)&HABPacketCallSignData,len - sizeof(HABPacketCallSignData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketCallSignData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketCallSignData, len);
    		crc16 = crc_16((unsigned char *)&HABPacketCallSignData,len - sizeof(HABPacketCallSignData.crc16) - NPAR);
    		if(crc16 != HABPacketCallSignData.crc16)
    		{
    			result = 0;
    			printf("Bad HABPacketCallSignData\n");
    		}
		}

		if(result)
		{
			sprintf((char *)webHABPacketData.webData,"$CALL %s", HABPacketCallSignData.callSignData);
			callsignQData.len = strlen((const char *)webHABPacketData.webData);
			if(callsignQData.len > sizeof(webHABPacketData.webData))
			{
				printf("ERROR MAX Call Sign Len exceeded\n");
			}
			else
			{
				callsignQData.buf = calloc(callsignQData.len,sizeof(unsigned char));
				memcpy(callsignQData.buf,webHABPacketData.webData,callsignQData.len);
				result = loggerThreadSend(callsignQData);

				webHABPacketData.packetType = HABPacketCallSignData.packetType;
				webHABPacketData.webDataLen = callsignQData.len;
				result = sendToGatewayServer(webHABPacketData);
				printf("%s\n",webHABPacketData.webData);
			}
		}
	}

	return result;
}
