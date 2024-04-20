#include "Display.hpp"
#include "glcdfont.c"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

Display::Display(uint8_t cs, uint8_t dc, uint8_t rst) : Adafruit_ST7735(cs, dc, rst) {
  m_bufferLength = ST7735_TFTWIDTH_128 * ST7735_TFTHEIGHT_160;
  m_p_frameBuffer = new uint16_t[m_bufferLength] {0};
}

void Display::drawImage(const uint16_t* source, int x, int y, int w, int h) {
  int pixelCount = 0;
  for (int row = 0; row < h; row++) { // For each scanline...
    for (int col = 0; col < w; col++) { // For each pixel...
      //To read from Flash Memory, pgm_read_XXX is required.
      //Since image is stored as uint16_t, pgm_read_word is used as it uses 16bit address
      writePixel(x + col, y + row, pgm_read_word(source + pixelCount));
      pixelCount++;
    }
  }
}

void Display::drawImage(const uint16_t* source, int x, int y, int w, int h, uint16_t chromaKey) {
  int pixelCount = 0;
  for (int row = 0; row < h; row++) { // For each scanline...
    for (int col = 0; col < w; col++) { // For each pixel...
      if(pgm_read_word(source + pixelCount) != chromaKey) {
        writePixel(x + col, y + row, pgm_read_word(source + pixelCount));
      }
      pixelCount++;
    }
  }
}

void Display::drawText(char *text, int x, int y, uint16_t color, int size) {
  setTextWrap(false);
  setTextSize(size);
  Vector2D newPos = centerText(text, x, y);
  setCursor(newPos.x, newPos.y);
  setTextColor(color);
  print(text);
}

Vector2D Display::centerText(char *text, int cursorX, int cursorY) {
  int16_t x, y;
  uint16_t w, h;
  getTextBounds(text, cursorX, cursorY, &x, &y, &w, &h);
  Vector2D newPos;
  newPos.x = cursorX - 0.5 * w;
  newPos.y = cursorY - 0.5 * h;
  return newPos;
}

void Display::renderBuffer(void) {
  startWrite();
  setAddrWindow(0, 0, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160);
  writePixels(m_p_frameBuffer, m_bufferLength);
  endWrite();
}

uint16_t Display::getPixel(int16_t x, int16_t y) {
  if ((x >= 0) && (x < ST7735_TFTWIDTH_128) && (y >= 0) && (y < ST7735_TFTHEIGHT_160)) {
    return m_p_frameBuffer[x + y * ST7735_TFTWIDTH_128];
  }
  return 0;
}

void Display::writePixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < ST7735_TFTWIDTH_128) && (y >= 0) && (y < ST7735_TFTHEIGHT_160)) {
    m_p_frameBuffer[x + y * ST7735_TFTWIDTH_128] = color;
  }
}

void Display::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  for(int i = 0; i < h; i++) {
    writePixel(x, y + i, color);
  }
}

void Display::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  for(int i = 0; i < w; i++) {
    writePixel(x + i, y, color);
  }
}

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  if (x0 == x1) {
    if (y0 > y1) {
      _swap_int16_t(y0, y1);
    }
    writeFastVLine(x0, y0, y1 - y0 + 1, color);
  } else if (y0 == y1) {
    if (x0 > x1) {
      _swap_int16_t(x0, x1);
    }
    writeFastHLine(x0, y0, x1 - x0 + 1, color);
  } else {
    writeLine(x0, y0, x1, y1, color);
  }
}

void Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for(int i = 0; i < w; i++) {
    for(int j = 0; j < h; j++) {
      writePixel(x + i, y + j, color);
    }
  }
}

void Display::fillScreen(uint16_t color) {
  fillRect(0, 0, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160, color);
}

size_t Display::write(uint8_t c) {
  if (c == '\n') {              // Newline?
    cursor_x = 0;               // Reset x to zero,
    cursor_y += textsize_y * 8; // advance y one line
  } else if (c != '\r') {       // Ignore carriage returns
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x, textsize_y);
    cursor_x += textsize_x * 6; // Advance x one char
  }
  return 1;
}

void Display::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) {  
  if ((x >= _width) ||               // Clip right
      (y >= _height) ||              // Clip bottom
      ((x + 6 * size_x - 1) < 0) ||  // Clip left
      ((y + 8 * size_y - 1) < 0)) {  // Clip top
    return;
  }
  if (!_cp437 && (c >= 176)) {
    c++; // Handle 'classic' charset behavior
  }
  for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
    uint8_t line = pgm_read_byte(&font[c * 5 + i]);
    for (int8_t j = 0; j < 8; j++, line >>= 1) {
      if (line & 1) {
        if (size_x == 1 && size_y == 1) {
          writePixel(x + i, y + j, color);
        } else {
          fillRect(x + i * size_x, y + j * size_y, size_x, size_y, color);
        }
      } else if (bg != color) {
        if (size_x == 1 && size_y == 1) {
          writePixel(x + i, y + j, bg);
        } else {
          fillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
        }
      }
    }
  }
  if (bg != color) { // If opaque, draw vertical line for last column
    if (size_x == 1 && size_y == 1) {
      writeFastVLine(x + 5, y, 8, bg);
    } else {
      writeFillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
    }
  }
}

Display::~Display() {
  delete[] m_p_frameBuffer;
}