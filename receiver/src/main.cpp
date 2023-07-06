#include <SoftwareSerial.h>
#include "Server.h"
#include "heltec.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

SoftwareSerial SoftSerial(36, 17, false);

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/,
  true /*Heltec.LoRa Disable*/,
  false /*Serial Enable*/,
  true /*PABOOST Enable*/,
  BAND /*long BAND*/);
  Heltec.display->init();

  Serial.begin(9600);
  SoftSerial.begin(9600);

  setupWebServer();

  delay(1000);
  Serial.print("hi!");
  SoftSerial.print("ready!");

  Heltec.display->clear();
  Heltec.display->drawString(0 , 0 , "LoRaTelegraph receiver ready");
  Heltec.display->display();
}

void loop() {
  handleWebClient();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String buf;

    // read packet
    while (LoRa.available()) {
      buf.concat(String((char)LoRa.read()));

      Heltec.display->clear();
      Heltec.display->drawString(0 , 0 , buf);
      Heltec.display->display();
    }

    Serial.printf("^-. rec '%s'\n", buf);
    SoftSerial.print(buf); // write string
  }
}