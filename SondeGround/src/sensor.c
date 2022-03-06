#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packetDefs.h"
#include "sensor.h"
#include "queue.h"
#include "logger.h"
#include "checksum.h"
#include "webClientThread.h"


int processInternalTempPacket(unsigned char * sensorPacket, int len, int packetType)
{
	int result = 1;
    uint16_t  crc16;
	struct HABPacketIntTempInfoDataType HABPacketIntTempInfoData;
	struct webHABPacketDataType webHABPacketData;

	QelementData sensorQData;

	if(len > sizeof(HABPacketIntTempInfoData))
	{
		printf("ERROR processSensorPacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketIntTempInfoData, '\0', sizeof(HABPacketIntTempInfoData) );
	    memcpy(&HABPacketIntTempInfoData,sensorPacket,len);

		crc16 = crc_16((unsigned char *)&HABPacketIntTempInfoData,len - sizeof(HABPacketIntTempInfoData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketIntTempInfoData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketIntTempInfoData, len);
			crc16 = crc_16((unsigned char *)&HABPacketIntTempInfoData,len - sizeof(HABPacketIntTempInfoData.crc16) - NPAR);
			if(crc16 != HABPacketIntTempInfoData.crc16)
			{
				printf("Bad HABPacketIntTempInfoData\n");
				result = 0;
			}
		}

		if(result)
		{
			sprintf((char *)webHABPacketData.webData,"$TEMP %dF", HABPacketIntTempInfoData.intTempInfoData);
			sensorQData.len = strlen((const char *)webHABPacketData.webData);
			if(sensorQData.len > sizeof(webHABPacketData.webData))
			{
				printf("ERROR MAX_SENSOR_LEN exceeded\n");
			}
			else
			{
				sensorQData.buf = calloc(sensorQData.len,sizeof(unsigned char));
				memcpy(sensorQData.buf,webHABPacketData.webData,sensorQData.len);
				result = loggerThreadSend(sensorQData);

				webHABPacketData.packetType    = packetType;
				webHABPacketData.webDataLen = sensorQData.len;
				result = sendToGatewayServer(webHABPacketData);
				printf("%s\n",webHABPacketData.webData);
			}
		}
	}

	return result;
}
