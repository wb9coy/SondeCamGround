
#include <pthread.h>
#include <pigpio.h>
#include "modem.h"
#include "rxThread.h"
#include "packetDefs.h"
#include "utils.h"
#include <unistd.h>

static GFSK_ctl modem;
struct rxThreadDataType rxThreadData;
//static float plusOffsetLmit  =  2000.0;
//static float minusOffsetLmit = -2000.0;
//static float minusOffset = 0;
//static float plusOffset  = 0;

void rxDoneISRf(int gpio_n, int level, uint32_t tick, void *modemptr)
{
    GFSK_ctl *modem = (GFSK_ctl *)modemptr;
	int goodPacket = 1;
	int status     = 1;

//	float adjustedFreq        = 0;
//	int32_t afcVal;
	int16_t rssiVal;
	//uint16_t LNA;

//    memset(modem->rx.data.buf,'\0',MTU_SIZE);
//	printf("Before read\n");
//	for(int i=0; i < MTU_SIZE; i++)
//	{
//		printf("%x ",modem->rx.data.buf[i]);
//	}
//	printf("\n");
//    LNA = readRegister(modem->spid, REG_LNA);
//    printf("LNA %x\n",LNA);

	usleep(400);
	rssiVal = getRSSI(getSPID());
	rxThreadData.rssi = rssiVal/2.0-22.0;

	if(rxThreadData.rssi < 105.00)
	{

		for(int i=0; i < MTU_SIZE; i++)
		{
			rxThreadData.buf[i] = readRegister(modem->spid,REG_FIFO);
		}

//		afcVal = getAFC(getSPID());
//
//		if(plusOffset < plusOffsetLmit)
//		{
//			if(afcVal > 400)
//			{
//				printf("afcVal = %d \n",afcVal);
//				plusOffset = plusOffset + 50;
//				adjustedFreq = modem->eth.freq + modem->eth.freqofs + plusOffset;
//				setFrequency(getSPID(),adjustedFreq);
//				printf("*********AFC +50\n");
//			}
//		}
//		else
//		{
//			plusOffset = 0;
//			printf("afcVal = %d \n",afcVal);
//			printf("Plus AFC adjust limit of %f reached  adjust offset in ini file\n",plusOffsetLmit);
//		}
//
//		if(minusOffset > minusOffsetLmit)
//		{
//			if(afcVal < -400)
//			{
//				printf("afcVal = %d \n",afcVal);
//				minusOffset = minusOffset - 50;
//				adjustedFreq = modem->eth.freq + modem->eth.freqofs + minusOffset;
//				setFrequency(getSPID(),adjustedFreq);
//				printf("*********AFC -50\n");
//			}
//
//		}
//		else
//		{
//			minusOffset = 0;
//			printf("afcVal = %d \n",afcVal);
//			printf("Minus AFC adjust limit of %f reached  adjust offset in ini file\n",minusOffsetLmit);
//		}

//	    LNA = readRegister(modem->spid, REG_LNA);
//	    printf("LNA3 %x\n",LNA);

		//printf("end\n");

		switch(rxThreadData.buf[0])
		{
			case IMAGE_DATA:
			{
				rxThreadData.size = sizeof(struct HABPacketImageDataType);
				break;
			}
			case CW_ID:
			{
				rxThreadData.size = sizeof(struct HABPacketCallSignDataType);
				break;
			}

			case BATT_INFO:
			{
				rxThreadData.size = sizeof(struct HABPacketBattInfoDataType);
				break;
			}

			case INT_TEMP:
			{
				rxThreadData.size = sizeof(struct HABPacketIntTempInfoDataType);
				break;
			}

			case EXT_TEMP:
			{
				rxThreadData.size = sizeof(struct HABPacketExtTempInfoDataType);
				break;
			}

			case PRESS_INFO:
			{
				rxThreadData.size = sizeof(struct HABPacketPressureInfoDataType);
				break;
			}
			case HUM_INFO:
			{
				rxThreadData.size = sizeof(struct HABPacketHumidityInfoDataType);
				break;
			}
			case INFO_DATA:
			{
				rxThreadData.size = sizeof(struct HABPacketInfoDataType);
				break;
			}

			case GPS_GGA:
			case GPS_GGA_1:
			case GPS_GGA_2:
			case GPS_RMC:
			case GPS_RMC_1:
			case GPS_RMC_2:
			{
				rxThreadData.size = MAX_GPS_BUF_LEN + MAX_GPS_HDR_LEN;
				break;
			}
			case START_IMAGE:
			{
				rxThreadData.size = sizeof(struct HABPacketImageStartType);
				break;
			}
			case END_IMAGE:
			{
				rxThreadData.size = sizeof(struct HABPacketImageEndType);
				break;
			}
			default:
			{
				rxThreadData.size = 0;
				printf("\n");
				printf("Default\n");
				for(int i=0; i < MTU_SIZE; i++)
				{
					printf("%x ",rxThreadData.buf[i]);
				}
				printf("\n");
				goodPacket = 0;
			}
		}

		if(goodPacket)
		{
//			rxThreadData.rssi = modem->rx.rssi;
//			rxThreadData.size = modem->rx.data.size;
//			memcpy(rxThreadData.buf,modem->rx.data.buf,rxThreadData.size);
			status = rxQueuePacket(rxThreadData);
			if(status != 1)
			{
				printf("Failed rxQueuePacket\n");
			}
//			pthread_create(&(modem->rx.cbThread), NULL, startRxCallback, (void *)(modem));
//			pthread_join(modem->rx.cbThread,NULL);
		}
	}//if(RSSI < 97.00)


    //writeRegister(modem->spid,REG_OP_MODE, FSK_STANDBY_MODE);
    clearIRQFlags(modem->spid);
    //writeRegister(modem->spid,REG_OP_MODE, FSK_RX_MODE);

	//printf("end\n");
}

