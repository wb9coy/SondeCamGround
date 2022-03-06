/*
 ============================================================================
 Name        : HAB_Ground.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <webClientThread.h>
#include "utils.h"
#include "rxThread.h"
#include "modem.h"
#include "logger.h"
#include "statusThread.h"
#include "timers.h"
#include "iniparser.h"
#include "version.h"
#include "ecc.h"

int main(void)
{
	pthread_cond_t	EndMainCon=PTHREAD_COND_INITIALIZER;
	pthread_mutex_t EndMainMut=PTHREAD_MUTEX_INITIALIZER;

	QelementData versionQData;
	int result;

	dictionary *ini;
	char iniFileName[] = "../habGWconfig.ini";
    ini = iniparser_load(iniFileName);
    if(ini == NULL)
    {
    	printf("ERROR Count not open ini file %s\n",iniFileName);
    	exit(-1);
    }

	setFlighName((char *)iniparser_getstring(ini,"payload:flightname","Unknown"));
	setBaseLogDir("/home/pi/logs");
	setFlightNumber(iniparser_getint(ini,"payload:flightnum",1));
	setGWID(iniparser_getint(ini,"payload:gwID",0));
    setGatewayServerIPAddress((char *)iniparser_getstring(ini,"network:gwIPaddress","Unknown"));
    setGatewayServerPort(iniparser_getint(ini,"network:gwport",0));
    setGatewayFTPPort(iniparser_getint(ini,"network:gwFTPport",0));

    createLogDirectories();

	rscode_init();

    result = startLoggerThread();
    if(!result)
    {
    	printf("Fatal Error could start Logger thread");
    }

    result = startRxThread();
    if(!result)
    {
    	printf("Fatal Error could not start Rx thread");
    }

    result = modemSetup(ini);
    if(!result)
    {
    	printf("Fatal Error could not setup modem");
    }

    result = startStatusThread();
    if(!result)
    {
    	printf("Fatal Error could not start status thread");
    }

    result = setupTimers();
    if(!result)
    {
    	printf("Fatal Error could timers");
    }

    if(result)
    {
		versionQData.len = strlen(gwVersionInfo);
		versionQData.buf = calloc(versionQData.len,sizeof(unsigned char));
		memcpy(versionQData.buf,gwVersionInfo,versionQData.len);
		result = loggerThreadSend(versionQData);
		printf("%s\n",gwVersionInfo);
    }

    result = startwebClientThread();
    if(!result)
    {
    	printf("Fatal Error could not start web client  thread");
    }


	pthread_cond_wait(&EndMainCon, &EndMainMut);
	printf("Ending HAB_Payload\n");

	return EXIT_SUCCESS;
}
