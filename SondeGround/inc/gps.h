#ifndef GPS_H
#define GPS_H


int processGPSPacket(unsigned char * GPSPacket, int len, int packetType);
int validateNMEAChecksum(char *buf, int len);
int getLastGGA(uint8_t buf[]);
int getLastRMC(uint8_t buf[]);

#endif
