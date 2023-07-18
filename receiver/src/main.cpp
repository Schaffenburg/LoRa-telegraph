#include "Receiver.h"

SoftwareSerial SoftSerial(RX_PIN, TX_PIN, false);
bool enableTicker;
int currentCharPos;

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/,
  true /*Heltec.LoRa Disable*/,
  false /*Serial Enable*/,
  true /*PABOOST Enable*/,
  BAND /*long BAND*/);
  Heltec.display->init();

  Serial.begin(9600);
  Serial.print("Serial ready.");
  SoftSerial.begin(9600);

  setupPixel();
  setupWifi();
  setupTime();
  setupWebServer();

  // readCSVfromFS();

  fillPixel(0xFF0000);
  delay(5000);

  enableTicker = false;
  currentCharPos = 0;

  SoftSerial.print("Fertig");
  fillPixel(0x00FF00);

  Heltec.display->clear();
  Heltec.display->drawString(0 , 0 , "LoRaTelegraphRcv ready");
  Heltec.display->display();
}

void handleLoRa() {
  static unsigned long lastPacketTime = 0; // remembers when the last packet was received
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String buf;
    unsigned long currentTime = millis();
    unsigned long interval = currentTime - lastPacketTime;

    // read packet
    while (LoRa.available()) {
      buf.concat(String((char)LoRa.read()));
    }

    Heltec.display->clear();
    Heltec.display->drawString(0 , 0 , buf);

    Serial.printf("^-. rec '%s' time passed %lu\n", buf.c_str(), interval);

    // if the packet is a single character, auto-insert space after interval
    if (buf.length() == 1) {
      if (currentCharPos == 2*LINE_LENGTH) {
        SoftSerial.write(CLEAR_DISPLAY);
        currentCharPos = 0;
      } else if (currentCharPos == LINE_LENGTH) {
        SoftSerial.write(0x09);
        SoftSerial.write(0x0A);
      } else if (interval > SPACE_TIMER_MS) {
        SoftSerial.print(' ');
        currentCharPos++;
        Heltec.display->drawString(0 , 0 , " ");
      }
      lastPacketTime = currentTime;
    }
    Heltec.display->display();
    SoftSerial.print(buf); // write string
    currentCharPos += buf.length();
  }
}

void loop() {
  handleWebClient();
  handleLoRa();
  // handleEventsTimer();
  handleTickerEvent();
}
