#include "neopixel.h"
#include "bluetooth.h"
#include "modes.h"
#include "telegraph.h"

void handleWord(char ch) {
  if (ch == 0x00) {
    activeMode += 1;

    if (activeMode > MODE_STRING) {
      activeMode = MODE_OFF;

      ledcAttachPin(PIN_BEEP, 0);
      ledcWrite(0, 2);
    }

    fillStandby();

    Serial.printf("new mode: %d\n", activeMode);
    return;
  }

  switch (activeMode) {
    case MODE_DIRECT:
      if (ch != NAK) sendStr(String(ch));
      break;

    case MODE_BLE:
      blePress(ch);
      break;

    case MODE_STRING:
      if (ch == ACK) {
        sendStr(strbuff);

        // reset buffer
        strbuff = "";

        flashPixels(0xFFFF00, 2);
      } else {
        // add to buffer
        strbuff.concat(ch);
      }
      break;

    default:
      activeMode = DEFAULT_MODE;
      break;
  }

  flashPixelFeedback(ch);
}
