#include "Display.hpp"

Display::Display(uint8_t cs, uint8_t dc, uint8_t rst) : Adafruit_ST7735(cs, dc, rst) {

}

void Display::drawImage(const uint16_t* source, int x, int y, int w, int h) {
  int pixelCount = 0;
  for (int row = 0; row < h; row++) { // For each scanline...
    for (int col = 0; col <w; col++) { // For each pixel...
      //To read from Flash Memory, pgm_read_XXX is required.
      //Since image is stored as uint16_t, pgm_read_word is used as it uses 16bit address
      drawPixel(x + col, y + row, pgm_read_word(source + pixelCount));
      pixelCount++;
    }
  }
}

void Display::drawImage(const uint16_t* source, int x, int y, int w, int h, uint16_t chromaKey) {
  int pixelCount = 0;
  for (int row = 0; row < h; row++) { // For each scanline...
    for (int col = 0; col <w; col++) { // For each pixel...
      if(pgm_read_word(source + pixelCount) != chromaKey) {
        drawPixel(x + col, y + row, pgm_read_word(source + pixelCount));
      }
      pixelCount++;
    }
  }
}