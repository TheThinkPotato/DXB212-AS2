#include "ReadDebounce.h"

ReadDebounce::ReadDebounce(uint8_t pin) {
  _pin = pin;
  _state = 0;
}

uint8_t ReadDebounce::getState() {
  uint8_t inputPattern = digitalRead(_pin);
  // debounce pattern (rotate bit to the left and place in new read at LSB)
  _state = (_state << 1) | inputPattern;
  return _state;
}

void ReadDebounce::updateState() {
  ReadDebounce::getState();
}

bool ReadDebounce::isRisingEdge() {
  return _state == 0b00111111;
}

bool ReadDebounce::isFallingEdge() {
  return _state == 0b11111100;
}
