#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "utils.h"

static char baseLogDir[MAX_LOG_PATH_NAME];
static char imageFileLogDir[MAX_LOG_PATH_NAME];
static char imageFilePathName[MAX_LOG_PATH_NAME];
static char imageFileLogDir[MAX_LOG_PATH_NAME];
static char dataFileLogDir[MAX_LOG_PATH_NAME];
static char dataFilePathName[MAX_LOG_PATH_NAME];
static char imageSeqLogDir[MAX_LOG_PATH_NAME];
static char imageSeqFilePathName[MAX_LOG_PATH_NAME];
static char flighName[MAX_FLIGHT_NAME];
static char gatewayServerIPAddress[MAX_IP_ADDRESS];
static int gatewayServerPort;
static int gatewayFTPPort;
static int flightNum;
static int imageFileID;
static  uint8_t gwID;


void setFlighName(char* flighNameParam)
{
	strcpy(flighName,flighNameParam);
}

void setBaseLogDir(char* baseLogDirParam)
{
	strcpy(baseLogDir,baseLogDirParam);
}

void setFlightNumber(int flightNumParam)
{
	flightNum = flightNumParam;
}

void setGWID(uint8_t gwIDParam)
{
	gwID = gwIDParam;
}

uint8_t getGWID()
{
	return gwID;
}

void setImageFileID(int imageFileIDParam)
{
	imageFileID = imageFileIDParam;
}

int getImageFileID()
{
	return imageFileID;
}


void createLogDirectories()
{
	int result;
	char tempDirName[MAX_LOG_PATH_NAME];

	result = mkdir(baseLogDir, 0777);
    if (!result)
    {
        printf("Directory %s created\n",baseLogDir);
    }

    sprintf (tempDirName, "%s/%s%d", baseLogDir,flighName, flightNum);
	result = mkdir(tempDirName, 0777);
    if (!result)
    {
        printf("Directory %s created\n",tempDirName);
    }

    sprintf (imageFileLogDir, "%s/%s%d/images", baseLogDir,flighName, flightNum);
	result = mkdir(imageFileLogDir, 0777);
    if (!result)
    {
        printf("Directory %s created\n",imageFileLogDir);
    }

    sprintf (imageSeqLogDir, "%s/%s%d/imageSeq", baseLogDir,flighName, flightNum);
	result = mkdir(imageSeqLogDir, 0777);
    if (!result)
    {
        printf("Directory %s created\n",imageSeqLogDir);
    }

    sprintf (dataFileLogDir, "%s/%s%d/data", baseLogDir,flighName, flightNum);
	result = mkdir(dataFileLogDir, 0777);
    if (!result)
    {
        printf("Directory %s created\n",dataFileLogDir);
    }

	printf("Images are located in %s\n",imageFileLogDir);
	printf("Data logs are located in %s\n",dataFileLogDir);

}

void createImageFilePathName()
{

	  sprintf (imageFilePathName, "%s/image%d.jpg", imageFileLogDir,imageFileID);
}

char * getImageFilePathName()
{
	return imageFilePathName;
}

void createImageSeqFilePathName()
{

	  sprintf (imageSeqFilePathName, "%s/image%dGW_%d.seq", imageSeqLogDir,imageFileID,gwID);
}

char * getImageSeqFilePathName()
{
	return imageSeqFilePathName;

}

void createDataFilePathName()
{

	  sprintf (dataFilePathName, "%s/flightdata.txt", dataFileLogDir);
}

char * getDataFilePathName()
{
	return dataFilePathName;

}

int getStartingFileID( char logDir[])
{
	int count = -2;
	  DIR *d;
	  struct dirent *dir;
	  d = opendir(logDir);
	  if (d) {
		while ((dir = readdir(d)) != NULL) {
		  printf("%s\n", dir->d_name);
		  count++;
		}
		closedir(d);
	  }
	  count++;
	  return(count);
}

long int findSize(char file_name[])
{
    // opening the file in read mode
    FILE* fp = fopen(file_name, "r");

    // checking if the file exist or not
    if (fp == NULL) {
        printf("File Not Found  %s\n",file_name);
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    long int res = ftell(fp);

    // closing the file
    fclose(fp);

    return res;
}

void setGatewayServerIPAddress(char * gatewayServerIPAddressParam)
{
	strcpy(gatewayServerIPAddress,gatewayServerIPAddressParam);
}

char * getGatewayServerIPAddress()
{
	return gatewayServerIPAddress;
}

void setGatewayServerPort(int gatewayServerPortParam)
{
	gatewayServerPort = gatewayServerPortParam;
}

int getGatewayServerPort()
{
	return gatewayServerPort;
}

void setGatewayFTPPort(int gatewayFTPPortParam)
{
	gatewayFTPPort = gatewayFTPPortParam;
}

int getGatewayFTPPort()
{
	return gatewayFTPPort;
}


