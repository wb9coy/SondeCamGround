#ifndef SENSOR_H
#define SENSOR_H

int processInternalTempPacket(unsigned char * sensorPacket, int len, int packetType);
int processExternalTempPacket(unsigned char * sensorPacket, int len, int packetType);
int processPressurePacket(unsigned char * sensorPacket, int len, int packetType);
int processHumidityPacket(unsigned char * sensorPacket, int len, int packetType);

#endif
