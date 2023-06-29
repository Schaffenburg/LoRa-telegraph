void setTone(uint8_t state) {
  if (state) {
    ledcDetachPin(PIN_BEEP);
  } else {
    ledcAttachPin(PIN_BEEP, 0);
    ledcWrite(0, 2);
  }
}

uint32_t setupTone() {
  ledcAttachPin(PIN_BEEP, 0);
  return ledcSetup(0, 1000, 2);
}
