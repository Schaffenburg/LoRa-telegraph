#include <Arduino.h>

#include "config.h"

// modes.h
#ifndef MODES_H
#define MODES_H

#define MODE_OFF 0
#define MODE_DIRECT 1
#define MODE_BLE 2
#define MODE_STRING 3

uint8_t activeMode = DEFAULT_MODE;

const uint32_t standbyColors[] {
  0x000000, // MODE_OFF
  0xFFFF00, // MODE_DIRECT
  0x0000D0, // MODE_BLE
  0x00D010, // MODE_STRING
};

void handleWord(char);

#endif // MODES_Hs