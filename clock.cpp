
#include "clock.h"

#define ST7735_GREY 0xBDF7
#define ofs 16

boolean initial = 1;

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;  // Saved h, m, s x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 64, osy = 64, omx = 64, omy = 64, ohx = 64, ohy = 64;  // Saved h, m, s x & y coords
uint16_t _x0 = 0, _x1 = 0, _y0 = 0, _y1 = 0;
uint16_t x3, y3, x6, y6, x9, y9, x12, y12;

// -----------------------------------------------------------------------------------
void CLOCK::X_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
// ===================================================================================
  x0 += ofs;
  x1 += ofs;
  tft.drawLine(x0, y0, x1, y1, color);
}

// ------------------------------------------------------------------------
void CLOCK::X_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
// ========================================================================
  x0 += ofs;
  tft.fillCircle(x0, y0, r, color);
}

// ----------------------------------------------------------
void CLOCK::X_drawPixel(int16_t x, int16_t y, uint16_t color) {
// ==========================================================
  x += ofs;
  tft.drawPixel(x, y, color);
}

// --------------------------------------------------------------------------------------------
void CLOCK::drawHand(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool erase, bool isSecond) {
// ============================================================================================

  uint16_t col = ST7735_BLACK;
  if (erase) col = ST7735_GREY;
  else {
    if (isSecond) col = ST7735_RED;
  }

  X_drawLine(x0, y0, x1, y1, col);

  #if defined(HAND_3LINES)
  if (x0 > x1) {
    X_drawLine(x0 - 1, y0 - 1, x1 - 1, y1 - 1, col);
    X_drawLine(x0 - 1, y0 + 1, x1 - 1, y1 + 1, col);
  }
  else {
    X_drawLine(x0 + 1, y0 - 1, x1 + 1, y1 - 1, col);
    X_drawLine(x0 + 1, y0 + 1, x1 + 1, y1 + 1, col);
  }
  #endif
}

// ----------------------------------------------------------------
void CLOCK::clock_printDateTime(char *ddmm, char *yyyy, char *wday) {
// ================================================================
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0, 0);
  tft.print(ddmm);
  tft.setCursor(0, 10);
  tft.print(wday);
  tft.setCursor(159 - 30, 0);
  tft.print(yyyy);
}

// ----------------------------------------------------
void CLOCK::clock_printTempHumi(char *temp, char *humi) {
// ====================================================
  int x, y;
  tft.setTextColor(ST7735_WHITE);
  //
  if (temp[0] != 0) {
    x = 0; y = 127 - 8;
    tft.fillRect(x, y, 6 * 7, 8, ST7735_BLACK);
    tft.setCursor(x, y);
    tft.print(temp);
  }
  //
  if (humi[0] != 0) {
    x = 159 - 24; y = 127 - 8;
    tft.fillRect(x, y, 4 * 7, 8, ST7735_BLACK);
    tft.setCursor(x, y);
    tft.print(humi);
  }
}

// -----------------------------
void CLOCK::printHour(int xhour) {
// =============================
  if (xhour == 12) tft.setCursor(x12, y12);
  else if (xhour == 3) tft.setCursor(x3, y3);
  else if (xhour == 6) tft.setCursor(x6, y6);
  else if (xhour == 9) tft.setCursor(x9, y9);
  else return;
  //
  tft.setTextColor(ST7735_BLACK);
  tft.print(xhour);
}

// ---------------------
void CLOCK::clock_init() {
// =====================
  tft.setFont();
  //tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  //tft.setRotation(1);
  tft.setTextSize(1);
  tft.fillScreen(ST7735_BLACK);
  //
  // --- Draw clock face ---
  X_fillCircle(64, 64, 61, ST7735_GREY);
  X_fillCircle(64, 64, 58, ST7735_WHITE);
  X_fillCircle(64, 64, 57, ST7735_GREY);

  // --- Draw 12 lines ---
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    _x0 = sx * 57 + 64;
    _y0 = sy * 57 + 64;
    _x1 = sx * 50 + 64;
    _y1 = sy * 50 + 64;
    //
    X_drawLine(_x0, _y0, _x1, _y1, ST7735_BLACK);
  }

   // --- Draw 60 dots ---
  for(int i = 0; i < 360; i += 6) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    _x0 = sx * 53 + 64;
    _y0 = sy * 53 + 64;
    
    X_drawPixel(_x0, _y0, ST7735_BLACK);
    if ((i == 0) || (i == 180)) {
      X_fillCircle(_x0, _y0, 1, ST7735_YELLOW);
      X_fillCircle(_x0 + 1, _y0, 1, ST7735_YELLOW);
      if (i == 0) {
        x12 = ofs + _x0 - 5;
        y12 = _y0 + 10;
        printHour(12);
      }
      else {
        x6 = ofs + _x0 - 2;
        y6 = _y0 - 16;
        printHour(6);
      }
    }
    if ((i == 90) || (i == 270)) {
      X_fillCircle(_x0, _y0, 1, ST7735_YELLOW);
      X_fillCircle(_x0 + 1, _y0, 1, ST7735_YELLOW);
      if (i == 90) {
        x3 = ofs + _x0 - 16;
        y3 = _y0 - 2;
        printHour(3);
      }
      else {
        x9 = ofs + _x0 + 10;
        y9 = _y0 - 2;
        printHour(9);
      }
    }
  }

  X_fillCircle(65, 65, 5, ST7735_BLUE);
}

// ---------------------------------------
void CLOCK::clock1(int hh, int mm, int ss) {
// =======================================
  // --- Pre-compute hand degrees, x & y coords for a fast screen update ---
  sdeg = ss * 6;                      // 0-59 -> 0-354
  mdeg = mm * 6 + sdeg * 0.01666667;  // 0-59 -> 0-360 - includes seconds
  hdeg = hh * 30 + mdeg * 0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
  hx = cos((hdeg - 90) * 0.0174532925);    
  hy = sin((hdeg - 90) * 0.0174532925);
  mx = cos((mdeg - 90) * 0.0174532925);    
  my = sin((mdeg - 90) * 0.0174532925);
  sx = cos((sdeg - 90) * 0.0174532925);    
  sy = sin((sdeg - 90) * 0.0174532925);

  if ((ss == 0) || (initial == true)) {
    initial = 0;
    // -- Erase hour and minute hand positions every minute ---
    drawHand(ohx, ohy, 65, 65, true, false);
    ohx = (hx * 33) + 65;    
    ohy = (hy * 33) + 65;
    //
    drawHand(omx, omy, 65, 65, true, false);
    omx = (mx * 44) + 65;
    omy = (my * 44) + 65;
  }

  // -- Redraw new second positions, hour and minute hands not erased here to avoid flicker ---
  // --- Alten Sekundenzeiger löschen ---
  drawHand(osx, osy, 65, 65, true, true);
  //
  // --- Stundenzeiger/Minutenzeiger bleiben gleich ---
  drawHand(ohx, ohy, 65, 65, false, false);
  drawHand(omx, omy, 65, 65, false, false);
  //
  // --- Neuen Sekundenzeiger anzeigen ---
  osx = (sx * 47) + 65;
  osy = (sy * 47) + 65;
  drawHand(osx, osy, 65, 65, false, true );

  // --- Ggf. die Stundenanzeige (12, 3, 6, 9) restaurieren ---
  printHour(12);
  printHour(3);
  printHour(6);
  printHour(9);

  // --- Punkt in der Mitte ---
  X_fillCircle(65, 65, 5, ST7735_BLUE);
}



