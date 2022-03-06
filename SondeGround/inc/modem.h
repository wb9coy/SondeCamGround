#ifndef MODEM_H
#define MODEM_H
#include "iniparser.h"
#include "SX1278FSK.h"

int modemSetup(dictionary *ini);
int modemSend(uint8_t *rxBuf, int len);
int getSPID();

#endif
