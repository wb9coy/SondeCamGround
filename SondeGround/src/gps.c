#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "packetDefs.h"
#include "gps.h"
#include "queue.h"
#include "logger.h"
#include "webClientThread.h"


static uint8_t   lastGGASentence[MAX_GPS_NMEA_SIZE];
static uint8_t   lastRMCSentence[MAX_GPS_NMEA_SIZE];

int processGPSPacket(unsigned char * GPSPacket, int len, int packetType)
{
	int result = 1;

	struct webHABPacketDataType webHABPacketData;
	struct HABPacketGPSDataType HABPacketGPSData;
	static uint8_t   gpsGGAData[MAX_GPS_NMEA_SIZE];
	static int gpsGGADataLen;
	static uint8_t   gpsRMCData[MAX_GPS_NMEA_SIZE];
	static int gpsRMCDataLen;
	uint8_t          gpsSentence[MAX_GPS_NMEA_SIZE];
	int gpsSentenceLen =0;

	QelementData gpsQData;

	if(len > sizeof(HABPacketGPSData))
	{
		printf("ERROR invald HABPacketGPSData\n");
		result = 0;
	}
	else
	{
		memset(&HABPacketGPSData,'\0',sizeof(gpsGGAData));
		memcpy(&HABPacketGPSData,GPSPacket,len);
		if(HABPacketGPSData.gpsDataLen > MAX_GPS_BUF_LEN)
		{
			printf("ERROR HABPacketGPSData.gpsDataLen > MAX_GPS_BUF_LEN\n");
			result = 0;
		}
		else
		{
			switch(packetType)
			{
				case GPS_GGA:
					memset(gpsSentence,'\0',sizeof(gpsSentence));
					memcpy(gpsSentence,HABPacketGPSData.gpsData,HABPacketGPSData.gpsDataLen);
					gpsSentenceLen = HABPacketGPSData.gpsDataLen;
					gpsGGADataLen = 0;
					break;
				case GPS_GGA_1:
				{
					memset(gpsGGAData,'\0',sizeof(gpsGGAData));
					memset(gpsSentence,'\0',sizeof(gpsSentence));
					memcpy(gpsGGAData,HABPacketGPSData.gpsData,HABPacketGPSData.gpsDataLen);
					gpsGGADataLen = HABPacketGPSData.gpsDataLen;
					break;
				}
				case GPS_GGA_2:
				{
					if( (HABPacketGPSData.gpsDataLen + MAX_GPS_HDR_LEN) > sizeof(HABPacketGPSData))
					{
						memset(gpsGGAData,'\0',sizeof(gpsGGAData));
						memset(gpsSentence,'\0',sizeof(gpsSentence));
						printf("ERROR GPS_GGA_2 len\n");
						result = 0;
						gpsGGADataLen = 0;
					}
					else
					{
						if(gpsGGADataLen == 0)
						{
							memset(gpsGGAData,'\0',sizeof(gpsGGAData));
							memset(gpsSentence,'\0',sizeof(gpsSentence));
							printf("ERROR GPS_GGA_2 len equal 0\n");
							result = 0;
						}
						else
						{
							gpsGGADataLen = HABPacketGPSData.gpsDataLen + gpsGGADataLen;
							memcpy(&gpsGGAData[MAX_GPS_BUF_LEN],HABPacketGPSData.gpsData,HABPacketGPSData.gpsDataLen);
							memcpy(gpsSentence,gpsGGAData,gpsGGADataLen);
							memset(lastGGASentence,'\0',sizeof(lastGGASentence));
							memcpy(lastGGASentence,gpsGGAData,gpsGGADataLen);
							gpsSentenceLen = gpsGGADataLen;
							gpsGGADataLen = 0;
						}
					}
					break;
				}
				case GPS_RMC:
					memset(gpsSentence,'\0',sizeof(gpsSentence));
					memcpy(gpsSentence,HABPacketGPSData.gpsData,HABPacketGPSData.gpsDataLen);
					gpsSentenceLen = HABPacketGPSData.gpsDataLen;
					gpsRMCDataLen = 0;
					break;
				case GPS_RMC_1:
				{
					memset(gpsRMCData,'\0',sizeof(gpsRMCData));
					memset(gpsSentence,'\0',sizeof(gpsSentence));
					memcpy(gpsRMCData,HABPacketGPSData.gpsData,HABPacketGPSData.gpsDataLen);
					gpsRMCDataLen = HABPacketGPSData.gpsDataLen;
					break;
				}
				case GPS_RMC_2:
				{
					if( (HABPacketGPSData.gpsDataLen + MAX_GPS_HDR_LEN) > sizeof(HABPacketGPSData))
					{
						memset(gpsGGAData,'\0',sizeof(gpsGGAData));
						memset(gpsSentence,'\0',sizeof(gpsSentence));
						printf("ERROR GPS_GGA_2 len\n");
						result = 0;
						gpsRMCDataLen = 0;
					}
					else
					{
						if(gpsRMCDataLen == 0)
						{
							memset(gpsRMCData,'\0',sizeof(gpsRMCData));
							memset(gpsSentence,'\0',sizeof(gpsSentence));
							printf("ERROR GPS_RMC_2 len equal 0\n");
							result = 0;
						}
						else
						{
							gpsRMCDataLen = HABPacketGPSData.gpsDataLen + gpsRMCDataLen;
							memcpy(&gpsRMCData[MAX_GPS_BUF_LEN],HABPacketGPSData.gpsData,HABPacketGPSData.gpsDataLen);
							memcpy(gpsSentence,gpsRMCData,gpsRMCDataLen);
							memset(lastRMCSentence,'\0',sizeof(lastRMCSentence));
							memcpy(lastRMCSentence,gpsRMCData,gpsRMCDataLen);
							gpsSentenceLen = gpsRMCDataLen;
							gpsRMCDataLen = 0;
						}
					}
					break;
				}
			}
		}
	}

	if(result)
	{
		switch(packetType)
		{
			case GPS_GGA:
			case GPS_RMC:
			case GPS_GGA_2:
			case GPS_RMC_2:
			if(result == 1)
			{
				result = validateNMEAChecksum((char *)gpsSentence,gpsSentenceLen);
				if(result != 1)
				{
					printf("ERROR GPS check sum failed\n");
					result = 0;
				}
				else
				{
					gpsQData.len = gpsSentenceLen;
					gpsQData.buf = calloc(gpsQData.len,sizeof(unsigned char));

					memset(gpsQData.buf,'\0',gpsQData.len);
					memcpy(gpsQData.buf,gpsSentence,gpsQData.len-2);

					result = loggerThreadSend(gpsQData);

					webHABPacketData.packetType = packetType;
					webHABPacketData.webDataLen = gpsSentenceLen;
					memcpy(webHABPacketData.webData,gpsSentence,webHABPacketData.webDataLen);
					result = sendToGatewayServer(webHABPacketData);
				}
			}
			gpsRMCDataLen = 0;
			gpsGGADataLen = 0;
			break;
		}
	}

	return result;
}

