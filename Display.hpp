#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

struct Point {
  int x;
  int y;
};

class Display : public Adafruit_ST7735 {
  private:
    uint16_t* m_p_frameBuffer;
    uint m_bufferLength;
  public:
    Display(uint8_t cs, uint8_t dc, uint8_t rst);
    void drawImage(const uint16_t* source, int x, int y, int w, int h);
    void drawImage(const uint16_t* source, int x, int y, int w, int h, uint16_t chromaKey);
    void drawText(char *text, int x, int y, uint16_t color, int size);
    Point centerText(char *text, int cursorX, int cursorY);
    void renderBuffer(void);
    virtual void writePixel(int16_t x, int16_t y, uint16_t color);
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void fillScreen(uint16_t color);
    virtual size_t write(uint8_t c);
    virtual void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);
    ~Display();
};