#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

class Display : public Adafruit_ST7735 {
  public:
    Display(uint8_t cs, uint8_t dc, uint8_t rst);
    void drawImage(const uint16_t* source, int x, int y, int w, int h);
    void drawImage(const uint16_t* source, int x, int y, int w, int h, uint16_t chromaKey);
};