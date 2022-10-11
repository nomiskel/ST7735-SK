
#include "ST7735-SK.h"

// -------------------------------
bool getTempAndHumi(bool doAnyway) {
// ===============================
  static float temperature = 0;
  static float temp255 = 0;
  static float humi255 = 0;
  static uint32_t ts255 = 0;
  
  // --- Alle 10 Sekunden Temperatur und Humidity auslesen ---
  uint32_t tsNow = millis();
  if ((tsNow - ts255) < 10000) {
    if (! doAnyway) return false;
  }
  ts255 = tsNow;

  // -----------------------------------------
  // Get temperature event and print its value
  // -----------------------------------------
  bool restricted = false;
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    temperature = 0;
  }
  else {
    float temp = event.temperature;  // Nur Änderungen von 0.1° zulassen.!
    Serial.print(F("Temperature (before): "));
    String tmp = String(temperature, 1);
    Serial.print(tmp);
    Serial.println(F(" C"));
    //
    Serial.print(F("Temperature (now): "));
    tmp = String(temp, 1);
    Serial.print(tmp);
    Serial.println(F(" C"));
    //
    if (temperature == 0) {
      temperature = temp;
    }
    else {
      int t4a = int(temperature * 100);
      int t4b = int(temp * 100);
      if ((t4b - t4a) > 10) {
        temperature += 0.1;
        restricted = true;
      }
      else if ((t4b - t4a) < -10) {
        temperature -= 0.1;
        restricted = true;
      }
      else {
        temperature = temp;
      }
    }
  }

  // --------------------------------------
  // Get humidity event and print its value
  // --------------------------------------
  float humidity;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    humidity = 0;
  }
  else {
    humidity = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(int(humidity));
    Serial.println(F(" %"));
  }

  // ----------------------------
  // Daten zum TFT-Display senden
  // ----------------------------
  // --- Temperature ---
  float temp = temperature;
  float CalibrationValue = 0;
  temp += CalibrationValue;
  //
  // --- Humidity ---
  if (humidity < 0) {
    humidity = 0;
  }
  else if (humidity > 99) {
    humidity = 99;
  }
  int h2 = int(humidity);
  //
  String tmp;
  char buf[10];
  uint16_t col;
  //
  if ((temp255 != temp) || (doAnyway == true)) {
    temp255 = temp;
    //
    tmp = String(temp, 1);
    tmp += " ";
    sprintf(buf, "%s", tmp);
    col = GREEN;
    if (temp < 19) col = BLUE;
    if (temp > 25) col = RED;
    tft1_print(6, 3, buf, col);
  }

  if ((humi255 != humidity) || (doAnyway == true)) {
    humi255 = humidity;
    //
    sprintf(buf, "%02i", h2);
    col = GREEN;
    if (humidity < 30) col = BLUE;
    if (humidity > 70) col = RED;
    tft1_print(6, 4, buf, col);
  }

  return true;
}

