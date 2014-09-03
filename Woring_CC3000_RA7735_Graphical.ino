/*************************************************** 
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /*
This example does a test of the TCP client capability:
  * Initialization
  * Optional: SSID scan
  * AP connection
  * DHCP printout
  * DNS lookup
  * Optional: Ping
  * Connect to website and print out webpage contents
  * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/
//digitalPin 7 is connected to the RESET pin on Arduino
//NOTE: you CANNOT program the board while they are connected
//by default digitalPin 13 will blink upon reset, so stick an LED in there

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <Wire.h>      
#include "Adafruit_RA8875.h"
#include <Adafruit_STMPE610.h>
#include "utility/debug.h"
#include <Time.h>
#include <SD.h>
#include <string.h>
#include "DHT.h"
#include <avr/wdt.h>
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

#define RA8875_INT 23
#define RA8875_CS 22
#define RA8875_RESET 29
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);


// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 28
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// These can be any pins:
#define RESET 41      // formerly 9 VS1053 reset pin (output)
#define CS 40        // formerly 10  VS1053 chip select pin (output)
#define DCS 38        // formerly 8 VS1053 Data/command select pin (output)
#define CARDCS 43     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 18       // formerly 3 VS1053 Data request, ideally an Interrupt pin

#define sclk 13
#define mosi 11
#define cs   10
#define dc   8
#define rst  0  // you can also connect this to the Arduino reset


// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID       "internet2.4"           // cannot be longer than 32 characters!
#define WLAN_PASS       "martylou9512"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  6000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

// What page to grab!
#define WEBSITE      "api.wunderground.com"
#define WEBPAGE2      "/api/f6d42935c5858b54/conditions/forecast/q/IL/Skokie.json"
//#define WEBPAGE2      "/api/f6d42935c5858b54/conditions/q/IL/Skokie.json"
#define WEBPAGE3      "/api/f6d42935c5858b54/forecast/q/IL/Skokie.json"

/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;
/****************************************************
    Screen
*******************************************************/

int x;        // coordinates for setup messages
int y;
int texTx;                // coordinates for text
int texTy;

//int clockCenterX = 400;   // Using 800 X 480 resolution
//int clockCenterY = 240;   // Center is offset to allow character lines of information
//int circleSize = 202;     // radius of clock
                  // touch screen coordinates
uint16_t px, py;
uint8_t pz;
uint16_t tsx, tsy;
uint8_t tsz;
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

 uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
/**************************************************
             counters
***************************************************/

int internetCtr = 0;
int bmpCtr = 0;
int searchIndex;
int foundIndex;
int indoorCtr = 62;
int titleCtr = 0, fahrenheitCtr = 0, fcttextCtr = 0, conditionsCtr = 0;


/********************************************************
                time
********************************************************/

int dayOfYear;                         // time variables
int mYsecond;
int mYminute;
int mYhour;
int mYweekDay;
int mYmonthDay;
int mYmonth;
int mYyear;
int prevSec;
int prevMin;
int prevHour;
int prevDay;
int xCtr = 0;
int buffCtr = 0;

#define DAYSINWEEK 8
#define CHARSINDAY 11
#define MONTHSINYEAR 14
#define CHARSINMONTH 6

/********************************************************
                                  Calendar
********************************************************/

#define MONTHSINYEAR 14
#define CHARSINMONTH 6
int doy;

static char months[MONTHSINYEAR][CHARSINMONTH] =
   {"Dec.", "Jan.", "Feb.", "March", "April", "May", "June", 
    "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."};

static char days[DAYSINWEEK][CHARSINDAY] =
   {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday" };

static int daysofmo[13] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};


/***********************************************************
       booleans
************************************************************/

boolean alarmSet = false;
boolean startQuote = false;
boolean lookingForobservationtime = false;
boolean lookingForfahrenheit = false;
boolean lookingFortitle = false;
boolean lookingForfcttext = false;
boolean lookingForweather = false;
boolean lookingFortemp_f = false;
boolean lookingForrelative_humidity = false;
boolean lookingForwind_dir = false;
boolean lookingForwind_mph = false;
boolean lookingForconditions = false;
boolean firsTtime = true;

/***************************************************
      Buffers
****************************************************/
String buff;

