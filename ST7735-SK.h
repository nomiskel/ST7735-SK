#ifndef ST7735_SK_H
#define ST7735_SK_H

#include <arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <NTPClient.h>        // include NTPClient library
#include <RTClib.h>
#include <Preferences.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>
#include <TimeLib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Fonts/FreeMono12pt7b.h>
#include "WLAN-SSID-PWD.h"    // set WiFi network SSID and password

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

// --- Temperatursensor DHT22 ---
#define DHTPIN 5
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);

// --- NTP-Server ---
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time
#define NTPServer "time.nist.gov"

//#define SEALEVELPRESSURE_HPA (1013.25)  // Needed for BME280

//#define CLEAR_PREF  // Comment out to clear preferences
//#define IP178       // Comment out to force IP: 192.168.178.xxx

// --- DS3231 RTC ---
RTC_DS3231 rtc;

// --- NTP-Client (Time Server) ---
struct tm TM;
WiFiUDP ntpUDP;

// --- For 1.44" and 1.8" TFT with ST7735 use: ---
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// --- WeekDays ---
static char *wdays[] = {" <Unused>  ", " Sonntag   ", " Montag    ", " Dienstag  ", " Mittwoch  ", " Donnerstag", " Freitag   ", " Samstag   "};

// --- return from centerText11 ---
static char txt12[12];

// --- WLLAN ok? ---
bool NetworkAvailable = false;

// --- Sync with NTP ---
bool syncNTP = false;

// --- RTC DS3231 vorhanden ? ---
bool RTC_OK = true;

// --- Prototypes ---
bool getTempAndHumi();
int SyncRTC();
void splitIPAddress(String xxxxIP, char *ip12, char *ip34);
char* centerText11(char *txt);
void tft1_print_msg(char *txt2, char *txt3, char *txt4, uint16_t col3, uint16_t pause);
void tft1_print(int16_t x, int16_t y, char *txt, uint16_t col);
void tft2_print(int16_t x, int16_t y, char *txt, uint16_t col);
void dt(int *d, int *m, int *y);
void setup();
void loop();

#endif





