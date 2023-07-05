  enum state {SHORT = 0, LONG, TIMEOUT};

void serialState(uint8_t state) {
  switch (state) {
    case SHORT:; Serial.print("short"); break;
    case LONG:; Serial.print("long"); break;
    case TIMEOUT:; Serial.print("timeout"); break;
    default:; Serial.print("none"); break;
  }
}

uint8_t resolveState(int diff) {
  return (diff < MIN_LONG) ? SHORT : LONG;
}

// short is 0; long is 1
char morse2char(uint8_t d, uint8_t len) {
  word w = d | (len << 8);

  switch (w) {
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

    // punctuation lol
    case (6 << 8 | 0b101010):; return '.'; break;
    case (6 << 8 | 0b110011):; return ','; break;
    case (6 << 8 | 0b001100):; return '?'; break;
    case (6 << 8 | 0b010101):; return ';'; break;
    case (6 << 8 | 0b000111):; return ':'; break;
    case (6 << 8 | 0b100001):; return '-'; break;
    case (5 << 8 | 0b01001):;  return '/'; break;
    case (6 << 8 | 0b011110):; return '\''; break;
    case (6 << 8 | 0b010010):; return '"'; break;

    // done
    case (5 << 8 | 0b01010):; return ACK; break;

    // "specials"
    case (5 << 8 | 0b01000):; return '\n'; break;

    case (8 << 8 | 0b00000000):; return BCK; break;
    case (8 << 8 | 0b10000000):; return CLR; break;

    // cycle modes
    case (8<<8 | 0xFF):; return 0x00; break;

    default:; return NAK;
  }
}
