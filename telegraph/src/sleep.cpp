#include <Arduino.h>
#include <WiFi.h>

#include "bluetooth.h"
#include "config.h"
#include "neopixel.h"

void go_to_sleep()
{
    Serial.println("Starting to go to sleep");

    WiFi.disconnect();
    bleKeyboard.end();

    // blink blink
    fillPixel(0x0);
    for (uint8_t i = 0; i < 254; i++)
    {
        fillPixel((254 - (uint32_t)i) << 16);
        delay(10);
    }

    fillPixel(0);
    delay(5);

    // telegraph go bye bye:
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_IN, LOW);

    Serial.println("bye bye- sayonara!");
    esp_deep_sleep_start();
}
