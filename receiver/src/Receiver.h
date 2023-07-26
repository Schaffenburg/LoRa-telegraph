// Receiver.h
#ifndef RECEIVER_H
#define RECEIVER_H

#include <SoftwareSerial.h>
#include <Preferences.h>
#include "TelegraphTime.h"
#include "StoreCSV.h"
#include "Server.h"
#include "StoreCSV.h"
#include "neopixel.h"
#include <WebServer.h>
#include "heltec.h"

#define BAND 868E6 // you can set band here directly,e.g. 868E6,915E6
#define SPACE_TIMER_MS 4000
#define RX_PIN 17
#define TX_PIN 23 // Connect this to Flipdot Controler Board's RXD

#define CLEAR_DISPLAY 0xC

#define LINE_LENGTH 18

extern bool enableTicker;
extern bool enableFallbackAP;
extern int currentCharPos;

extern Preferences preferences;
extern SoftwareSerial SoftSerial;

#endif