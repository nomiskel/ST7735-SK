
#include "ST7735-SK.h"

// -------------------------------------------------------
void splitIPAddress(String xxxxIP, char *ip12, char *ip34) {
// =======================================================
  int p = xxxxIP.indexOf('.', 0);
  p++;
  p = xxxxIP.indexOf('.', p);
  p++;
  String xx = xxxxIP.substring(0, p);
  xx.toCharArray(ip12, 12);
  //
  xx = xxxxIP.substring(p);
  xx.toCharArray(ip34, 12);
}

// --------------------------
char* centerText11(char *txt) {
// ==========================
  String txt11 = txt;
  //
  while (txt11.length() < 11) {
    txt11 = " " + txt11;
    if (txt11.length() < 11) {
      txt11 += " ";
    }
  }
  txt11.toCharArray(txt12, 12);
  
  return txt12;
}

// -----------------------------------------------------------------------------------
void tft1_print_msg(char *txt2, char *txt3, char *txt4, uint16_t col3, uint16_t pause) {
// ===================================================================================
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  //
  if (txt2 != "") tft1_print(0, 1, centerText11(txt2), WHITE);
  if (txt3 != "") tft1_print(0, 2, centerText11(txt3), col3);
  if (txt4 != "") tft1_print(0, 3, centerText11(txt4), WHITE);
  //
  if (pause > 0) delay(pause);
}

// -----------------------------------------------------------
void tft1_print(int16_t x, int16_t y, char *txt, uint16_t col) {
// ===========================================================
  tft.setTextSize(1);
  int16_t x1, y1;
  x1 = x * 14;
  //Serial.printf("tft.width = %d\r\n", tft.width()); = 160
  if (x1 >= tft.width()) return;
  x1++;
  //
       if (y == 0) y1 = 18;
  else if (y == 1) y1 = 42;
  else if (y == 2) y1 = 66;
  else if (y == 3) y1 = 90;
  else if (y == 4) y1 = 114;
  else return;

  size_t len = strlen(txt) * 14;
  tft.fillRect(x1, y1 - 15, len, 24, BLACK);
  tft.drawRect(x1, y1 - 15, len, 24, BLACK);
  
  tft.setCursor(x1, y1);
  tft.setTextColor(col);
  tft.print(txt);
}

// -----------------------------------------------------------
void tft2_print(int16_t x, int16_t y, char *txt, uint16_t col) {
// ===========================================================
  tft.setTextSize(2);
  int16_t x1, y1;
  x1 = x * 28;
  x1 -= 2;
  if (x1 >= tft.width()) return;
  //
       if (y == 0) y1 = 32;
  else if (y == 1) y1 = 71;
  else if (y == 2) y1 = 110;
  else return;

  size_t len = strlen(txt) * 28;
  tft.drawRect(x1, y1 - 28, len, 39, BLACK);
  tft.fillRect(x1, y1 - 28, len, 39, BLACK);
  
  tft.setCursor(x1, y1);
  tft.setTextColor(col);
  tft.print(txt);
}

// ----------------------------
void dt(int *d, int *m, int *y) {
// ============================
  int month, day, year;
  char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char buf[64];
  sscanf(__DATE__, "%s %d %d", buf, &day, &year);
  month = (strstr(month_names, buf) - month_names) / 3 + 1;
  //sprintf(buf, "%d%02d%02d", year, month, day);
  //
  *d = day;
  *m = month;
  *y = year;
}

