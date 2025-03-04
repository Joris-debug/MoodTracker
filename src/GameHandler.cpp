#include "GameHandler.hpp"
#include "ressources.hpp"
#include <stdlib.h>     //rand, srand, sprintf

GameHandler* GameHandler::s_instance = nullptr;
const float GameHandler::CAR_SPEED = 0.04;
const float GameHandler::HAZARD_SPEED = 0.0002;
const float GameHandler::HAZARD_WIDTH_INCREASE = 52;

GameHandler::GameHandler(void) {
  m_p_tft = nullptr;
  m_p_pot = nullptr;
  m_carPos = ST7735_TFTWIDTH_128 * 0.5;
  m_obstacle = generateHazard();
  m_hitPoints = MAX_HP;
  m_startTime = 0;
  m_lastDamage = 0;
  m_gameSpeed = 1.0;
}

void GameHandler::init(Display* p_tft, Regulator* p_pot, Button* p_but, Preferences* p_prefs) {
  m_p_tft = p_tft;
  m_p_pot = p_pot;
  m_p_but = p_but;
  m_p_prefs = p_prefs;
}

GameHandler& GameHandler::getInstance(void) {
  if(s_instance == nullptr) {
    s_instance = new GameHandler;
  }
  return *s_instance;
}

void GameHandler::run(void) {  
  srand(micros());
  unsigned long lastTime = m_startTime = millis();
  unsigned long deltaTime = 0;      //No movement on the first frame
  while(m_hitPoints > 0) {
    drawStreet();
    moveCar(deltaTime);
    updateHazard(deltaTime);
    drawCar();
    drawHUD();
    m_p_tft->renderBuffer();
    increaseDifficulty();
    deltaTime = millis() - lastTime;  //Milliseconds that have passed since the last cycle of the game loop
    if(deltaTime < 1000 / FPS) {
      delay(1000 / FPS - deltaTime);
      deltaTime = 1000 / FPS;
    }
    lastTime = millis();
  }
  drawGameOver();
  /* Reset to default settings */
  m_carPos = ST7735_TFTWIDTH_128 * 0.5;
  m_obstacle = generateHazard();
  m_hitPoints = MAX_HP;
  m_lastDamage = 0;
  m_gameSpeed = 1.0;
}

void GameHandler::drawStreet(void) {  
  const static uint8_t STREET_OFFSET_FAR = 52;    //Offset from the very left/right of the screen
  m_p_tft->fillScreen(0x8410);
  m_p_tft->fillTriangle(0, 0, STREET_OFFSET_FAR, 0, 0, ST7735_TFTHEIGHT_160, ST77XX_GREEN);
  m_p_tft->fillTriangle(ST7735_TFTWIDTH_128, 0, ST7735_TFTWIDTH_128 - STREET_OFFSET_FAR, 0, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160, ST77XX_GREEN);
}

void GameHandler::drawCar(void) {
  bool carIsInvincible;
  if(m_lastDamage == 0) {     //The game has just started
    carIsInvincible = false;
  } else {
    carIsInvincible = m_lastDamage + DAMAGE_COOLDOWN > millis();
  }
  bool carIsHit = false;
  int x = m_carPos - 0.5 * CAR_WIDTH;
  int y = ST7735_TFTHEIGHT_160 - CAR_HEIGHT;
  int pixelCount = 0;
  for (int row = 0; row < CAR_HEIGHT; row++) { // For each scanline...
    for (int col = 0; col < CAR_WIDTH; col++) { // For each pixel...
      uint16_t nextPixel = pgm_read_word(car + pixelCount);
      if(nextPixel != ST77XX_GREEN) {   //If the pixel is not green, then it is part of the car
        if(carIsInvincible) {
          m_p_tft->writePixel(x + col, y + row, (millis() / 100) % 2 == 0 ? ST77XX_WHITE : nextPixel);  //The car flashes white if it is invincible 
        } else {
          if(m_p_tft->getPixel(x + col, y + row) == ST77XX_YELLOW) {  //Check if the car overlays an obstacle
            carIsHit = true;            
          }
          m_p_tft->writePixel(x + col, y + row, nextPixel);
        }
      }
      pixelCount++;
    }
  }
  if(carIsHit) {
    m_lastDamage = millis();
    m_hitPoints--;
  }
}

void GameHandler::drawHUD(void) {
  for(int i = 0; i < m_hitPoints; i++) {
    m_p_tft->drawImage(heart, ST7735_TFTWIDTH_128 * 0.05, i * HEART_HEIGHT, HEART_WIDTH, HEART_HEIGHT, ST77XX_BLACK);
  }
  uint secondsAlive = (millis() - m_startTime) / 1000;   //For how many seconds is the game running
  char tmpText[] = "Deine";
  m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.845, ST7735_TFTHEIGHT_160 * 0.04, ST77XX_BLACK, 1);  
  strcpy(tmpText, "Zeit:");
  m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.865, ST7735_TFTHEIGHT_160 * 0.14, ST77XX_BLACK, 1);
  m_p_tft->drawText(parseTimeToText(secondsAlive), ST7735_TFTWIDTH_128 * 0.86, ST7735_TFTHEIGHT_160 * 0.24, ST77XX_BLACK, 1);
}

void GameHandler::updateHazard(unsigned long deltaTime) {
  m_obstacle.progress += deltaTime * m_gameSpeed * HAZARD_SPEED;
  if(m_obstacle.progress > 1) {
    m_obstacle = generateHazard();
  }
  drawHazard();
}

