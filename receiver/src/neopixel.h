// neopixel.h
#ifndef NEOPIX_H
#define NEOPIX_H

#include <Adafruit_NeoPixel.h>
#define NEO_PIN 12
#define NUM_PIX 35

extern Adafruit_NeoPixel pixels;

void setupPixel();
void fillPixel(uint32_t);

void neoPixelOn();
void neoPixelOff();

#endif // NEOPIX_H

