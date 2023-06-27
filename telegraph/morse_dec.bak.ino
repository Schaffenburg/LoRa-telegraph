/*
Ideas:
 - I2C OLED fuer preview
 - mehr sonderzeichen (auch backspace)
 - mehr protos (vlt auch BL tastatur)
 - MEHR leds, da besser
 - USB breakout zeug
*/

#include "heltec.h"
#include <Adafruit_NeoPixel.h>

// LoRa
#define LoRaBAND    868E6

// Telegraph
#define PIN_IN 13

#define MIN_TIME 30

#define MIN_LONG 190
#define MIN_TIMEOUT 500

// - * NEO * -
#define NEOPIN 12
#define NUMPIX 2

#define NUM_FLASH_ERR 6

Adafruit_NeoPixel pixels(NUMPIX, NEOPIN, NEO_GRB | NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_IN, INPUT_PULLUP);

  Serial.begin(115200);

  // LoRa
  Heltec.begin(
    false /*DisplayEnable Enable*/,
    true /*Heltec.LoRa Disable*/,
    true /*Serial Enable*/,
    true /*PABOOST Enable*/,
    LoRaBAND /*long BAND*/);
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  
  pixels.begin();

  delay(1000);
  Serial.print("hi!\n");
  flashPixels(0x00FF00, 3);
}

int state = HIGH; // prevent T on startup
int lastchange;

int lasttime;

uint8_t wordword = 0;
uint8_t readinx = 0;
boolean readingword = false;

int strbuffinx = 0;
String strbuff = "";

void loop() {  
  // put your main code here, to run repeatedly:
  int newstate = digitalRead(PIN_IN);
  int now = millis();

  // debouce
  if(now - lastchange < MIN_TIME) {
    return;
  }

  int diff = now-lastchange;

  if(newstate != state) {
    if(newstate == 1) {
      state = resolveState(diff);

      // write word
      readingword = true;
      wordword |= state << readinx;
      readinx++;
    }

    state = newstate;
    lastchange = now;
  } else {
    if(diff > MIN_TIMEOUT && readingword) {
      char ch = morse2char(wordword, readinx);
      
      Serial.print("word done: 0b");
      printWord(wordword, readinx);
      Serial.printf(" len(%d) -> %c;\n", readinx, ch);

      // send if 01010 (=\r)
      if(ch == '\r') {
        Serial.printf("sending: %s\n", strbuff);
        sendStr(strbuff);
        
        strbuff = "";
        strbuffinx = 0; 

        flashPixels(0xFFFF00, 2);
      } else {
        // add to strbuff
        strbuff.concat(ch);

        switch(ch) {
          case '?':
            flashPixels(0xFF0000, NUM_FLASH_ERR);
            break;

          default:
            flashPixels(randomColor(), 1);
        }
      }
      
      // reset
      readingword = false;
      wordword = 0b0;
      readinx = 0;
    }
  }

  lasttime = now;

  delay(15);
}

enum state{SHORT = 0, LONG, TIMEOUT};

void serialState(uint8_t state) {
  switch(state) {
    case SHORT:; Serial.print("short"); break;
    case LONG:; Serial.print("long"); break;
    case TIMEOUT:; Serial.print("timeout"); break;
    default:; Serial.print("none"); break;
  }
}

uint8_t resolveState(int diff) {
  return (diff < MIN_LONG) ? SHORT : LONG;
}

void printWord(uint8_t w, uint8_t len) {
  for(uint8_t i = 0 ; i < len; i++) {
    if(w & 1 << i) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
  }
}

// short is 0; long is 1
char morse2char(uint8_t d, uint8_t len) {
  word w = d | (len << 8);
  
  switch(w) {
    case (2 << 8 | 0b10):;   return 'A'; break;
    case (4 << 8 | 0b0001):; return 'B'; break;
    case (4 << 8 | 0b00101):;return 'C'; break;
    case (3 << 8 | 0b001):;  return 'D'; break;
    case (1 << 8 | 0b0):;    return 'E'; break;
    case (4 << 8 | 0b0100):; return 'F'; break;
    case (3 << 8 | 0b011):;  return 'G'; break;
    case (4 << 8 | 0b0000):; return 'H'; break;
    case (2 << 8 | 0b00):;   return 'I'; break;
    case (4 << 8 | 0b1110):; return 'J'; break;
    case (3 << 8 | 0b101):;  return 'K'; break;
    case (4 << 8 | 0b0010):; return 'L'; break;
    case (2 << 8 | 0b11):;   return 'M'; break;
    case (2 << 8 | 0b01):;   return 'N'; break;
    case (3 << 8 | 0b111):;  return 'O'; break;
    case (4 << 8 | 0b0110):; return 'P'; break;
    case (4 << 8 | 0b1011):; return 'Q'; break;
    case (3 << 8 | 0b010):;  return 'R'; break;
    case (3 << 8 | 0b000):;  return 'S'; break;
    case (1 << 8 | 0b1):;    return 'T'; break;
    case (3 << 8 | 0b100):;  return 'U'; break;
    case (4 << 8 | 0b1000):; return 'V'; break;
    case (3 << 8 | 0b110):;  return 'W'; break;
    case (4 << 8 | 0b1001):; return 'X'; break;
    case (4 << 8 | 0b1101):; return 'Y'; break;
    case (4 << 8 | 0b0011):; return 'Z'; break;
    
    case (5 << 8 | 0b11110):; return '1'; break;
    case (5 << 8 | 0b11100):; return '2'; break;
    case (5 << 8 | 0b11000):; return '3'; break;
    case (5 << 8 | 0b10000):; return '4'; break;
    case (5 << 8 | 0b00000):; return '5'; break;
    case (5 << 8 | 0b00001):; return '6'; break;
    case (5 << 8 | 0b00011):; return '7'; break;
    case (5 << 8 | 0b00111):; return '8'; break;
    case (5 << 8 | 0b01111):; return '9'; break;
    case (5 << 8 | 0b11111):; return '0'; break;

    // ÄÖÜ -> boese!!11!
    //case (4 << 8 | 0b0101):;  return 'Ä'; break;
    //case (4 << 8 | 0b1011):;  return 'Ö'; break;
    //case (4 << 8 | 0b1100):;  return 'Ü'; break;
  
    // done
    case (5 << 8 | 0b01010):; return '\r'; break;

    // "specials"
    case (5 << 8 | 0b01000):; return '\n'; break;

    default:; return '?';
  }
}

void sendChar(char c) {
  LoRa.beginPacket();
  LoRa.print("char:");
  LoRa.print(c);
  LoRa.endPacket();
}

void sendStr(String str) {
  LoRa.beginPacket();
  LoRa.print("");
  LoRa.print(str);
  LoRa.endPacket();
}

uint32_t randomColor() {
  return pixels.ColorHSV((uint16_t)esp_random(), 255, 255);
}

void flashPixels(uint32_t c, uint8_t times) {
  for(uint8_t i = 0; i < times; i++) {
    fillPixel(c);
    delay(100);
    fillPixel(0x0);
    delay(100);  
  }
}

void fillPixel(uint32_t c) {
  for(uint8_t i = 0; i < NUMPIX; i++) {
    pixels.setPixelColor(i, c);
  }

  pixels.show();
}
