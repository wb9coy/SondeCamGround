#ifndef LOGGER_H
#define LOGGER_H
#include "queue.h"

int getLoggerQueueSize();
int loggerThreadSend(QelementData data);
void stopLoggerThread();
int startLoggerThread();


#endif
