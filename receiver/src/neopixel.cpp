#include "neopixel.h"

Adafruit_NeoPixel pixels(NUM_PIX, NEO_PIN, NEO_GRB | NEO_KHZ800);

void setupPixel() {
    pixels.begin();

    pixels.setBrightness(255/2);
};

void fillPixel(uint32_t c)
{
  for (uint8_t i = 0; i < NUM_PIX; i++)
  {
    pixels.setPixelColor(i, c);
  }

  pixels.show();
}

void neoPixelOn() {
    fillPixel(0xFFFFFF);
}

void neoPixelOff() {
    fillPixel(0x0);
}
