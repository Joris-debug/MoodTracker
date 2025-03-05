#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "Regulator.hpp"
#include "Display.hpp"
#include "Button.hpp"

#define HIGH_SCORE_KEY "highScore"
#define MAX_GAME_SPEED 3

struct Line {
  Vector2D start;
  Vector2D direction;
  Line() : start(), direction() {}
  Line(Vector2D start, Vector2D direction) : start(start), direction(direction) {}
  Vector2D getValue(float lambda) { 
    return start +  direction * lambda;
  }
};

struct Hazard {
  bool side;         //0 = left, 1 = right
  double progress;   //Between 0 and 1 (0% - 100%)
  Line path;         //The obstacle travels on this line
};

class GameHandler {
  private:
    const static uint8_t MAX_HP = 3;
    const static uint DAMAGE_COOLDOWN = 2000;
    const static float CAR_SPEED;
    const static float HAZARD_SPEED;
    const static uint8_t FPS = 60;
    const static uint8_t HAZARD_LINE_START_X = 58;  //Offset from the very left/right of the screen
    const static uint8_t HAZARD_LINE_END_X = 32;
    const static uint8_t HAZARD_WIDTH_FAR = 11;     //How big is the obstacle when it spawns
    const static float HAZARD_WIDTH_INCREASE;       //How much does the progress increase the obstacle size
    static GameHandler* s_instance;
    Display* m_p_tft;
    Regulator* m_p_pot;
    Button* m_p_but;
    Preferences* m_p_prefs;
    uint8_t m_carPos;
    int8_t m_hitPoints;
    Hazard m_obstacle;
    unsigned long m_startTime;    //Timestamp of the start of the game
    unsigned long m_lastDamage;   //Timestamp of when the last damage was taken
    float m_gameSpeed;            //Decides how fast the game runs (Increases difficulty)
    GameHandler(void);
    void drawStreet(void);
    void moveCar(unsigned long deltaTime);
    void drawCar(void);
    void drawHUD(void);
    void updateHazard(unsigned long deltaTime);
    void drawHazard(void);
    void drawGameOver(void);
    char* parseTimeToText(uint secondsAlive);
    Hazard generateHazard(void);
    void increaseDifficulty(void);
  public:
    void run(void);
    static GameHandler& getInstance(void);
    void init(Display* p_tft, Regulator* p_pot, Button* p_but, Preferences* p_prefs);
    ~GameHandler();
};