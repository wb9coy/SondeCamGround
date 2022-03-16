#ifndef WEB_CLIENT_H
#define WEB_CLIENT_H
#include "utils.h"
#include "gps.h"

#define HAB_RX_BUFFER_BYTES 10
#define MAX_WEB_BUF_LEN		256

struct __attribute__((__packed__)) webHABPacketDataType
{
	uint16_t   packetType;
	uint8_t    webDataLen;
	uint8_t    webData[MAX_WEB_BUF_LEN];
};

int connectToServer();
void setGatewayServerIPAddress(char * webClientIPAddressParam);
void setGatewayServerPort(int webClientPortParam);

int startwebClientThread();
int sendToGatewayServer(struct webHABPacketDataType webHABPacketData);

#endif
