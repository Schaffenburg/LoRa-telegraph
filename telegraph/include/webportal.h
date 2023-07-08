// webportal.h
#ifndef WEBPORTAL_H
#define WEBPORTAL_H

#include <WebServer.h>

void wlanConnected(int);
void setupWeb();

extern WebServer server;

void addMsg(String);

#define MAXWEBMSG 200

extern String web_msg;

#endif // WEBPORTAL_H