#include <Preferences.h>
#include "ressources.hpp"
#include "Regulator.hpp"
#include "Button.hpp"
#include "Display.hpp"
#include "GameHandler.hpp"

#define TFT_CS         D2
#define TFT_RST        D3  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         D4
#define APP_BUZZ       D8
#define APP_POT        A0
#define APP_BUT        D1

#define NOTE_C4   	   262
#define NOTE_D4        294
#define NOTE_E4        330
#define NOTE_F4        349
#define NOTE_G4        392
#define NOTE_A4        440
#define NOTE_B4        494
#define NOTE_C5        523
#define NOTE_D5        587
#define NOTE_E5        659
#define NOTE_F5        698
#define NOTE_G5        784
#define NOTE_A5        880
#define NOTE_B5        988

const int           buttonDuration = 125; //How long the sound will play
const int               STREET_X1 = 60;
const int               STREET_X2 = 68;
const int             VOTE_MEMORY = 30;
const uint16_t            APP_RED = 0xa986;
const uint16_t          APP_GREEN = 0x9f2a;
const uint16_t         APP_YELLOW = 0xff86;

Regulator pot(APP_POT);
Button but(APP_BUT);
GameHandler game = GameHandler::getInstance();
Display tft(TFT_CS, TFT_DC, TFT_RST);
Preferences prefs;

enum Mood { Happy = 1, Neutral = 2, Sad = 3 };

void setup(void) {
  pinMode(APP_BUZZ, OUTPUT);
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  but.init();
  game.init(&tft, &pot);
  prefs.begin("my-app");
}

void loop() {
  drawMenu();
}

void playSound(int note, int duration) {
  tone(APP_BUZZ, note, duration);  
  delay(duration);
  noTone(APP_BUZZ);
}

void drawMenu(void) {
  int selected = 999; //Force the screen to always draw at the first cycle
  
  while(true) {
    int selectedPrevious = selected;
    selected = pot.getCurrentSection(3);  //We divide the potentiometer into 3 zones since we have 3 options in the menu    
    delay(5);

    if(but.hasBeenPressed()) {
      playSound(NOTE_C5, buttonDuration);
      switch(selected) {
        case 1:
          game.run();
          selected = pot.getCurrentSection(3);
          break;
        case 2:
          drawVoteMenu();
          selected = pot.getCurrentSection(3);
          break;
        case 3:
          drawVoteResults();
          drawMoodAverage();
          selected = pot.getCurrentSection(3);
          break;
      }
    } else if(selected == selectedPrevious) {
      continue;
    }

    tft.fillScreen(ST77XX_BLACK);
    switch(selected) {  //Draws the rectangle around the selection
      case 1:
        tft.drawRect(0, 0, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160 * 0.33, ST77XX_WHITE);
        break;
      case 2:
        tft.drawRect(0, ST7735_TFTHEIGHT_160 * 0.33, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160 * 0.33, ST77XX_WHITE);
        break;
      case 3:
        tft.drawRect(0, ST7735_TFTHEIGHT_160 * 0.66, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160 * 0.33, ST77XX_WHITE);
        break;
    }
    char tmpText[11] = "AutoByte";
    tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.165, ST77XX_WHITE, 2);
    strcpy(tmpText, "Abstimmen");
    tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.5, ST77XX_WHITE, 2);
    strcpy(tmpText, "Ergebnisse");
    tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.825, ST77XX_WHITE, 2);
    tft.renderBuffer();
  }
}

void drawSmiley(Mood mood) {
  switch(mood) {
    case Happy:
      tft.drawImage(happy, 0, -20, SMILEY_WIDTH, SMILEY_HEIGHT);
      break;
    case Neutral:
      tft.drawImage(neutral, 0, -20, SMILEY_WIDTH, SMILEY_HEIGHT);
      break;
    case Sad:
      tft.drawImage(sad, 0, -20, SMILEY_WIDTH, SMILEY_HEIGHT);
      break;
  }
}

void drawVoteMenu(void) {  
  int selected = 999; //Force the screen to always draw at the first cycle
  bool waiting = true;
  
  while(waiting) {
    int selectedPrevious = selected;
    selected = pot.getCurrentSection(3);

    if(but.hasBeenPressed()) {      
      playSound(NOTE_C5, buttonDuration);
      addVote(static_cast<Mood>(selected));
      drawVoteSuccess();
      return;
    }
    
    delay(5);
    if(selected == selectedPrevious) {
      continue;
    }
    //tft.fillScreen(ST77XX_BLACK);
    switch(selected) {  //Draws the rectangle around the selection
      case 1:        
        drawSmiley(Happy);
        break;
      case 2:        
        drawSmiley(Neutral);
        break;
      case 3:
        drawSmiley(Sad);
        break;
    }    
    char tmpText[14] = "Wie geht";
    tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.8, ST77XX_WHITE, 2);
    strcpy(tmpText, "es dir?");
    tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.92, ST77XX_WHITE, 2);
    tft.renderBuffer();
  }
}

