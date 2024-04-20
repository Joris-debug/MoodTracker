#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

struct Vector2D {
  float x;
  float y;
  Vector2D() : x(0), y(0) {}
  Vector2D(int x, int y) : x(x), y(y) {}
  Vector2D operator+(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
  }
  Vector2D& operator+=(const Vector2D& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  Vector2D operator-(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
  }
  Vector2D& operator-=(const Vector2D& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  Vector2D operator*(float scalar) const {
    return Vector2D(x * scalar, y * scalar);
  }
};

class Display : public Adafruit_ST7735 {
  private:
    uint16_t* m_p_frameBuffer;
    uint m_bufferLength;
  public:
    Display(uint8_t cs, uint8_t dc, uint8_t rst);
    void drawImage(const uint16_t* source, int x, int y, int w, int h);
    void drawImage(const uint16_t* source, int x, int y, int w, int h, uint16_t chromaKey);
    void drawText(char* text, int x, int y, uint16_t color, int size);
    Vector2D centerText(char* text, int cursorX, int cursorY);
    void renderBuffer(void);
    uint16_t getPixel(int16_t x, int16_t y);
    void writePixel(int16_t x, int16_t y, uint16_t color) override;
    void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
    void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
    void fillScreen(uint16_t color) override;
    size_t write(uint8_t c) override;
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);
    ~Display();
};