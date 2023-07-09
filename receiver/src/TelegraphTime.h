// Time.h
#ifndef TELEGRAPH_TIME_H
#define TELEGRAPH_TIME_H

#include <time.h>
#include "heltec.h"

extern const char* ntpServer;
extern const long  gmtOffset_sec;
extern const int   daylightOffset_sec;

void printLocalTime();

#endif