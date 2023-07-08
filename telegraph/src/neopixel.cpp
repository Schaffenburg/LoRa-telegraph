#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "modes.h"
#include "neopixel.h"

int ledNextFlash = 0;

uint8_t ledState = 0; // off
uint8_t ledFlashes = 0;
uint32_t ledColor = 0;

Adafruit_NeoPixel pixels(NUMPIX, NEOPIN, NEO_GRB | NEO_KHZ800);

void handleNeopixel(int now)
{
  if ((ledFlashes > 0) && (now > ledNextFlash))
  {
    if (ledState)
    {
      fillStandby();
      ledFlashes--;
    }
    else
    {
      fillPixel(ledColor);
    }

    ledNextFlash = millis() + 100;

    ledState = !ledState;

    Serial.printf("Still flashing %d times in %x next at %d\n",
                  ledFlashes, ledColor, ledNextFlash);
  }
}

void flashPixelFeedback(char ch)
{
  switch (ch)
  {
  case NAK:
    flashPixels(0xFF0000, NUM_FLASH_ERR);
    break;

  case ACK:
    flashPixels(0xFFFF00, NUM_FLASH_ACC);
    break;

  case BCK:
    flashPixels(0x0000FF, NUM_FLASH_ACC);
    break;

  default:
    flashPixels(0x00FF00, NUM_FLASH_ACC);
    break;
  }
}

uint32_t randomColor()
{
  return pixels.ColorHSV((uint16_t)esp_random(), 255, 255);
}

void flashPixels(uint32_t c, uint8_t times)
{
  Serial.printf("Flashing %d times with color %x\n", times, c);

  ledColor = c;
  ledState = 0;
  ledFlashes = times;
  ledNextFlash = 0; // ASAP

  return;
}

void fillStandby()
{
  fillPixel(standbyColors[activeMode]);
}

void fillPixel(uint32_t c)
{
  for (uint8_t i = 0; i < NUMPIX; i++)
  {
    pixels.setPixelColor(i, c);
  }

  pixels.show();
}
