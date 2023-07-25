// SERVER.h
#ifndef SERVER_H
#define SERVER_H

#include "Receiver.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <Update.h>

void connectToWiFi();
void setupWebServer();
void handleWebClient();
void handleTickerEvent();

extern WebServer server;
extern WebSocketsServer webSocket;

#endif // SERVER_H
