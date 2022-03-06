#ifndef PACKET_DEFS_H
#define PACKET_DEFS_H
#include <stdint.h>
#include "ecc.h"


#define MAX_BUF_LEN		   58
#define MAX_GPS_BUF_LEN    62
#define MAX_IMG_BUF_LEN    56
#define MAX_CALL_SIGN_SIZE 12
#define MAX_INFO_DATA_SIZE 32

#define MTU_SIZE        64

//Packet Type
#define	START_IMAGE		0x01
#define	IMAGE_DATA		0x02
#define	END_IMAGE		0x03
#define	START_SEQ_IMAGE	0x04
#define	IMAGE_SEQ_DATA  0x05
#define	END_SEQ_IMAGE	0x06
#define	GPS_GGA_1		0x10
#define	GPS_GGA_2		0x11
#define	GPS_RMC_1		0x12
#define	GPS_RMC_2		0x13
#define	INT_TEMP		0x20
#define	EXT_TEMP		0x25
#define	HUM				0x30
#define	PRES			0x40
#define INFO_DATA		0x60
#define CW_ID			0x70
#define BATT_INFO		0x80
#define RSSI_INFO		0x90

struct __attribute__((__packed__)) HABPacketImageStartType
{
	uint8_t  packetType;
	uint8_t  imageFileID;
	uint32_t fileSize;
	uint16_t crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageDataType
{
	uint8_t   packetType;
	uint8_t   imageFileID;
	uint16_t  imageSeqnum;
	uint8_t   imageDataLen;
	uint8_t   imageData[MAX_IMG_BUF_LEN];
	uint16_t  crc16;
	//uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageEndType
{
	uint8_t  packetType;
	uint8_t  imageFileID;
	uint16_t crc16;
	uint8_t  codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageSeqStartType
{
	uint8_t  packetType;
	uint8_t  imageFileID;
	uint8_t  gwID;
	uint32_t fileSize;
};

struct __attribute__((__packed__)) HABPacketImageSeqDataType
{
	uint8_t   packetType;
	uint8_t   imageFileID;
	uint16_t  imageSeqnum;
	uint8_t   imageDataLen;
	uint8_t   gwID;
	uint8_t   imageData[MAX_IMG_BUF_LEN];
};

struct __attribute__((__packed__)) HABPacketImageSeqEndType
{
	uint8_t  packetType;
	uint8_t  imageFileID;
	uint8_t  gwID;
};

struct __attribute__((__packed__)) HABPacketGPSDataType
{
	uint8_t   packetType;
	uint8_t   gpsDataLen;
	uint8_t   gpsData[MAX_GPS_BUF_LEN];
};


struct __attribute__((__packed__)) HABPacketInfoDataType
{
	uint8_t   packetType;
	uint8_t   infoDataLen;
	uint8_t   infoData[MAX_INFO_DATA_SIZE];
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketFTPEndType
{
	uint8_t  packetType;
	uint16_t imageFileID;
};

struct __attribute__((__packed__)) HABPacketCallSignDataType
{
	uint8_t   packetType;
	uint8_t   callSignDataLen;
	uint8_t   callSignData[MAX_CALL_SIGN_SIZE];
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketBattInfoDataType
{
	uint8_t   packetType;
	float     battInfoData;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketIntTempInfoDataType
{
	uint8_t   packetType;
	int8_t    intTempInfoData;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};


#endif
