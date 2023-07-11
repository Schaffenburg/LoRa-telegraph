// Time.h
#ifndef TELEGRAPH_TIME_H
#define TELEGRAPH_TIME_H

#include "Receiver.h"
#include <time.h>

extern const char* ntpServer;
extern const long  gmtOffset_sec;
extern const int   daylightOffset_sec;

void setupTime();
void printLocalTime();
void handleEventsTimer();

#endif