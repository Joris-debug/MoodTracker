#include "Regulator.hpp"

Regulator::Regulator(int pin) {
  m_pin = pin;
  m_section = 1;
}

int Regulator::getCurrentSection(int zones) {
  int sensorValue = analogRead(m_pin);
  switch(zones) {
    case 1:
      m_section = 1;
      break;
    case 2:
      if(sensorValue > RESS_DIV_3 + TOLERANCE) {
        m_section = 1;
      } else if (sensorValue > RESS_DIV_3 - TOLERANCE) {
        m_section = 2;
      }
      break;
    case 3:
      if(sensorValue > RESS_DIV_1 + TOLERANCE) {
        m_section = 1;
      } else if(sensorValue < RESS_DIV_2 - TOLERANCE) {
        m_section = 3;
      } else if(sensorValue > RESS_DIV_2 + TOLERANCE && sensorValue < RESS_DIV_1 - TOLERANCE) {
        m_section = 2;
      }
      break;
    default:
      return 0;
  }
  return m_section;
}