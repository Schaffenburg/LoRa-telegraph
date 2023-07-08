/*
   WICHTIG
   OLED nicht AKTIVEREN, beeper haengt an pin 4, welcher auch an den OLED geht

  Ideas:
  - I2C OLED fuer preview
  - mehr sonderzeichen (auch backspace)
  - mehr protos (vlt auch BL tastatur)
  - MEHR leds, da besser
  - beep beep

  :) - USB breakout zeug
*/

#include <heltec.h>

#include "telegraph.h"
#include "config.h"
#include "bluetooth.h"
#include "OTAUpdate.h"
#include "tone.h"
#include "neopixel.h"
#include "modes.h"
#include "webportal.h"
#include "morse.h"

String strbuff = "";

uint8_t wordword = 0;
uint8_t readinx = 0;
boolean readingword = false;

int state = HIGH; // prevent T on startup
int lastchange = 0;

void pushWord(uint8_t state) {
  readingword = true;
  wordword |= state << readinx;
  readinx++;
}

void printWord(uint8_t w, uint8_t len) {
  for (uint8_t i = 0 ; i < len; i++) {
    if (w & 1 << i) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
  }
}

void handleTelegraph(int now) {
  int newstate = digitalRead(PIN_IN);
  int diff = now - lastchange;

  // debouce
  if (diff < MIN_TIME) {
    return;
  }

  if (newstate != state) {
    if (newstate == 1) {
      state = resolveState(diff);

      pushWord(state);
    }

    state = newstate;
    lastchange = now;
  } else {
    // timeout waiting for more of word
    if (diff > MIN_TIMEOUT && readingword) {
      char ch = morse2char(wordword, readinx);

      Serial.print("word done: 0b");
      printWord(wordword, readinx);
      Serial.printf(" len(%d) -> %c;\n", readinx, ch);

      handleWord(ch); // -> modes.ino

      // reset
      readingword = false;
      wordword = 0b0;
      readinx = 0;
    }
  }
}


void setup() {
  // telegraph
  pinMode(PIN_IN, INPUT_PULLUP);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_IN, LOW);

  Serial.begin(115200);

  // LoRa and stuff
  Heltec.begin(
    false /*DisplayEnable Enable*/,
    true /*Heltec.LoRa Disable*/,
    true /*Serial Enable*/,
    true /*PABOOST Enable*/,
    LoRaBAND /*long BAND*/);
  LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);

  // * n e o *
  pixels.begin();

  setupBLE();

  // beep boop
  pinMode(PIN_BEEP, OUTPUT);
  uint32_t freq = setupTone();

  setupWeb();

  setupOTA();

  delay(1000);
  Serial.print("hi!\n");
  Serial.printf("BEEP_TONE is %d; LEDC: %sok\n", BEEP_TONE, BEEP_TONE == freq ? "" : "not ");

  //TODO: print configuration

  flashPixels(0x00FF00, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
  int now = millis();

  setTone(digitalRead(PIN_IN));

  handleTelegraph(now);

  wlanConnected(now);

  server.handleClient();
  handleNeopixel(now);

  lasttime = now;
}

void sendStr(String str) {
  Serial.printf("sending '%s' .-^\n", str);

  LoRa.beginPacket();
  LoRa.print(str);
  LoRa.endPacket();

  // add to webportal
  addMsg(str);
}