/******************************************************
            Weather
******************************************************/

int wl;
int wdl;
int wmph;
int l;
static char weather[36];
String observationtime;
//int highestBuffct = 0;
static char wind_mph[10];
static char temp_f[7]; 
static char relative_humidity[4];
static char wind_dir[10];
char title1[20];
char title2[20];
char title3[20];
char title4[20];
char title5[20];
char title6[20];
char title7[20];
char title8[20];
char fahrenheit0[4];
char fahrenheit1[4];
char fahrenheit2[4];
char fahrenheit3[4];
char fahrenheit4[4];
char fahrenheit5[4];
char fahrenheit6[4];
char fahrenheit7[4];
char fcttext1[200];
char fcttext2[200];
char fcttext3[200];
char fcttext4[200];
char conditions0[40];
char conditions1[40];
char conditions2[40];
char conditions3[40];
char conditions4[40];
char conditions5[40];

static char weatherChars[19][36] = { "Light Drizzle", "Light Rain", "Heavy Rain", "Rain", 
                                     "Light Snow", "Heavy Snow", "Snow",  "Hail", 
                                     "Light Fog", "Heavy Fog", "Fog",  "Haze",
                                     "Heavy Thunderstorm",  "Thunderstorm", "Overcast", "Clear",  
                                     "Partly Cloudy", "Mostly Cloudy",  "Scattered Clouds"};


static int weatherBmps[19] = { 3, 3, 3, 3, 5, 8, 8,  11, 2, 2, 2, 6, 10, 10, 6, 9, 1, 1, 1};


void setup(void)
{
  digitalWrite(7, HIGH); //We need to set it HIGH immediately on boot
  pinMode(7,OUTPUT);     //We can declare it an output ONLY AFTER it's HIGH
                         // (( HACKHACKHACKHACK ))
  Serial.begin(115200);
   observationtime.reserve(36);
//  Serial.println(F("Hello, CC3000!\n")); 
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC); 
  Serial.println(F("RA8875 start"));
  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480)) {
      Serial.println(F("RA8875 Not Found!"));
      while (1);
      }

  Serial.println(F("Found RA8875"));

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.graphicsMode();                 // go back to graphics mode
  tft.fillScreen(RA8875_BLUE);  
  /* Switch to text mode */  
  tft.textMode();
  tft.textEnlarge(1);  
  tft.textColor(RA8875_WHITE, RA8875_BLUE);
  x = 0;
  y = 0;
  tft.textSetCursor(x, y);
  tft.print(F("Free RAM: ")); 
  tft.print(getFreeRam(), DEC);
  y = y + 40;
  tft.touchEnable(true);

  if (!ts.begin()) {
      tft.textSetCursor(x, y);
      tft.print(F("Couldn't start touchscreen controller"));
      y = y + 40;
      Serial.println(F("Couldn't start touchscreen controller"));
      }
 tft.textSetCursor(x , y);
 tft.print(F("Touchscreen started"));
 y = y + 40;
 Serial.println(F("Touchscreen started"));

 tft.textSetCursor(x , y);
 tft.print(F("(")); tft.print(tft.width());           // display the screen size
 tft.print(F(", ")); tft.print(tft.height());
 tft.print(F(")"));
 y = y + 40;
 Serial.print(F("(")); Serial.print(tft.width());
 Serial.print(F(", ")); Serial.print(tft.height());
 Serial.println(F(")"));
 Serial.print("At start sd Free RAM: "); Serial.println(freeRam(), DEC); 
 Serial.print(F("Initializing SD card..."));
 tft.textSetCursor(x , y);
 tft.print(F("Initializing SD card..."));
 y = y + 40;
 pinMode(CARDCS, OUTPUT);
 if (!SD.begin(CARDCS)) {                       // using vs1053 sd card
     Serial.println(F("initialization failed!"));
     tft.textSetCursor(x , y);
     tft.print(F("initialization failed!"));
     y = y + 40;
     return;
     }
     
// Start dht22     
 dht.begin();
 
   delay(1000);   
   drawMainscreen();

   /* Initialise the module */
  Serial.println(F("\nInitializing...CC3000"));

  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
 //    tft.textSetCursor(x , y);
 //    tft.print(F("Attempting to connect to ")); tft.print(WLAN_SSID);