int validateNMEAChecksum(char *buf, int len)
{
    char ch;
    unsigned int calculatedCheckSum = 0;
    int givenCheckSum               = 0;
    int stopFlag                    = 0;
    int errorFlag                   = 0;
    int result                      = 0;
    int i;              // loop counter

    //foreach(char Character in sentence)
    for (i=0;stopFlag==0;++i)
    {
    	if(i>len)
    	{
    		errorFlag = 1;
    		break;
    	}
        ch = buf[i];
        switch(ch)
        {
            case '$':
                // Ignore the dollar sign
                break;
            case '*':
                // Stop processing before the asterisk
            	stopFlag = 1;
                continue;
            default:
                // Is this the first value for the checksum?
                if (calculatedCheckSum == 0)
                {
                    // Yes. Set the checksum to the value
                	calculatedCheckSum = ch;
                }
                else
                {
                    // No. XOR the checksum with this character's value
                	calculatedCheckSum = calculatedCheckSum ^ ch;
                }
                break;
        }
    }

    if(errorFlag != 1)
    {
        sscanf(&buf[i], "%2x", &givenCheckSum);
        if(givenCheckSum == calculatedCheckSum)
        {
        	result = 1;
        }
    }

    return (result);
}

int getLastGGA(uint8_t buf[])
{
    int result = 0;

    memcpy(buf,lastGGASentence,strlen((const char *)lastGGASentence));

    return result;
}

int getLastRMC(uint8_t buf[])
{
    int result = 0;

    memcpy(buf,lastRMCSentence,strlen((const char *)lastRMCSentence));

    return result;
}
