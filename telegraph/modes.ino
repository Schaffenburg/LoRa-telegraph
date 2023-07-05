uint8_t activeMode = DEFAULT_MODE;

#define MODE_OFF 0
#define MODE_DIRECT 1
#define MODE_BLE 2
#define MODE_STRING 3

const uint32_t standbyColors[] {
  0x000000, // MODE_OFF
  0xFFFF00, // MODE_DIRECT
  0x0000D0, // MODE_BLE
  0x00D010, // MODE_STRING
};

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
