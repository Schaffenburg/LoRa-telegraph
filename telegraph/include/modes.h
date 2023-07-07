#include <Arduino.h>

#include "config.h"

// modes.h
#ifndef MODES_H
#define MODES_H

#define MODE_OFF 0
#define MODE_DIRECT 1
#define MODE_BLE 2
#define MODE_STRING 3

extern uint8_t activeMode;

extern const uint32_t standbyColors[];

void handleWord(char);

#endif // MODES_Hs