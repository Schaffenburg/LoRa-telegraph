#include <BleKeyboard.h>
// get from https://github.com/T-vK/ESP32-BLE-Keyboard

// name, manufacturer, init. batter level
BleKeyboard bleKeyboard("LoRa-Telegraph", "schaffenburg", 69);

void setupBLE() {
  bleKeyboard.end();
  bleKeyboard.begin();
}

void blePress(char key) {
  bleKeyboard.press(key);

  bleKeyboard.releaseAll();
}
