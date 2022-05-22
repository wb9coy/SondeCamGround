#ifndef RX_H
#define RX_H

#include "packetDefs.h"

#define MAX_RX_DATA_BUF 256

struct rxThreadDataType
{
	uint8_t buf[MAX_RX_DATA_BUF];
	uint8_t size;
	float rssi;
};

int rxThreadDispatchPacket(unsigned char *rxThreadDispatchPacketBuf,int len);
int startRxThread();
int rxQueuePacket(struct rxThreadDataType rxThreadData);

#endif
