// config.h
#ifndef CONFIG_H
#define CONFIG_H

// LoRa
#define LoRaBAND 868E6

// Telegraph (only use RTC connected pins to enable deep-sleep)
#define PIN_IN 13

#define MIN_TIME 30

#define MIN_LONG 150
#define MIN_TIMEOUT 400

// - * NEO * -
#define NEOPIN 12
#define NUMPIX 2

#define NUM_FLASH_ERR 6
#define NUM_FLASH_ACC 2

// clearscreen
#define CLR (char)'\r' // 12 0xD
// backspace
#define BCK (char)0x7F
#define ACK (char)0x06
#define NAK (char)0x15 // =25 // err value

// beep beep
#define PIN_BEEP 4
#define BEEP_TONE (note_t)1000

// modes
#define DEFAULT_MODE 0

#endif // CONFIG_H
