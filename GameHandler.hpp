#pragma once
#include <Arduino.h>
#include "Regulator.hpp"
#include "Display.hpp"

class GameHandler {
  private:
    const static int STREET_OFFSET_FAR = 52;
    const static float CAR_SPEED;
    const static int FPS = 3;
    static GameHandler* s_instance;
    Display* m_p_tft;
    Regulator* m_p_pot;
    int m_carPos;
    GameHandler(void);
    void drawStreet(void);
    void moveCar(unsigned long deltaTime);
    void drawCar(void);
  public:
    void run(void);
    static GameHandler& getInstance(void);
    void init(Display* p_tft, Regulator* p_pot);
    ~GameHandler();
};