// ---------
void setup() {
// =========
  Serial.begin(115200);
  delay(100);
  Serial.println(F("Hello.! - This is ST7735-SK Scetch"));

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  Serial.println(F("Initialized"));
  tft.setRotation(3);
  tft.setFont(&FreeMono12pt7b);
  tft1_print_msg("TFT ST7735", "OK", "INIT", GREEN, 2000);
  //
  // --- Initialize DS3231 RTC ---
  Serial.print("DS3231 -> ");
  if (! rtc.begin()) {  // I2C = 0x68 und 0x57(Eprom)
    Serial.println("Error.!");
    String xinfo = "Couldn't find RTC DS3231";
    Serial.println(xinfo);
    tft1_print_msg("RTC DS3231", "ERROR.!", "rtc.begin()", RED, 0);
    Serial.flush();
    while(1) {}  // Das wars
  }
  else {
    Serial.println("RTC DS3231 successfully found");
    tft1_print_msg("RTC DS3231", "SUCCESS", "rtc.begin()", GREEN, 1000);
  }
  //
  // --- Start WLAN ---
  Serial.println("Starting WLAN...");
  tft1_print_msg("TFT ST7735", "Starting", "WLAN", YELLOW, 1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to "); Serial.println(ssid);
  byte cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    cnt++;
    if (cnt == 20) break;
  }
  if (WiFi.status() == WL_CONNECTED) {
    tft1_print_msg("TFT ST7735", "SUCCESS", "WLAN", GREEN, 2000);
    NetworkAvailable = true;
    String xinfo = " -> connected";
    Serial.println(xinfo);
    String xxxxIP = WiFi.localIP().toString();
    xinfo = "IP: ";
    Serial.print(xinfo); Serial.println(xxxxIP);
    char xxIP12[12];
    char xxIP34[12];
    splitIPAddress(xxxxIP, xxIP12, xxIP34);
    tft1_print_msg("IP:", xxIP12, xxIP34, WHITE, 3000);
    //
    #ifdef IP178
    if (! xxxxIP.startsWith("192.168.178.")) {
      xinfo = "Error in IP Address.!";
      Serial.println(xinfo);
      tft1_print_msg(xxIP12, "ERROR.!", xxIP34, RED, 4000);
      NetworkAvailable = false;
    }
    #endif
    //
    if (NetworkAvailable) {
      // --- Zeit vom NTPServer holen ---
      Serial.println(F("Getting NTP Date/Time"));
      configTzTime(TZ_INFO, NTPServer);  // ESP32 Systemzeit mit NTP Synchronisieren
      bool ok = getLocalTime(&TM, 10000);      // Versuche 10 s zu Synchronisieren
      if (ok) {
        Serial.println("getLocalTime() - Success");
        tft1_print_msg("WLAN", "SUCCESS", "NTP-TIME", RED, 2000);
      }
      else {
        Serial.println("getLocalTime() - Failed.!");
        tft1_print_msg("WLAN", "ERROR.!", "NTP-TIME", RED, 3000);
      }
    }
  }
  else {
    Serial.println(F("Network is not available.!"));
    tft1_print_msg("TFT ST7735", "FAILED.!", "WLAN", RED, 4000);
  }


  
  //
  int day, month, year;
  dt(&day, &month, &year);
  //
  char tm[] = __TIME__;
  Serial.println(tm);
  String h = String(tm[0]);
  h += String(tm[1]);
  String m = String(tm[3]);
  m += String(tm[4]);
  String s = String(tm[6]);
  s += String(tm[7]);
  //
  setTime(h.toInt(), m.toInt(), s.toInt(), day, month, year);
  time_t Now = now();  // Add 17 Seconds
  Now += 17;
  setTime(Now);
  //
  #ifdef OK1  // dispModeBig = true
  tft.fillScreen(ST77XX_BLACK);
  tft2_print(0, 0, "00:00", YELLOW);
  tft1_print(0, 2, " 1.01.2000", WHITE);
  tft1_print(0, 3, "Temp: 23,4", GREEN);
  tft1_print(0, 4, "Humi: 50 %", CYAN);
  //tft.drawRect(0, 0, tft.width() - 1, tft.height() - 1, MAGENTA);
  #endif

  #ifdef OK2  // dispModeBig = false
  tft.fillScreen(ST77XX_BLACK);
  tft1_print(0, 0, " 00:00 00", YELLOW);
  tft1_print(0, 1, " Montag", WHITE);
  tft1_print(0, 2, " 1.01.2000", WHITE);
  tft1_print(0, 3, "Temp: 23,4", GREEN);
  tft1_print(0, 4, "Humi: 50 %", CYAN);
  //tft.drawRect(0, 0, tft.width() - 1, tft.height() - 1, MAGENTA);
  #endif
}

