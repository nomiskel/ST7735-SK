#ifndef clock_h
#define clock_h

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735

extern Adafruit_ST7735 tft;

class CLOCK {
private:
  void X_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void X_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void X_drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawHand(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool erase, bool isSecond);
  void printHour(int xhour);
public:
  void clock_printDateTime(char *ddmm, char *yyyy, char *wday);
  void clock_printTempHumi(char *temp, char *humi);
  void clock_init();
  void clock1(int hh, int mm, int ss);
};

#endif