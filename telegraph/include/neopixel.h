#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "config.h"

// neopixel.h
#ifndef PIXEL_H
#define PIXEL_H

void fillPixel(uint32_t);
void flashPixels(uint32_t, uint8_t);
void fillStandby();

void flashPixelFeedback(char);

uint32_t randomColor();

void handleNeopixel(int);

extern Adafruit_NeoPixel pixels;

#endif // PIXEL_H