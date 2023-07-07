#include <Arduino.h>
#include <WiFi.h>

#include "bluetooth.h"
#include "config.h"
#include "neopixel.h"


void go_to_sleep() {
    WiFi.disconnect();
    bleKeyboard.end();

    // blink blink
    fillPixel(0x0);
    fillPixel(0xFF0000);
    delay(200);
    fillPixel(0x0);
    fillPixel(0xFF0000);
    delay(200);
    fillPixel(0x0);

    // telegraph go bye bye:
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_IN, LOW);
    esp_deep_sleep_start();
}
