#pragma once
#include <Arduino.h>

class Button {
  private:
    int m_pin;
    bool m_isActive;
  public:
    Button(int pin);
    inline void init(void) { pinMode(m_pin, INPUT_PULLUP); }
    bool hasBeenPressed(void);
};