#ifndef READ_DEBOUNCE_H
#define READ_DEBOUNCE_H

#include <Arduino.h>

class ReadDebounce {
public:
  ReadDebounce(uint8_t pin);
  uint8_t getState();
  void updateState();
  bool isRisingEdge();
  bool isFallingEdge();

private:
  uint8_t _pin;
  uint8_t _state = 0;
};

#endif