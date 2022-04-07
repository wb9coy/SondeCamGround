#ifndef PACKET_DEFS_H
#define PACKET_DEFS_H
#include <stdint.h>
#include "ecc.h"


#define MAX_BUF_LEN		   58
#define MAX_GPS_BUF_LEN    61
#define MAX_GPS_HDR_LEN    3
#define MAX_IMG_BUF_LEN    56
#define MAX_CALL_SIGN_SIZE 12
#define MAX_INFO_DATA_SIZE 32

#define MTU_SIZE          64
#define MAX_GPS_NMEA_SIZE 82

//Packet Type
#define	START_IMAGE		0xff
#define	IMAGE_DATA		0xee
#define	END_IMAGE		0xdd
#define	GPS_GGA			0xc1
#define	GPS_GGA_1		0xc2
#define	GPS_GGA_2		0xc3
#define	GPS_RMC			0xb1
#define	GPS_RMC_1		0xb2
#define	GPS_RMC_2		0xb3
#define	INT_TEMP		0xaa
#define	EXT_TEMP		0x99
#define BATT_INFO		0x88
#define PRESS_INFO		0x77
#define HUM_INFO        0x66
#define INFO_DATA		0x55
#define CW_ID			0x44

#define	START_SEQ_IMAGE	0x10
#define	IMAGE_SEQ_DATA  0x11
#define	END_SEQ_IMAGE	0x12
#define RSSI_INFO		0x13
#define PING			0x14

struct __attribute__((__packed__)) HABPacketImageStartType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint32_t  fileSize;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageDataType
{
	uint16_t   packetType;
	uint8_t    imageFileID;
	uint16_t   imageSeqnum;
	uint8_t    imageDataLen;
	uint8_t    imageData[MAX_IMG_BUF_LEN];
	uint16_t   crc16;
	//uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageEndType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint16_t  crc16;
	uint8_t   codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketImageSeqStartType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint8_t   gwID;
	uint32_t  fileSize;
};

struct __attribute__((__packed__)) HABPacketImageSeqDataType
{
	uint16_t   packetType;
	uint8_t    imageFileID;
	uint16_t   imageSeqnum;
	uint8_t    imageDataLen;
	uint8_t    gwID;
	uint8_t    imageData[MAX_IMG_BUF_LEN];
};

struct __attribute__((__packed__)) HABPacketImageSeqEndType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint8_t   gwID;
};

struct __attribute__((__packed__)) GWPacketImageSeqEndType
{
	uint16_t  packetType;
	uint8_t   imageFileID;
	uint8_t   gwID;
	uint8_t   GGASentence[MAX_GPS_NMEA_SIZE];
	uint8_t   RMCSentence[MAX_GPS_NMEA_SIZE];
};

struct __attribute__((__packed__)) HABPacketGPSDataType
{
	uint16_t   packetType;
	uint8_t    gpsDataLen;
	uint8_t    gpsData[256];
};


struct __attribute__((__packed__)) HABPacketInfoDataType
{
	uint16_t   packetType;
	uint8_t    infoDataLen;
	uint8_t    infoData[MAX_INFO_DATA_SIZE];
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketFTPEndType
{
	uint16_t  packetType;
	uint16_t  imageFileID;
};

struct __attribute__((__packed__)) HABPacketCallSignDataType
{
	uint16_t   packetType;
	uint8_t    callSignDataLen;
	uint8_t    callSignData[MAX_CALL_SIGN_SIZE];
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketBattInfoDataType
{
	uint16_t   packetType;
	float      battInfoData;
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketIntTempInfoDataType
{
	uint16_t   packetType;
	int8_t     intTempInfoData;
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketExtTempInfoDataType
{
	uint16_t   packetType;
	float      extTempInfoData;
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketPressureInfoDataType
{
	uint16_t   packetType;
	float      pressureInfoData;
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

struct __attribute__((__packed__)) HABPacketHumidityInfoDataType
{
	uint16_t   packetType;
	float      humidityInfoData;
	uint16_t   crc16;
	uint8_t    codeword[NPAR];
};

#endif
