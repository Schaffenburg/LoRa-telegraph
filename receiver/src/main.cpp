#include <SoftwareSerial.h>
#include "Server.h"
#include "TelegraphTime.h"
#include "StoreCSV.h"
#include "heltec.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
#define SPACE_TIMER_MS 1500

SoftwareSerial SoftSerial(36, 17, false);

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

  setupWifi();
  setupTime();
  setupWebServer();

  readCSVfromFS();

  delay(1000);

  SoftSerial.print("Setup complete!");

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

      if (interval > SPACE_TIMER_MS) {
        SoftSerial.print(' ');
        Heltec.display->drawString(0 , 0 , " ");
      }
      lastPacketTime = currentTime;
    }

    Heltec.display->display();
    SoftSerial.print(buf); // write string
  }
}

void loop() {
  handleWebClient();
  handleLoRa();
  handleEventsTimer();
}

