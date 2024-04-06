#include "Button.hpp"

Button::Button(int pin) {
  m_pin = pin;
  m_isActive = true;
}

bool Button::hasBeenPressed(void) {
  bool buttonState = !digitalRead(m_pin);
  if(m_isActive == false) {
    m_isActive = buttonState;
    return buttonState;
  }
  m_isActive = buttonState;
  return false;
}