//     y = y + 40;

  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println("Connected to WiFi network!");
   
  Serial.println(F("Connected!"));
  
  // Wait for DHCP to complete 
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  // Display the IP address DNS, Gateway, etc. 
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  ip = 0;
   while (ip == 0) {
  // Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
 //     tft.textSetCursor(x , y);
  //   tft.print(WEBSITE); tft.print(F(" -> "));
  

    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  cc3000.printIPdotsRev(ip);
//  mprintIPdotsRev(ip);

}

void loop(void)
{

  if (prevSec != second()) {          // of it is a different second than the previous do the clock thing
      prevSec = second();
      tsz = 0;
      get_touch();     // check for touch
      if (tsz > 0){        // if there is pressure call the main menu
         tsz = 0;
         foreCast();
         drawMainscreen(); 
         firsTtime = true;   
         prevSec = 99;
        indoorCtr = 99;
        }   

      if (minute() ==  0 || minute() == 10 || minute() == 20 || 
          minute() == 30 || minute() == 40 || minute() == 50 || 
          firsTtime == true) {
          if (second() < 3 || firsTtime == true){           // top of the hour
              firsTtime = false;
              tft.fillRect(230, 280, 340, 130, GREEN);
              internetCtr++;  
  /* Try connecting to the website.
     Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
  */
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
titleCtr = 0;
fahrenheitCtr = 0;
fcttextCtr = 0;
conditionsCtr = 0;
              if (www.connected()) {
                  www.fastrprint(F("GET "));
                  www.fastrprint(WEBPAGE2);
                  www.fastrprint(F(" HTTP/1.1\r\n"));
                  www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
                  www.fastrprint(F("\r\n"));
                  www.println();
                  } else {
                          Serial.println(F("Connection failed"));  
                          Serial.flush();
                          www.close();
//                          cc3000.disconnect();    
                          cc3000.stop();
                          void(* resetFunc) (void) = 0; //declare reset function @ address 0
                          resetFunc();  //call reset
// worked                          Serial.println("RESET!");
// worked                         Serial.flush();
                          digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset.

                          return;
                          }
              Serial.println();
              Serial.println(F("-------------------------------------"));

              char compareChar = '"';
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
              unsigned long lastRead = millis();
              while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
              while (www.available()) {
                 char c = www.read();
                 Serial.print(c);
              lastRead = millis();
              if (c == compareChar) {
                  if (startQuote == false) {
                      startQuote = true;
                      buff = "";
                      }
                  else  {   // not quote
                         if (startQuote == true) {
                             startQuote = false;
                             if (lookingForobservationtime == true) {
                                 lookingForobservationtime = false; 
                                 l = buff.length();
                                 observationtime = "";
                                 observationtime = buff.substring(0, l-6);
//                                 Serial.println(); Serial.println(observationtime); Serial.println(buff);
                                 buff = "";                         
                                 }
                             else                         
                             if (buff == String("local_time_rfc822")){
                                 lookingForobservationtime = true;
                                 buff = "";      
                                 }
                             else
                             if (lookingForweather == true) {
                                 lookingForweather = false; 
                                 l = buff.length();
                                 wl = l;
                                 buff.toCharArray(weather, l+1);
//                                 Serial.println(); Serial.println(weather); Serial.println(buff);
                                 buff = "";
                                 }
                             else                         
                             if (buff == String("weather")){
                                 lookingForweather = true;
                                 buff = "";
                                 }
                             else
                             if (lookingFortemp_f == true) {
                                 lookingFortemp_f = false; 
                                 l = buff.length();
                                 buff.toCharArray(temp_f, l+1);
//                                 Serial.println(); Serial.println(temp_f); Serial.println(buff);
                                 buff = "";
                                 startQuote = true;
                                 }
                             else
                             if (buff == String("temp_f")){
                                  buff = "";  
                                 lookingFortemp_f = true;                         
                                 startQuote = true;
                                 }                  
                             else
                             if (lookingForrelative_humidity == true) {
                                 lookingForrelative_humidity = false; 
                                 l = buff.length();
                                 buff.toCharArray(relative_humidity, l);
//                                 Serial.println(); Serial.println(relative_humidity); Serial.println(buff);
                                 buff = "";
                                 }
                             else                         
                             if (buff == String("relative_humidity")){
                                 buff = "";  
                                 lookingForrelative_humidity = true;  
                                 }                               
                             else
                             if (lookingForwind_dir == true) {
                                 lookingForwind_dir = false; 
                                 l = buff.length();
                                 buff.toCharArray(wind_dir, l+1);
                                 wdl = l+1;
//                                Serial.println(); Serial.println(wind_dir); Serial.println(buff);
                                 buff = "";
                                 }
                             else                         
                             if (buff == String("wind_dir")){
                                 buff = "";  
                                 lookingForwind_dir = true;  
                                 }                               
                             else
                             if (lookingForwind_mph == true) {
                                 lookingForwind_mph = false; 
                                 l = buff.length();
                                 wmph = l+1;
                                 buff.toCharArray(wind_mph, l+1);
//                                 Serial.println(); Serial.println(wind_mph); Serial.println(buff);
                                 buff = "";
                                 startQuote = true;
                                 }
                             else                             
                             if (buff == String("wind_mph")){
                                 buff = "";
                                 lookingForwind_mph = true;
                                 startQuote = true;
                                 }
                             else
                             if (lookingFortitle == true) {
                                 lookingFortitle = false; 
                                 l = buff.length();
                                 if (titleCtr == 1 ) {
                                     buff.toCharArray(title1, l+1);
                                     }
                                 if (titleCtr == 2 ) {
                                     buff.toCharArray(title2, l+1);
                                     }
                                 if (titleCtr == 3 ) {
                                     buff.toCharArray(title3, l+1);
                                     }
                                 if (titleCtr == 4 ) {
                                     buff.toCharArray(title4, l+1);
                                     }
                                 if (titleCtr == 5 ) {
                                     buff.toCharArray(title5, l+1);
                                     }
                                 if (titleCtr == 6 ) {
                                     buff.toCharArray(title6, l+1);
                                     }
                                 if (titleCtr == 7 ) {
                                     buff.toCharArray(title7, l+1);
                                     }
                                 if (titleCtr == 8 ) {
                                     buff.toCharArray(title8, l+1);
                                     }

    //Serial.println(); Serial.print("titleCtr: "); Serial.print(titleCtr); Serial.print(" title: "); Serial.print(title[titleCtr]); Serial.print(" buff: "); Serial.println(buff);
                                 titleCtr++;
                                 buff = "";
                                 }
                              else
                              if (buff == String("title")){
                                  lookingFortitle = true;
                                  buff = "";
                                  }

                              if (lookingForfahrenheit == true) {
                                  lookingForfahrenheit = false; 
                                  l = buff.length();
                                  if (fahrenheitCtr == 0) {
                                      buff.toCharArray(fahrenheit0, l+1);
                                      }
                                  if (fahrenheitCtr == 1) {
                                      buff.toCharArray(fahrenheit1, l+1);
                                      }
                                  if (fahrenheitCtr == 2) {
                                      buff.toCharArray(fahrenheit2, l+1);
                                      }
                                  if (fahrenheitCtr == 3) {
                                      buff.toCharArray(fahrenheit3, l+1);
                                      }
                                  if (fahrenheitCtr == 4) {
                                      buff.toCharArray(fahrenheit4, l+1);
                                      }
                                  if (fahrenheitCtr == 5) {
                                      buff.toCharArray(fahrenheit5, l+1);
                                      }
                                  if (fahrenheitCtr == 6) {
                                      buff.toCharArray(fahrenheit6, l+1);
                                      }
                                  if (fahrenheitCtr == 7) {
                                      buff.toCharArray(fahrenheit7, l+1);
                                      }

                                  fahrenheitCtr++;
                                  buff = "";
                                  }
                              else
                             if (buff == String("fahrenheit")){
                                 lookingForfahrenheit = true;
                                 buff = "";
                                 }
                             else 
                             if (lookingForfcttext == true) {
                                 lookingForfcttext = false; 
                                 l = buff.length();
// Serial.println();  Serial.print("fcttextCtr: "); Serial.print(fcttextCtr); Serial.print(" L: "); Serial.print(l); Serial.print(" buff: "); Serial.print(buff);  
                                 if (fcttextCtr ==0) {
                                     buff.toCharArray(fcttext1, l);
// Serial.println();                   Serial.print(" fcttext1: ");Serial.println(fcttext1);
                                     }
                                 if (fcttextCtr ==3) {
                                     buff.toCharArray(fcttext2, l);
// Serial.println();                   Serial.print(" fcttext2: ");Serial.println(fcttext2);                                     
                                     }
                                 if (fcttextCtr ==5) {
                                     buff.toCharArray(fcttext3, l);
// Serial.println();                   Serial.print(" fcttext3: ");Serial.println(fcttext3);                                     
                                     }
                                 if (fcttextCtr ==7) {
                                     buff.toCharArray(fcttext4, l);
 //Serial.println();                   Serial.print(" fcttext4: ");Serial.println(fcttext4);                                     
                                     }
                                 fcttextCtr++;
                                 buff = "";
                                 }
                             else
                             if (buff == String("fcttext")){
                                 lookingForfcttext = true;
                                 buff = "";
                                 }
                             else 
                             if (lookingForconditions == true) {
                                 lookingForconditions = false; 
                                 l = buff.length();
// Serial.println();  Serial.print("fcttextCtr: "); Serial.print(fcttextCtr); Serial.print(" L: "); Serial.print(l); Serial.print(" buff: "); Serial.print(buff);  
                                 if (conditionsCtr ==0) {
                                     buff.toCharArray(conditions0, l+1);
// Serial.println();                   Serial.print(" conditions0: ");Serial.println(conditions0);
                                     }
                                 if (conditionsCtr ==1) {
                                     buff.toCharArray(conditions1, l+1);
// Serial.println();                   Serial.print(" conditions1: ");Serial.println(conditions1);                                     
                                     }
                                 if (conditionsCtr ==2) {
                                     buff.toCharArray(conditions2, l+1);
// Serial.println();                   Serial.print(" conditions2: ");Serial.println(conditions2);                                     
                                     }
                                 if (conditionsCtr ==3) {
                                     buff.toCharArray(conditions3, l+1);
 //Serial.println();                   Serial.print(" conditions3: ");Serial.println(conditions3);                                     
                                     }
                                 if (conditionsCtr ==4) {
                                     buff.toCharArray(conditions4, l+1);
 //Serial.println();                   Serial.print(" conditions4: ");Serial.println(conditions4);                                     
                                     }

                                 conditionsCtr++;
                                 buff = "";
                                 }
                             else
                             if (buff == String("conditions")){
                                 lookingForconditions = true;
                                 buff = "";
                                 }

                         }  // end of  startQuote == true              
                  } // end of else not quote
              } // end c ==
               else
              if (startQuote == true) buff += c;
              }  // end of while available   
          } // end of while connected

          Serial.println();

          www.close();
          tft.fillRect(230, 80, 340, 140,  CYAN);
          for (int i = 0; i < 20; i++) {
               int result = strcmp(weather, weatherChars[i]);
               if (result == 0) {
                   tft.graphicsMode();
                   switch (weatherBmps[i]){
                   case 1:
                          bmpDraw("cldy128.bmp", 330, 90);
                          break;
                   case 2:
                          bmpDraw("fog128.bmp", 330, 90);
                          break;
                   case 3:
                          bmpDraw("ltrain28.bmp", 330, 90);
                          break;
                   case 4:
                          bmpDraw("lteshw128.bmp", 330, 90);
                          break;
                   case 5:
                          bmpDraw("ltesnw128.bmp", 330, 90);
                          break;
                   case 6:
                          bmpDraw("ovrct128.BMP", 330, 90);
                          break;
                   case 7:
                          bmpDraw("rain128.bmp", 330, 90);
                          break;
                   case 8:
                          bmpDraw("snow128.bmp", 330, 90);
                          break;
                   case 9:
                          bmpDraw("suny128.bmp", 330, 90);
                          break;
                   case 10:
                          bmpDraw("thdst128.bmp", 330, 90);
                          break;
                   case 11:
                        bmpDraw("hail128.bmp", 330, 90);
                        break;
                   }  // end of switch               
                 }
               }
           x = 240;
           y = 230;
           tft.fillRect(x, y, 330, 25, CYAN);
           for (int i = 0; i < wl; i++){
                tft.drawChar(x, y, weather[i], BLACK, CYAN, 3);  // print  char                 
                x = x + 19;                             // move cursor to right
                }
          int x = 30;
          int y = 140;
          for (int i = 0; i < 2 ; i++){
               tft.drawChar(x, y,temp_f[i+1], BLACK, BLUE, 10);  
               x = x + 60;
               } 
          x = 30;
          y = 320;
          for (int i = 0; i < 2 ; i++){
               tft.drawChar(x, y,relative_humidity[i], BLACK, BLUE, 10);  
               x = x + 60;
               } 
          x = 240;
          y = 300;
          for (int i = 0; i < wdl ; i++){
               tft.drawChar(x, y,wind_dir[i], BLACK, GREEN, 5);  
               x = x + 30;
               } 
          x = 240;
          y = 360;
          for (int i = 0; i < wmph; i++){
               if (wind_mph[i+1] == ',') break;
               tft.drawChar(x, y, wind_mph[i+1], BLACK, GREEN, 5);  
               x = x + 30;
               }
          tft.textMode();
          tft.textEnlarge(1);  
          tft.textColor(BLACK, GREEN);
          tft.textSetCursor(x, y + 5);
          tft.print("Mph");
          tft.textMode();
          tft.textEnlarge(1);  
          tft.textColor(BLACK, WHITE);
          tft.textSetCursor(35, 10);
          tft.print(observationtime);
          tft.graphicsMode();
          tft.fillRect(300, 10, 140, 35, WHITE);
          int searchIndex;
          int foundIndex;
          char observeshortdays[DAYSINWEEK][4] =
                              {"Sat", "Sun", "Mon", "Tue", "Wed",
                               "Thu", "Fri", "Sat" };
          String foundValue = "";
          searchIndex = 0;
          foundIndex = observationtime.indexOf(',');
          foundValue = observationtime.substring(0, foundIndex);
//          Serial.print(foundValue);
          mYweekDay = 9;
          for (int i = 0; i < 7; i++) {
               if (foundValue == observeshortdays[i]) {
                   mYweekDay = i;
                   }
               }
//          Serial.print(" WD = "); Serial.println(mYweekDay);     
          searchIndex = foundIndex + 2;
          foundIndex = observationtime.indexOf(" ", searchIndex);
          foundValue = observationtime.substring(searchIndex, foundIndex);
          char tempday[foundValue.length() + 1];
          foundValue.toCharArray(tempday, sizeof(tempday));
  //        Serial.print(foundValue); 
          mYmonthDay = atoi(tempday);
  //        Serial.print(" MD + "); Serial.println(mYmonthDay);
          searchIndex = foundIndex + 1;
          foundIndex = observationtime.indexOf(" ", searchIndex);
          foundValue = observationtime.substring(searchIndex, foundIndex);
  //        Serial.println(foundValue);
          if (foundValue == String("Jan")) mYmonth = 1;
          if (foundValue == String("Feb")) mYmonth = 2;
          if (foundValue == String("Mar")) mYmonth = 3;
          if (foundValue == String("Apr")) mYmonth = 4;
          if (foundValue == String("May")) mYmonth = 5;
          if (foundValue == String("Jun")) mYmonth = 6;
          if (foundValue == String("Jul")) mYmonth = 7;
          if (foundValue == String("Aug")) mYmonth = 8;
          if (foundValue == String("Sep")) mYmonth = 9;
          if (foundValue == String("Oct")) mYmonth = 10;
          if (foundValue == String("Nov")) mYmonth = 11;
          if (foundValue == String("Dec")) mYmonth = 12;
          searchIndex = foundIndex + 1;
          foundValue = observationtime.substring(searchIndex, searchIndex + 4);
          char tempyear[foundValue.length() + 1];
          foundValue.toCharArray(tempyear, sizeof(tempyear));
  //        Serial.print(foundValue);
          mYyear = atoi(tempyear);
  //        Serial.print(" MY = "); Serial.println(mYyear);
          searchIndex = searchIndex + 5;
          foundValue = observationtime.substring(searchIndex, searchIndex + 2);
  //        Serial.print(foundValue);
          char temphour[foundValue.length() + 1];  
          foundValue.toCharArray(temphour, sizeof(temphour));
          mYhour = atoi(temphour);
  //        Serial.print( "hr = "); Serial.println(mYhour);
          searchIndex = searchIndex + 3;
          foundValue = observationtime.substring(searchIndex, searchIndex + 2);
  //        Serial.print(foundValue);
          char tempminute[foundValue.length() + 1];  
          foundValue.toCharArray(tempminute, sizeof(tempminute));
          mYminute = atoi(tempminute);
  //        Serial.print(" Min = "); Serial.println(mYminute);
          searchIndex = searchIndex + 3;
          foundValue = observationtime.substring(searchIndex, searchIndex + 2);
          char tempsecond[foundValue.length() + 1];  
          foundValue.toCharArray(tempsecond, sizeof(tempsecond));
  //        Serial.print(foundValue);
          mYsecond = atoi(tempsecond);
  //        Serial.println(" Sec = "); Serial.println(mYsecond);
          setTime(mYhour, mYminute, mYsecond, mYmonthDay, mYmonth, mYyear);
                 
 // Serial.println(F("-------------------------------------"));
  
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
 // Serial.println(F("\n\nDisconnecting"));
//  tft.setCursor(texTx, texTy);
//  tft.print("Disconnecting");
//  texTy = texTy + 10;
//  cc3000.disconnect();
//   cc3000.stop();
 // Serial.println();
   www.close();
  Serial.println("===================== Weather Site =========");  
  Serial.print("observationtime: "); Serial.println(observationtime);
  Serial.print("weather: "); Serial.println(weather);  
  Serial.print("Access ctr: "); Serial.println(internetCtr);
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC); 
  Serial.print(title1); Serial.print(" "); Serial.print(fahrenheit0); Serial.print("/"); Serial.print(fahrenheit1); Serial.print(" "); Serial.println(conditions1);
  Serial.println(fcttext1);
  Serial.print(title3); Serial.print(" "); Serial.print(fahrenheit2); Serial.print("/"); Serial.print(fahrenheit3); Serial.print(" "); Serial.println(conditions2);
  Serial.println(fcttext2);
  Serial.print(title5); Serial.print(" "); Serial.print(fahrenheit4); Serial.print("/"); Serial.print(fahrenheit5); Serial.print(" "); Serial.println(conditions3);
  Serial.println(fcttext3);
  Serial.print(title7); Serial.print(" "); Serial.print(fahrenheit6); Serial.print("/"); Serial.print(fahrenheit7); Serial.print(" "); Serial.println(conditions4);
  Serial.println(fcttext4);
  
             }
        }
      tft.textMode();
      tft.textEnlarge(1);
      tft.textColor(BLACK, WHITE);
        /* time of day */
      tft.textSetCursor(450, 10);
      tft.print(internetCtr);
      tft.textSetCursor(550, 10);
      int tempHr;
      if (hour() > 12) {
          tempHr = hour() - 12;
          }
          else {
                tempHr = hour();
                }
      if (hour() == 0) {
          tempHr = 12;
          }    
      if (tempHr < 10) {
          tft.print(F("0"));
          }
      tft.print(tempHr);
      tft.print(F(":"));
      if (minute() < 10) {
          tft.print(F("0"));
          }
      tft.print(minute());
      tft.print(F(":"));
      if (second() < 10) {
          tft.print(F("0"));
          }
      tft.print(second());
      if (hour() > 11) {
          tft.print(F(" P.M. "));
          }
      else {
            tft.print(F(" A.M. "));
            }
    /* alarm time0 */
      if (alarmSet == true) {
          tft.graphicsMode();
          tft.fillCircle(755, 20, 5, RED);
          tft.textMode();
          tft.textColor(RA8875_WHITE, RA8875_BLUE);     
          }
      indoorCtr++;
      if (indoorCtr > 60) {
          indoorCtr = 0;
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
          float h = dht.readHumidity();
          float t = dht.readTemperature();
   // check if returns are valid, if they are NaN (not a number) then something went wrong!
      if (isnan(t) || isnan(h)) {
          Serial.println("Failed to read from DHT");
         } 
         else {
               Serial.print("Humidity: "); 
               Serial.print(h);
               Serial.print(" %\t");
               int ph = h/10;
               tft.graphicsMode();
               char c[2];
               String str;
               str = String(ph);
               str.toCharArray(c, 2);
//         Serial.print(" ph: "); Serial.print(ph); Serial.print(" c: "); Serial.print(c[0]);
               tft.drawChar(630, 320, c[0], BLACK, YELLOW, 10);    
               ph = h - (ph * 10);
               str = String(ph);
               str.toCharArray(c, 2);
//         Serial.print(" ph: "); Serial.print(ph); Serial.print(" c: "); Serial.println(c[0]);
               tft.drawChar(690, 320, c[0], BLACK, YELLOW, 10);  
//    Serial.print("Temperature: "); 
//    Serial.print(t);
//    Serial.println(" *C");
               float temperatureF = (t * 9.0 / 5.0) + 32.0;
               Serial.print(temperatureF);
               Serial.println(" *F ");
               ph = temperatureF/10;
               str = String(ph);
               str.toCharArray(c, 2);
//         Serial.print(" ph: "); Serial.print(ph); Serial.print(" c: "); Serial.print(c[0]);
               tft.drawChar(630, 140, c[0], BLACK, YELLOW, 10);  
               ph = temperatureF - (ph *10);
               str = String(ph);
               str.toCharArray(c, 2);
//         Serial.print(" ph: "); Serial.print(ph); Serial.print(" c: "); Serial.println(c[0]);         
               tft.drawChar(690, 140, c[0], BLACK, YELLOW, 10); 
               } // end of else 
       }   // end of indoor ctr > 60
     }     // end of prevsec compare
}          // end of loop

