// bluetooth.h
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BleKeyboard.h>

void blePress(char ch);
void setupBLE();

extern BleKeyboard bleKeyboard;

#endif // BLUETOOTH_H