#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packetDefs.h"
#include "habInfo.h"
#include "queue.h"
#include "logger.h"
#include "checksum.h"
#include "webClientThread.h"

int processInfoPacket(unsigned char * infoPacket, int len)
{

	int result = 1;
    uint16_t  crc16;
	struct HABPacketInfoDataType HABPacketInfoData;
	struct webHABPacketDataType  webHABPacketData;

	QelementData infoQData;

	if(len != sizeof(HABPacketInfoData))
	{
		printf("ERROR processInfoPacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketInfoData, '\0', sizeof(HABPacketInfoData) );
	    memcpy(&HABPacketInfoData,infoPacket,len);
		crc16 = crc_16((unsigned char *)&HABPacketInfoData,len - sizeof(HABPacketInfoData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketInfoData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketInfoData, len);
			crc16 = crc_16((unsigned char *)&HABPacketInfoData,len - sizeof(HABPacketInfoData.crc16) - NPAR);
			if(crc16 != HABPacketInfoData.crc16)
			{
				printf("Bad HABPacketInfoData\n");
				result = 0;
			}
		}

		if(result)
		{
			sprintf((char *)webHABPacketData.webData,"$INFO %s", HABPacketInfoData.infoData);
			infoQData.len = strlen((const char *)webHABPacketData.webData);
			if(infoQData.len > sizeof(webHABPacketData.webData))
			{
				printf("ERROR MAX INFO Len exceeded\n");
				result = 0;
			}
			else
			{
				infoQData.buf = calloc(infoQData.len,sizeof(unsigned char));
				memcpy(infoQData.buf,webHABPacketData.webData,infoQData.len);
				result = loggerThreadSend(infoQData);

				webHABPacketData.packetType = HABPacketInfoData.packetType;
				webHABPacketData.webDataLen = infoQData.len;
				result = sendToGatewayServer(webHABPacketData);
				printf("%s\n",webHABPacketData.webData);
			}
		}
	}

	return result;
}