// --------
void loop() {
// ========
  static uint32_t msec255 = millis();
  static int secs255 = 0xFF;
  static int mins255 = 0xFF;
  static int hour255 = 0xFF;
  static int wday255 = 0xFF;
  static int xd255 = 0xFF;
  static int xm255 = 0xFF;
  static int xy255 = 0xFF;
  static bool dispModeBig = false;
  //
  uint32_t msecNow = millis();
  if ((msecNow - msec255) < 1000) return;
  msec255 = msecNow;
  //
  int secs = second();  //numberOfSeconds(now());
  int mins = minute();  //numberOfMinutes(now());
  //int hour = hour(); -> Erzeugt Fehlermeldung.!
  int hour = numberOfHours(now());
  //
  int wday = weekday();  // 0=Unused, 1=Sonntag, 7=Samstag
  int xd = day();
  int xm = month();
  int xy = year();
  Serial.printf("%02i.%02i.%04i %02i:%02i:%02i\r\n", xd, xm, xy, hour, mins, secs);
  //
  if ((secs % 15) == 0) {
    char s11[] = "           ";
    tft1_print(0, 0, s11, BLACK);
    tft1_print(0, 1, s11, BLACK);
    //
    secs255 = 0xFF;
    mins255 = 0xFF;
    hour255 = 0xFF;
    wday255 = 0xFF;
    //
    dispModeBig = !dispModeBig;
    if (dispModeBig) tft2_print(2, 0, ":", YELLOW);
    else tft1_print(3, 0, ":", YELLOW);
  }
  //
  char sec10 = secs / 10;
  sec10 += 48;
  char buf10[2];
  sprintf(buf10, "%c", sec10);
  //buf10[1] = 0;
  //Serial.println(buf10);
  //
  char sec1 = secs % 10;
  sec1 += 48;
  char buf1[2];
  sprintf(buf1, "%c", sec1);
  //buf1[1] = 0;
  //Serial.println(buf1);
  //
  if (dispModeBig) {
    if (sec10 != secs255) {
      tft1_print(10, 0, &buf10[0], RED);
      secs255 = sec10;
    }
    tft1_print(10, 1, &buf1[0], RED);
    //
    if (mins != mins255) {
      char min[3];
      sprintf(min, "%02i", mins);
      //tft2_print(3, 0, &min[0], YELLOW);
      tft2_print(3, 0, min, YELLOW);
      mins255 = mins;
    }
    //
    if (hour != hour255) {
      char hrs[3];
      sprintf(hrs, "%02i", hour);
      //tft2_print(0, 0, &hrs[0], YELLOW);
      tft2_print(0, 0, hrs, YELLOW);
      //tft.drawRect(0, 0, tft.width() - 1, tft.height() - 1, MAGENTA);
      hour255 = hour;
    }
  }
  else {
    //  012345678
    // " 00:00 00"
    tft1_print(8, 0, &buf1[0], RED);
    if (secs255 != sec10) {
      tft1_print(7, 0, &buf10[0], RED);
      secs255 = sec10;
    }
    if (mins255 != mins) {
      char min[3];
      sprintf(min, "%02i", mins);
      tft1_print(4, 0, min, YELLOW);
      mins255 = mins;
    }
    if (hour255 != hour) {
      char hrs[3];
      sprintf(hrs, "%02i", hour);
      tft1_print(1, 0, hrs, YELLOW);
      hour255 = hour;
    }
    if (wday255 != wday) {
      tft1_print(0, 1, wdays[wday], WHITE);
      wday255 = wday;
    }
  }
  //
  if ((xd255 != xd) || (xm255 != xm) || (xy255 != xy)) {
    char buf[12];
    sprintf(buf, "%2i.%02i.%04i ", xd, xm, xy);
    tft1_print(0, 2, buf, WHITE);
    xd255 = xd;
    xm255 = xm;
    xy255 = xy;
  }
}


