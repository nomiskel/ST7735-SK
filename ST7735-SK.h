#ifndef ST7735_SK_H
#define ST7735_SK_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <TimeLib.h>
#include <Fonts/FreeMono12pt7b.h>

#define OK2  // dispModeBig = false

#define BLACK ST77XX_BLACK
#define BLUE ST77XX_BLUE
#define CYAN ST77XX_CYAN
#define GREEN ST77XX_GREEN
#define MAGENTA ST77XX_MAGENTA
#define ORANGE ST77XX_ORANGE
#define RED ST77XX_RED
#define WHITE ST77XX_WHITE
#define YELLOW ST77XX_YELLOW

#if defined(ESP32)
  #define TFT_CS         17   // CS
  #define TFT_RST        14   // RESET
  #define TFT_DC         2    // A0
  // Ansonsten die Hardware Pins
  // #define TFT_MOSI    23   // SDA
  // #define TFT_SCLK    18   // SCK
#endif

// --- For 1.44" and 1.8" TFT with ST7735 use: ---
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// --- WeekDays ---
char *wdays[] = {" <Unused>  ", " Sonntag   ", " Montag    ", " Dienstag  ", " Mittwoch  ", " Donnerstag", " Freitag   ", " Samstag   "};

// --- Prototypes ---
void tft1_print(int16_t x, int16_t y, char *txt, uint16_t col);
void tft2_print(int16_t x, int16_t y, char *txt, uint16_t col);
void dt(int *d, int *m, int *y);
void setup();
void loop();

#endif