// ----------
int SyncRTC() {
// ==========
  if (! NetworkAvailable) return 0;
  //
  // --- WLAN wieder enablen ? ---
  if (! WiFi.isConnected()) {
    bool ok = WiFi.begin();
    if (! ok) return 1;
  }
  // --- Zeit vom NTP-Server holen ---
  Serial.println(F("Getting NTP Date/Time"));
  configTzTime(TZ_INFO, NTPServer);  // ESP32 Systemzeit mit NTP Synchronisieren
  bool ok = getLocalTime(&TM, 10000);      // Versuche 10 s zu Synchronisieren
  if (! ok) {
      String xinfo = "timeClient.update() failed";
      Serial.println(xinfo);
      tft1_print_msg("NTP-Request", "ERROR.!", "", RED, 4000);
      return 2;
  }
  //
  // --- Set DT in RTC DS3231 ---
  int y = TM.tm_year + 1900;  //year(unix_epoch);
  int m = TM.tm_mon + 1;      //month(unix_epoch);
  int d = TM.tm_mday;         //day(unix_epoch);
  int h = TM.tm_hour;         //hour(unix_epoch);
  int n = TM.tm_min;          //minute(unix_epoch);
  int s = TM.tm_sec;          //second(unix_epoch);
  String ymd = String(y);
  ymd += "/";
  ymd += String(m);
  ymd += "/";
  ymd += String(d);
  Serial.println(ymd);
  // --- RTC DS3231 ok ? ---
  if (RTC_OK) {
    rtc.adjust(DateTime(y, m, d, h, n, s));
  }
  setTime(h, n, s, d, m, y);  // now() wird auf Local gesetzt und kann benutzt werden.
  //
  String xinfo = "SyncNTP successfully done";
  Serial.println(xinfo);
  tft1_print_msg("NTP-Request", "SUCCESS", "", GREEN, 2000);
  //
  return -1;
}

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
    txt11 += " ";
    if (txt11.length() < 11) {
      txt11 = " " + txt11;
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

  // --- DHT22 ---
  dht.begin();

  // --- Initialize RTC DS3231 RTC ? ---
  if (RTC_OK) {
    Serial.print("RTC DS3231 -> ");
    if (! rtc.begin()) {  // I2C = 0x68 und 0x57(Eprom)
      Serial.println("Error.!");
      Serial.println("Couldn't find RTC DS3231");
      tft1_print_msg("RTC DS3231", "ERROR.!", "rtc.begin()", RED, 0);
      Serial.flush();
      while(1) {}  // Das wars
    }
    else {
      Serial.println("RTC DS3231 successfully found");
      tft1_print_msg("RTC DS3231", "SUCCESS", "rtc.begin()", GREEN, 2000);
    }
  }
  else {
    // --- Use date and time from __DATE__ and __TIME__ ---
    // --- Maybe WLAN can't be started ---
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
    time_t Now = now();  // Add 13 Seconds
    Now += 13;
    setTime(Now);
  }

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
    Serial.println(" -> connected");
    String xxxxIP = WiFi.localIP().toString();
    Serial.print("IP: "); Serial.println(xxxxIP);
    char xxIP12[12];
    char xxIP34[12];
    splitIPAddress(xxxxIP, xxIP12, xxIP34);
    tft1_print_msg("IP:", xxIP12, xxIP34, WHITE, 3000);
    //
    #ifdef IP178
    if (! xxxxIP.startsWith("192.168.178.")) {
      Serial.println("Error in IP Address.!");
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
        //
        // --- Set date and time from NTP-Server ---
        int y = TM.tm_year + 1900;  //year(unix_epoch);
        int m = TM.tm_mon + 1;   //month(unix_epoch);
        int d = TM.tm_mday;  //day(unix_epoch);
        int h = TM.tm_hour;  //hour(unix_epoch);
        int n = TM.tm_min;   //minute(unix_epoch);
        int s = TM.tm_sec;   //second(unix_epoch);
        String ymd = String(y);
        ymd += "/";
        ymd += String(m);
        ymd += "/";
        ymd += String(d);
        Serial.println(ymd);
        // --- RTC DS3231 ok ? ---
        if (RTC_OK) {
          rtc.adjust(DateTime(y, m, d, h, n, s));
        }
        setTime(h, n, s, d, m, y);  // now() wird auf Local gesetzt und kann benutzt werden.
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

  #ifdef OK1  // dispModeBig = true
  tft.fillScreen(ST77XX_BLACK);
  tft2_print(0, 0, "00:00", YELLOW);
  tft1_print(0, 2, " 1.01.2000", WHITE);
  tft1_print(0, 3, "Temp: 0.0", GREEN);
  tft1_print(0, 4, "Humi: 00 %", CYAN);
  #endif

  #ifdef OK2  // dispModeBig = false
  tft.fillScreen(ST77XX_BLACK);
  tft1_print(0, 0, " 00:00 00", YELLOW);
  tft1_print(0, 1, " Montag", WHITE);
  tft1_print(0, 2, " 1.01.2000", WHITE);
  tft1_print(0, 3, "Temp: 0.0", GREEN);
  tft1_print(0, 4, "Humi: 00 %", CYAN);
  #endif
}

// --------
void loop() {
// ========
  static uint32_t msec255 = 0;
  static int xs255 = 0xFF;
  static int xn255 = 0xFF;
  static int xh255 = 0xFF;
  static int xwd255 = 0xFF;
  static int xd255 = 0xFF;
  static int xm255 = 0xFF;
  static int xy255 = 0xFF;
  static bool dispModeBig = false;
  static int minute255 = 0xFF;
  //
  uint32_t msecNow = millis();
  if ((msecNow - msec255) < 1000) return;
  msec255 = msecNow;
  //
  // --- Aktuelles date/time holen ---
  int xh, xn, xs,wd, xd, xm, xy;
  if (RTC_OK) {
    DateTime Now = rtc.now();
    xh = Now.hour();
    xn = Now.minute();
    xs = Now.second();
    //
    wd = Now.dayOfTheWeek() + 1;  // 6=Samstag
    xd = Now.day();
    xm = Now.month();
    xy = Now.year();
  }
  else {
    xs = second();  //numberOfSeconds(now());
    xn = minute();  //numberOfMinutes(now());
    xh = hour();    //numberOfHours(now());
    //
    wd = weekday();  // 0=Unused, 1=Sonntag, 7=Samstag
    xd = day();
    xm = month();
    xy = year();
  }
  Serial.printf("%02i.%02i.%04i %02i:%02i:%02i\r\n", xd, xm, xy, xh, xn, xs);
  //
  // --- Neue Minute ? ---
  if (minute255 != xn) {
    if (xn == 0) {
      if (xh == 4) {  // --- Um 4 Uhr mit NTP-Server synchronisieren ---
        int rc = SyncRTC();
        if (rc == -1) Serial.println("SyncRTC() ok");
        else Serial.printf("SyncRTC() failed with rc = %i.!\r\n", rc);
      }
      // --- Restore TFT ---
      xs255 = 0xFF;
      xn255 = 0xFF;
      xh255 = 0xFF;
      xwd255 = 0xFF;
      xd255 = 0xFF;
      tft1_print(0, 3, "Temp: ", GREEN);
      tft1_print(0, 4, "Humi: ?? %", CYAN);
      getTempAndHumi(true);
    }
    minute255 = xn;
  }
  //
  // --- Alle 15 Sekunden die Uhrzeit-Anzeige umswitchen ---
  if ((xs % 15) == 0) {
    char s11[] = "           ";
    tft1_print(0, 0, s11, BLACK);
    tft1_print(0, 1, s11, BLACK);
    //
    xs255 = 0xFF;
    xn255 = 0xFF;
    xh255 = 0xFF;
    xwd255 = 0xFF;
    //
    dispModeBig = !dispModeBig;
    if (dispModeBig) tft2_print(2, 0, ":", YELLOW);
    else tft1_print(3, 0, ":", YELLOW);
  }
  //
  char sec10 = xs / 10;
  sec10 += 48;
  char buf10[2];
  sprintf(buf10, "%c", sec10);
  //buf10[1] = 0;
  //Serial.println(buf10);
  //
  char sec1 = xs % 10;
  sec1 += 48;
  char buf1[2];
  sprintf(buf1, "%c", sec1);
  //buf1[1] = 0;
  //Serial.println(buf1);
  //
  if (dispModeBig) {
    if (xs255 != sec10) {
      tft1_print(10, 0, &buf10[0], RED);
      xs255 = sec10;
    }
    tft1_print(10, 1, &buf1[0], RED);
    //
    if (xn255 != xn) {
      char min[3];
      sprintf(min, "%02i", xn);
      tft2_print(3, 0, min, YELLOW);
      xn255 = xn;
    }
    //
    if (xh255 != xh) {
      char hrs[3];
      sprintf(hrs, "%02i", xh);
      tft2_print(0, 0, hrs, YELLOW);
      xh255 = xh;
    }
  }
  else {
    //  012345678
    // " 00:00 00"
    tft1_print(8, 0, &buf1[0], RED);
    if (xs255 != sec10) {
      tft1_print(7, 0, &buf10[0], RED);
      xs255 = sec10;
    }
    if (xn255 != xn) {
      char min[3];
      sprintf(min, "%02i", xn);
      tft1_print(4, 0, min, YELLOW);
      xn255 = xn;
    }
    if (xh255 != xh) {
      char hrs[3];
      sprintf(hrs, "%02i", xh);
      tft1_print(1, 0, hrs, YELLOW);
      xh255 = xh;
    }
    if (xwd255 != wd) {
      tft1_print(0, 1, wdays[wd], WHITE);
      xwd255 = wd;
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
  //
  // --- Temperatur und Humidity alle 10 Sekunden auslesen ---
  getTempAndHumi();
}


