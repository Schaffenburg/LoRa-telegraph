// telegraph.h
#ifndef TELEGRAPH_H
#define TELEGRAPH_H

void handleTelegraph(int);

String strbuff = "";

uint8_t wordword = 0;
uint8_t readinx = 0;
boolean readingword = false;

int state = HIGH; // prevent T on startup
int lastchange = 0;

void sendStr(String);

#endif // TELEGRAPH_H