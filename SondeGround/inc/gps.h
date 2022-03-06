#ifndef GPS_H
#define GPS_H

#define MAX_GPS_NMEA_SIZE 82

int processGPSPacket(unsigned char * GPSPacket, int len, int packetType);
int validateNMEAChecksum(char *buf, int len);

#endif
