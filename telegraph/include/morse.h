#include <Arduino.h>

// morse.h
#ifndef MORSE_H
#define MORSE_H

extern int lasttime;

char morse2char(uint8_t, uint8_t);
uint8_t resolveState(int);

#endif // MORSE_H