int modemReceive(GFSK_ctl *modem)
{
	int status =1;
	uint8_t st0;


//	sx1278fsk.writeRegister(modem->spid, REG_SEQ_CONFIG1, 0x88);
//	sx1278fsk.writeRegister(modem->spid, REG_SEQ_CONFIG2, 0x24);

	setPayloadLength(modem->spid,MTU_SIZE);

	status = set_dio_rx_mapping(modem->spid);
	if(status != 1)
	{
		printf("Rx register mapping  FAILED\n");
		return status;
	}

	//
	setRxDoneDioISR(modem->eth.dio0GpioN, rxDoneISRf, modem);
	printf("dio0GpioN %d\n",modem->eth.dio0GpioN);

//	sx1278fsk.writeRegister(modem->spid, REG_PACKET_CONFIG1, 0x09);
//	sx1278fsk.writeRegister(modem->spid, 0x27, 0xC8);

	st0 = readRegister(modem->spid,REG_OP_MODE);
	printf("modemReceive 1 %d\n",st0);

	//sx1278fsk.writeRegister(modem->spid, REG_SEQ_CONFIG1, 0x40);

	writeRegister(modem->spid, REG_OP_MODE, FSK_RX_MODE);
	writeRegister(modem->spid, REG_OP_MODE, FSK_RX_MODE);

	st0 = readRegister(modem->spid,REG_OP_MODE);
	printf("modemReceive 2 %d\n",st0);

	return status;
}