void drawVoteSuccess(void) {
  tft.fillScreen(ST77XX_BLACK);
  char tmpText[14] = "Du hast";
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.18, ST77XX_WHITE, 2);
  strcpy(tmpText, "mit Erfolg");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.35, ST77XX_WHITE, 2);
  strcpy(tmpText, "abgestimmt");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.51, ST77XX_WHITE, 2);

  tft.drawImage(checkmark, (ST7735_TFTWIDTH_128 - CHECKMARK_WIDTH) * 0.5, ST7735_TFTHEIGHT_160 * 0.65, CHECKMARK_WIDTH, CHECKMARK_HEIGHT, 0x0000);
  tft.renderBuffer();
  waitForAction();
}

void drawVoteResults(void) {
  const int ROWS = 5;
  const int COLUMNS = 6;
  const int SQUARE_WIDTH = 16;
  const int Y_OFFSET = 8;
  const int X_OFFSET = 16;
  
  tft.fillScreen(ST77XX_BLACK);
  byte* votes = getVotes();
  int dayCount = 0;
  for(int i = 0; i < ROWS; i++) {
    for(int j = 0; j < COLUMNS; j++) {
      uint16_t color;
      switch(votes[dayCount]) {
        case 1:
          color = APP_GREEN;
          break;
        case 2:
          color = APP_YELLOW;
          break;
        case 3:
          color = APP_RED;
          break;
        default:
          color = 0x0000;
          break;
      }
      tft.fillRect(X_OFFSET + SQUARE_WIDTH * j, Y_OFFSET + (SQUARE_WIDTH + 1) * i,  SQUARE_WIDTH,  SQUARE_WIDTH, color); //+1 to compensate for the divider-line
      dayCount++;
    }
    if(i != ROWS) { //Dont draw a divider for the last row
      tft.drawLine(X_OFFSET, Y_OFFSET + (SQUARE_WIDTH + 1) * (i + 1), ST7735_TFTWIDTH_128 - X_OFFSET, Y_OFFSET + (SQUARE_WIDTH + 1) * (i + 1), ST77XX_BLACK);
    }
  }  
  char tmpText[] = "Hier siehst du";
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.65, ST77XX_WHITE, 1);
  strcpy(tmpText, "die Ergebnisse");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.75, ST77XX_WHITE, 1);
  strcpy(tmpText, "der letzten");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.85, ST77XX_WHITE, 1);
  strcpy(tmpText, "30 Tage");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.95, ST77XX_WHITE, 1);
  tft.renderBuffer();
  waitForAction();
}

void drawMoodAverage(void) {
  uint red = 0;
  uint green = 0;
  uint blue = 0;
  uint16_t color = 0;
  byte* votes = getVotes();
  for(int i = 0; i < VOTE_MEMORY; i++) {
      switch(votes[i]) {
        case 1:
          color = APP_GREEN; 
          break;
        case 2:
          color = APP_YELLOW;;
          break;
        case 3:
          color = APP_RED;
          break;
        default:
          color = 0x0000;
          break;
      }
      red += (color & 0b1111100000000000) >> 11;  //The color has palette mode 565
      green += (color & 0b0000011111100000) >> 5;
      blue += color & 0b0000000000011111;
  }
  red /= VOTE_MEMORY;
  green /= VOTE_MEMORY;
  blue /= VOTE_MEMORY;
  color = static_cast<uint16_t>((red << 11) | (green << 5) | blue);
  tft.fillScreen(color);
  char tmpText[] = "Im Schnitt";
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.26, ST77XX_BLACK, 2);
  strcpy(tmpText, "war das");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.42, ST77XX_BLACK, 2);
  strcpy(tmpText, "deine");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.58, ST77XX_BLACK, 2);
  strcpy(tmpText, "Stimmung");
  tft.drawText(tmpText, ST7735_TFTWIDTH_128 * 0.5, ST7735_TFTHEIGHT_160 * 0.76, ST77XX_BLACK, 2);
  tft.renderBuffer(); 
  waitForAction();
}

byte* getVotes(void) {  
  static byte votes[VOTE_MEMORY];
  prefs.getBytes("votes", votes, VOTE_MEMORY);
  return votes;
}

void addVote(Mood vote) {
  byte* votes = getVotes();
  for(int i = 0; i < VOTE_MEMORY; i++) {
    if(votes[i] == 0) { //The array is not full yet
      votes[i] = static_cast<byte>(vote);
      prefs.putBytes("votes", votes, VOTE_MEMORY);
      return;
    }
  }
  byte newVotes[VOTE_MEMORY];
  for(int i = 1; i < VOTE_MEMORY; i++) {
    newVotes[i - 1] = votes[i];
  }
  newVotes[VOTE_MEMORY - 1] = static_cast<byte>(vote);
  prefs.putBytes("votes", newVotes, VOTE_MEMORY);
}

void waitForAction(void) {
  while(!but.hasBeenPressed()) {
    delay(5);
  }  
  playSound(NOTE_C5, buttonDuration);
}