// helper function to determine free ram at runtime
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

boolean checkLeapYear(int yearRequest){              // check for leap year
 if ( yearRequest%4 == 0 && yearRequest%100 != 0 || yearRequest % 400 == 0 ) return true;
     else  return false;
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, int x, int y) {
  bmpCtr++;
//  Serial.print("At drawbmp Free RAM: "); Serial.println(freeRam(), DEC);
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

//  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
  //      tft.setAddrWindow(x, y, x+w-1, y+h-1);  not needed for RA8875
  
        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
     //           tft.pushColors(lcdbuffer, lcdidx, first);  replace with drawPixel
                  tft.drawPixel(x + col, y + row, lcdbuffer[lcdidx]);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
//             lcdbuffer[lcdidx++] = tft.color565(r,g,b);   replaced with function call
            lcdbuffer[lcdidx] = color565(r,g,b);
            tft.drawPixel(x + col, y + row, lcdbuffer[lcdidx]);
                 } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
//          tft.pushColors(lcdbuffer, lcdidx, first);    replaced with drawPixel
            tft.drawPixel(col, row, lcdbuffer[lcdidx]);
        } 
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}
void mprintIPdotsRev(uint32_t ip) {

//  tft.print((uint8_t)(ip >> 24));
//  tft.print('.');
//  tft.print((uint8_t)(ip >> 16));
//  tft.print('.');
//  tft.print((uint8_t)(ip >> 8));
//  tft.print('.');
//  tft.print((uint8_t)(ip));  
}
/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
 
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
 //    tft.textSetCursor(x , y);
 //    tft.print(F("IP Addr: ")); mprintIPdotsRev(ipAddress);
     y = y + 40;
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
 //    tft.textSetCursor(x , y);
 //    tft.print(F("Netmask: ")); mprintIPdotsRev(netmask);
     y = y + 40;
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
 //    tft.textSetCursor(x , y);
 //    tft.print(F("Gateway: ")); mprintIPdotsRev(gateway);
     y = y + 40;
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
 //    tft.textSetCursor(x , y);
 //    tft.print(F("DHCPsrv: ")); mprintIPdotsRev(dhcpserv);
     y = y + 40;
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
  //   tft.textSetCursor(x , y);
  //   tft.print(F("DNServ:: ")); mprintIPdotsRev(dnsserv);
     y = y + 40;

    Serial.println();
    return true;
  }
}
