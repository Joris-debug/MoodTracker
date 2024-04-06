#pragma once
#include <Arduino.h>

class Regulator {
  private:
    static const int RESS_DIV_1 = 915;  //Divides the potentiometer into 3 zones
    static const int RESS_DIV_2 = 150;   //Divides the potentiometer into 3 zones
    static const int RESS_DIV_3 = 545;   //Divides the potentiometer into 2 zones
    static const int TOLERANCE = 10;
    int m_pin;
    int m_section;
  public:
    Regulator(int pin);
    int getCurrentSection(int zones);
};