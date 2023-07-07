// telegraph.h
#ifndef TELEGRAPH_H
#define TELEGRAPH_H

void handleTelegraph(int);

extern String strbuff;

extern uint8_t wordword;
extern uint8_t readinx;
extern boolean readingword;

extern int state; 
extern int lastchange;

void sendStr(String);

#endif // TELEGRAPH_H