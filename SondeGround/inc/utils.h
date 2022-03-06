#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>


#define MAX_FLIGHT_NAME		30
#define MAX_LOG_PATH_NAME   200
#define MAX_COMMAND_SIZE    MAX_LOG_PATH_NAME + 50
#define MAX_IP_ADDRESS      50
#define MAX_PASSWORD        50


void createImageFilePathName();
char * getImageFilePathName();

void createImageSeqFilePathName();
char * getImageSeqFilePathName();

void createDataFilePathName();
char * getDataFilePathName();

int getStartingFileID( char logDir[]);
void setFlighName(char* flighNameParam);
void setBaseLogDir(char* BaseLogDirParam);
void setFlightNumber(int flightNumParam);
void createLogDirectories();
long int findSize(char file_name[]);

void setGWID(uint8_t gwIDParam);
uint8_t getGWID();

void setGatewayServerIPAddress(char * gatewayServerIPAddress);
char * getGatewayServerIPAddress();

void setGatewayServerPort(int gatewayServerPortParam);
int  getGatewayServerPort();

void setGatewayFTPPort(int gatewayFTPPortParam);
int  getGatewayFTPPort();

void setFTPPassword(char * ftpPasswordParam);
char *  getFTPPassword();

void setImageFileID(int imageFileIDParam);
int getImageFileID();

int convBW(const char *bwString);
int convSF(const char *sfString);
int convCodingRate(const char *codingRateString);


#endif