int modemSetup(dictionary *ini)
{
	int status = 0;
	float temp;
	uint8_t sync;
	int maxCurrent;

    if (gpioInitialise() < 0)
    {
        printf("Pigpio init error\n");
        return status;
    }
	usleep(1000000);

	modem.spiCS = iniparser_getint(ini,"modem:spiCS",1);

    modem.spid = spiOpen(modem.spiCS, 32000, 0);
	if(modem.spid < 0)
	{
		printf("spiOpen FAILED\n");
		return -1;
	}
	usleep(1000000);

	modem.eth.resetGpioN      = iniparser_getint(ini,"modem:resetGpioN",4);
	modem.eth.dio0GpioN       = iniparser_getint(ini,"modem:dio0GpioN",25);
	modem.eth.bps             = iniparser_getint(ini,"modem:bps",0);
	modem.eth.resetGpioN      = iniparser_getint(ini,"modem:resetGpioN",4);
	modem.eth.freq            = iniparser_getint(ini,"modem:freq",434500000);
	modem.eth.freqofs         = iniparser_getint(ini,"modem:offset",0);
	modem.eth.afcbw           = 31300;
	modem.eth.rxbw            = 20800;


	status = resetChip(modem.eth.resetGpioN);
	if(status != 1)
	{
		printf("Reset Chip Failed\n");
		return 0;
	}
	else
	{
		printf("Reset SX1278 Chip Success\n");
	}

	status = setMaxCurrent(modem.spid,0x1B);
	if(status != 1)
	{
		printf("Setting SX1278 power on FAILED state = %d\n",status);
		return 0;
	}

	maxCurrent = getMaxCurrent(modem.spid);
	printf("Max current set to %d\n",maxCurrent);

	status = setFSK(modem.spid);
	if(status != 0)
	{
		printf("Setting FSK mode FAILED\n");
		return status;
	}

	status = setBitrate(modem.spid,modem.eth.bps);
	if(status != 1)
	{
		printf("Setting bitrate FAILED\n");
		return status;
	}

	temp = getBitrate(modem.spid);
	printf("Bit Rate %f\n",temp);

	status = setAFCBandwidth(modem.spid,modem.eth.afcbw);
	if(status != 1)
	{
		printf("Setting AFC bandwidth %d Hz FAILED\n", modem.eth.afcbw);
		return status;
	}

	status = setRxBandwidth(modem.spid,modem.eth.rxbw);
	if(status != 1)
	{
		printf("Setting RX bandwidth to %d Hz FAILED\n", modem.eth.rxbw);
		return status;
	}

	// Enable auto-AFC, auto-AGC, RX Trigger by preamble
	status = setRxConf(modem.spid,0x1E);
	if(status != 1)
	{
		printf("Setting RX Config FAILED\n");
		return status;
	}

	const char *SYNC="\x08\x6D\x53\x88";
	status = setSyncConf(modem.spid,0x53, 4, (const uint8_t *)SYNC);
	if(status != 1)
	{
		printf("Setting SYNC Config FAILED\n");
		return status;
	}

	sync = getSyncConf(modem.spid);
	printf("Sync %d\n",sync);

	status =  setPreambleLength(modem.spid, 8);
	if(status != 0)
	{
		printf("Setting Preamble Len FAILED\n");
		return status;
	}

	//tatus = setPreambleDetect(modem.spid,0xA8);
	status = setPreambleDetect(modem.spid,0xAA);
	if(status != 1)
	{
		printf("Setting PreambleDetect FAILED\n");
		return status;
	}

	// CTT CRC
	status = setPacketConfig(modem.spid,0x08,0x40);
	//status = sx1278fsk.setPacketConfig(modem->spid,0x18,0x40);
	if(status != 1)
	{
		printf("Setting Packet config FAILED\n");
		return status;
	}

	status = setFrequency(modem.spid,modem.eth.freq);
	if(status != 1)
	{
		//printf("Setting freq %f FAILED\n", tempFreq);
		return status;
	}

	temp = getFrequency(modem.spid);
	printf("Freq Set To %f\n",temp);

	status = clearIRQFlags(modem.spid);
	if(status != 1)
	{
		printf("clearIRQFlags FAILED\n");
		return status;
	}

	status = modemReceive(&modem);
	if(status != 1)
	{
		printf("Receve initialization FAILED\n");
		return status;
	}

	return status;
}

int getSPID()
{
	return modem.spid;
}


