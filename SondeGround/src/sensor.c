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
			sprintf((char *)webHABPacketData.webData,"$INT_TEMP %dF", HABPacketIntTempInfoData.intTempInfoData);
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

int processExternalTempPacket(unsigned char * sensorPacket, int len, int packetType)
{
	int result = 1;
    uint16_t  crc16;
	struct HABPacketExtTempInfoDataType HABPacketExtTempInfoData;
	struct webHABPacketDataType webHABPacketData;

	QelementData sensorQData;

	if(len > sizeof(HABPacketExtTempInfoData))
	{
		printf("ERROR processExternalTempPacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketExtTempInfoData, '\0', sizeof(HABPacketExtTempInfoData) );
	    memcpy(&HABPacketExtTempInfoData,sensorPacket,len);

		crc16 = crc_16((unsigned char *)&HABPacketExtTempInfoData,len - sizeof(HABPacketExtTempInfoData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketExtTempInfoData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketExtTempInfoData, len);
			crc16 = crc_16((unsigned char *)&HABPacketExtTempInfoData,len - sizeof(HABPacketExtTempInfoData.crc16) - NPAR);
			if(crc16 != HABPacketExtTempInfoData.crc16)
			{
				printf("Bad HABPacketExtTempInfoData\n");
				result = 0;
			}
		}

		if(result)
		{
			sprintf((char *)webHABPacketData.webData,"$EXT_TEMP %.1fF", HABPacketExtTempInfoData.extTempInfoData);
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

int processPressurePacket(unsigned char * sensorPacket, int len, int packetType)
{
	int result = 1;
    uint16_t  crc16;
	struct HABPacketPressureInfoDataType HABPacketPressureInfoData;
	struct webHABPacketDataType webHABPacketData;

	QelementData sensorQData;

	if(len > sizeof(HABPacketPressureInfoData))
	{
		printf("ERROR processPressurePacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketPressureInfoData, '\0', sizeof(HABPacketPressureInfoData) );
	    memcpy(&HABPacketPressureInfoData,sensorPacket,len);

		crc16 = crc_16((unsigned char *)&HABPacketPressureInfoData,len - sizeof(HABPacketPressureInfoData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketPressureInfoData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketPressureInfoData, len);
			crc16 = crc_16((unsigned char *)&HABPacketPressureInfoData,len - sizeof(HABPacketPressureInfoData.crc16) - NPAR);
			if(crc16 != HABPacketPressureInfoData.crc16)
			{
				printf("Bad HABPacketPressureInfoData\n");
				result = 0;
			}
		}

		if(result)
		{
			sprintf((char *)webHABPacketData.webData,"$PRES %.1fPa", HABPacketPressureInfoData.pressureInfoData);
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

int processHumidityPacket(unsigned char * sensorPacket, int len, int packetType)
{
	int result = 1;
    uint16_t  crc16;
	struct HABPacketHumidityInfoDataType HABPacketHumidityInfoData;
	struct webHABPacketDataType webHABPacketData;

	QelementData sensorQData;

	if(len > sizeof(HABPacketHumidityInfoData))
	{
		printf("ERROR processHumidityPacket len error\n");
		result = 0;
	}
	else
	{
		memset( &HABPacketHumidityInfoData, '\0', sizeof(HABPacketHumidityInfoData) );
	    memcpy(&HABPacketHumidityInfoData,sensorPacket,len);

		crc16 = crc_16((unsigned char *)&HABPacketHumidityInfoData,len - sizeof(HABPacketHumidityInfoData.crc16) - NPAR);
		//printf("Packet CRC %x Calc CRC %x\n",HABPacketCallSignData.crc16,crc16);
		if(crc16 != HABPacketHumidityInfoData.crc16)
		{
			printf("RS Packet\n");
			rscode_decode((unsigned char *)&HABPacketHumidityInfoData, len);
			crc16 = crc_16((unsigned char *)&HABPacketHumidityInfoData,len - sizeof(HABPacketHumidityInfoData.crc16) - NPAR);
			if(crc16 != HABPacketHumidityInfoData.crc16)
			{
				printf("Bad HABPacketHumidityInfoData\n");
				result = 0;
			}
		}

		if(result)
		{
			sprintf((char *)webHABPacketData.webData,"$HUM %.1fRH", HABPacketHumidityInfoData.humidityInfoData);
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
