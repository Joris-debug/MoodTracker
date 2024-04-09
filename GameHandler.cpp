#include "GameHandler.hpp"
#include "ressources.hpp"

GameHandler* GameHandler::s_instance = nullptr;
const float GameHandler::CAR_SPEED = 0.05;

GameHandler::GameHandler(void) {
  m_p_tft = nullptr;
  m_p_pot = nullptr;
  m_carPos = ST7735_TFTWIDTH_128 * 0.5;
}

void GameHandler::init(Display* p_tft, Regulator* p_pot) {
  m_p_tft = p_tft;
  m_p_pot = p_pot;
}

GameHandler& GameHandler::getInstance(void) {
  if(s_instance == nullptr) {
    s_instance = new GameHandler;
  }
  return *s_instance;
}

void GameHandler::run(void) {
  unsigned long lastTime = millis();
  unsigned long deltaTime = 0;      //No movement on the first frame
  while(true) {
    m_p_tft->fillScreen(ST77XX_WHITE);
    drawStreet();
    moveCar(deltaTime);
    drawCar();
    m_p_tft->renderBuffer();
    lastTime = millis();
    deltaTime = millis() - lastTime;  //Milliseconds that have passed since the last cycle of the game loop
    if(deltaTime < 1000 / FPS) {
      delay(1000 / FPS - deltaTime);
      deltaTime = 1000 / FPS;
    }
  }
}

void GameHandler::drawStreet(void) {
  m_p_tft->drawLine(STREET_OFFSET_FAR, 0, 0, ST7735_TFTHEIGHT_160, ST77XX_BLACK);
  m_p_tft->drawLine(ST7735_TFTWIDTH_128 - STREET_OFFSET_FAR, 0, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160, ST77XX_BLACK);
}

void GameHandler::drawCar(void) {
  m_p_tft->drawImage(car, m_carPos - 0.5 * CAR_WIDTH, ST7735_TFTHEIGHT_160 - CAR_HEIGHT, CAR_WIDTH, CAR_HEIGHT, ST77XX_GREEN);
}

void GameHandler::moveCar(unsigned long deltaTime) {
  uint8_t selected = m_p_pot->getCurrentSection(3);
  switch(selected) {
    case 1:        
      m_carPos -= deltaTime * CAR_SPEED;
      break;
    case 2:        
      return;
    case 3:
      m_carPos += deltaTime * CAR_SPEED;
      break;
  }

  if(m_carPos < 0.5 * CAR_WIDTH) {
    m_carPos = 0.5 * CAR_WIDTH;
  } else if(m_carPos > ST7735_TFTWIDTH_128 - 0.5 * CAR_WIDTH) {
    m_carPos = ST7735_TFTWIDTH_128 - 0.5 * CAR_WIDTH;
  }
}

GameHandler::~GameHandler() {
  s_instance = nullptr;
}