void GameHandler::drawHazard(void) {
  /* Draw the base triangle */  
  const static float SIN_60 = 0.8660254;
  Vector2D currentPos = m_obstacle.path.getValue(m_obstacle.progress);
  int16_t x0 = int(currentPos.x);
  int16_t y0 = int(currentPos.y);
  uint8_t side_length = HAZARD_WIDTH_FAR + m_obstacle.progress * HAZARD_WIDTH_INCREASE;
  int16_t y12 = y0 + side_length * SIN_60;
  int16_t x1 = x0 - 0.5 * side_length;
  int16_t x2 = x0 + 0.5 * side_length;
  m_p_tft->fillTriangle(x0, y0, x1, y12, x2, y12, ST77XX_YELLOW);
  m_p_tft->drawTriangle(x0, y0, x1, y12, x2, y12, ST77XX_BLACK);

  /* Draw the exclamation mark */
  static const int MIN_WIDTH = 1;
  static const int MAX_WIDTH = 3;
  static const int THRESHOLD = 40;
  uint16_t markWidth = side_length < THRESHOLD ? MIN_WIDTH : MAX_WIDTH;
  uint16_t x3 = markWidth == MIN_WIDTH ? x0 : x0 - MAX_WIDTH / 2;
  uint16_t y3 = y0 + side_length * 0.15;
  uint16_t markLength = side_length * 0.5;
  m_p_tft->fillRect(x3, y3, markWidth, markLength, ST77XX_BLACK);
  uint16_t y4 = y0 + side_length * 0.75;
  m_p_tft->fillCircle(x0, y4, side_length * 0.05, ST77XX_BLACK);

}

void GameHandler::drawGameOver(void) {  
  m_p_tft->fillScreen(ST77XX_BLACK);
  char tmpText[14] = "Game Over";
  m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.117, ST77XX_WHITE, 2);
  /* Draw the current time */
  uint secondsAlive = (millis() - m_startTime) / 1000;   //For how many seconds was the game running
  strcpy(tmpText, "Zeit:");
  m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160  * 0.284, ST77XX_WHITE, 2);
  m_p_tft->drawText(parseTimeToText(secondsAlive), ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.4, ST77XX_WHITE, 2);
  /* Draw the high score */
  strcpy(tmpText, "Rekord:");
  m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160  * 0.617, ST77XX_WHITE, 2);
  uint highScore = m_p_prefs->getInt(HIGH_SCORE_KEY, 0); //Default high score is zero seconds
  m_p_tft->drawText(parseTimeToText(highScore), ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.733, ST77XX_WHITE, 2);

  if(secondsAlive > highScore) { //New high score
    m_p_prefs->putInt(HIGH_SCORE_KEY, secondsAlive);
    strcpy(tmpText, "Neuer Rekord,");
    m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.88, ST77XX_WHITE, 1);
    strcpy(tmpText, "gratulation!");
    m_p_tft->drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.95, ST77XX_WHITE, 1);
  }
  m_p_tft->renderBuffer();
  while(!m_p_but->hasBeenPressed()) {
    delay(5);
  }
}

char* GameHandler::parseTimeToText(uint totalSeconds) {
  uint8_t minutes = totalSeconds / 60;
  uint8_t seconds = totalSeconds % 60;
  static char timeText[7]; //The biggest possible score would be "255:59", so the score shouldn't be longer than 7 characters
  totalSeconds %= 15360; //Prevent overflow if the time is longer than 255 minutes and 50 seconds
  snprintf(timeText, sizeof(timeText), "%d:%02d", minutes, seconds);
  return timeText;
}

Hazard GameHandler::generateHazard(void) {
  Hazard newObstacle;
  newObstacle.progress = 0;
  newObstacle.side = bool(rand() % 2);
  Line newPath;
  if(newObstacle.side == 0) {
    newPath.start = Vector2D(HAZARD_LINE_START_X, -HAZARD_WIDTH_FAR);
    newPath.direction = Vector2D(HAZARD_LINE_END_X, ST7735_TFTHEIGHT_160 + HAZARD_WIDTH_FAR);
  } else {
    newPath.start = Vector2D(ST7735_TFTWIDTH_128 - HAZARD_LINE_START_X, -HAZARD_WIDTH_FAR);
    newPath.direction = Vector2D(ST7735_TFTWIDTH_128 - HAZARD_LINE_END_X, ST7735_TFTHEIGHT_160 + HAZARD_WIDTH_FAR);
  }
  newPath.direction -= newPath.start;
  newObstacle.path = newPath;
  
  return newObstacle;
}

void GameHandler::moveCar(unsigned long deltaTime) {
  uint8_t selected = m_p_pot->getCurrentSection(3);
  switch(selected) {
    case 1:        
      m_carPos -= deltaTime * m_gameSpeed * CAR_SPEED;
      break;
    case 2:        
      return;
    case 3:
      m_carPos += deltaTime * m_gameSpeed * CAR_SPEED;
      break;
  }

  if(m_carPos < 0.5 * CAR_WIDTH) {
    m_carPos = 0.5 * CAR_WIDTH;
  } else if(m_carPos > ST7735_TFTWIDTH_128 - 0.5 * CAR_WIDTH) {
    m_carPos = ST7735_TFTWIDTH_128 - 0.5 * CAR_WIDTH;
  }
}

void GameHandler::increaseDifficulty(void) {
    //Calculate how many seconds the game has been running
    uint secondsAlive = (millis() - m_startTime) / 1000;
    
    //Increase speed by 0.1 every 10 seconds
    m_gameSpeed = 1.0 + (secondsAlive / 10.0) * 0.1;

    //Ensure the game speed does not exceed MAX_GAME_SPEED
    if (m_gameSpeed > MAX_GAME_SPEED) {
        m_gameSpeed = MAX_GAME_SPEED;
    }
}

GameHandler::~GameHandler() {
  s_instance = nullptr;
}
