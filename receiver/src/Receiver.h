// Receiver.h
#ifndef RECEIVER_H
#define RECEIVER_H

#include <SoftwareSerial.h>
#include "TelegraphTime.h"
#include "StoreCSV.h"
#include "Server.h"
#include "StoreCSV.h"
#include "neopixel.h"
#include <WebServer.h>
#include "heltec.h"

#define BAND 868E6 // you can set band here directly,e.g. 868E6,915E6
#define SPACE_TIMER_MS 2500
#define RX_PIN 17
#define TX_PIN 23 // Connect this to Flipdot Controler Board's RXD

#define CLEAR_DISPLAY 0xC

extern SoftwareSerial SoftSerial;

#endif