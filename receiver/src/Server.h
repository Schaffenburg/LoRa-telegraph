// WebServer.h
#ifndef SERVER_H
#define SERVER_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <SPI.h>
#include <Update.h>
#include "heltec.h"

void setupWebServer();
void handleWebClient();

#endif // SERVER_H
