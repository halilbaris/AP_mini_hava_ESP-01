/*
 * Proje normalde Access Point ve OTA ozelligi olmadan yayinlanmistir.
 * Ozerinde calismalarimla Access Point ozelligini ekledim daha sonrasinda ise OTA
 * ozelligini ekledim . Bu sekilde bu projeye bilgisayariniza baglamadan program
 * yuklemeniz mumkun .
 * Hava tahminlerini forecasit.io dan almaktadir API key icin buradan bir hesap
 * acmaniz gerekmektedir,
 * Projenin dosyalarin  http://blog.squix.ch sayfasindan alinarak degisiklikler yapilmis
 * ve bu degisiklikleri www.arduinom.org sayfasinda paylasima sinulmustur.
*/

#include <Wire.h>
#include <Ticker.h>
#include "ssd1306_i2c.h"
#include "icons.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   
#define TRIGGER_PIN 0

//pinmode(0, INPUT);
//digitalWrite(pin, HIGH);
//pinMode(0, INPUT_PULLUP);

#include <ESP8266WiFi.h>
#include "WeatherClient.h"


#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// OLED ekran baglantisisi
SSD1306 display(0x3c, 2, 0);
WeatherClient weather;
Ticker ticker;
void drawFrame1(int x, int y);
void drawFrame2(int x, int y);
void drawFrame3(int x, int y);
void drawFrame4(int x, int y);
void setReadyForWeatherUpdate();
void drawForecast(int x, int y, int dayIndex);
// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
void (*frameCallbacks[3])(int x, int y) = {drawFrame1, drawFrame2, drawFrame3};

// how many frames are there?
int frameCount = 3;
// on frame is currently displayed
int currentFrame = 0;

// your network SSID (name)
// your network password

// Go to forecast.io and register for an API KEY
String forecastApiKey = "7411d1baf79831c16f2cbebaab0fb482";

// Coordinates of the place you want
// weather information for
////////Sehir koordinatlari///////

//////LAHORE//////////////
double latitude = 31.5451;
double longitude = 74.3407;
//////KONYA////////
//double latitude = 37.872;
//double longitude = 32.4844;



// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = true;

#define TRIGGER_PIN 0
void setup() {

 WiFi.mode(WIFI_STA);
 ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
     display.clear();
    display.drawXbm(34,20, 60, 36, WiFi_Logo_bits);
    display.drawString(20, 1 , "GUNCELLENDI");
    
    display.display();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));

 display.clear();
    display.drawXbm(34,20, 60, 36, WiFi_Logo_bits);
    display.drawString(10, 1 , "GUNCELLENIYOR");
    display.display();


  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
 pinMode(TRIGGER_PIN, INPUT);


  pinMode(0, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  
  // initialize dispaly
  display.init();
  // set the drawing functions
  display.setFrameCallbacks(3, frameCallbacks);
  // how many ticks does a slide of frame take?
  display.setFrameTransitionTicks(10);

  display.clear();
  display.display();
  
/////////////////
 
   
//////////


  
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // update the weather information every 10 mintues only
  // forecast.io only allows 1000 calls per day
  ticker.attach(60 * 10, setReadyForWeatherUpdate);
   
    display.clear();
   display.drawXbm(34,15, 60, 36, WiFi_Logo_bits);
    display.drawString(0, 1 , "AP MODDA ACILDI");
    display.drawString(20,55,"192.168.4.1");
    display.display();
    display.display();
    
    WiFiManager wifiManager;
    wifiManager.autoConnect("MiniHAVA AP");

    wifiManager.setTimeout(60);
      //reset and try again, or maybe put it to deep sleep


 if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.clear();
    display.drawXbm(34,20, 60, 36, WiFi_Logo_bits);
    display.drawString(20, 1 , "HAVA DURUMU");
    display.display();
  delay(5000);
    
  }


}

void loop(){

 ArduinoOTA.handle();
   

    



  



  
  if (readyForWeatherUpdate && display.getFrameState() == display.FRAME_STATE_FIX) {
    readyForWeatherUpdate = false;
    weather.updateWeatherData(forecastApiKey, latitude, longitude);  
  }
  display.clear();
  display.nextFrameTick();
  display.display();
}

void setReadyForWeatherUpdate() {
  readyForWeatherUpdate = true;  
}

void drawFrame1(int x, int y) {
   display.setFontScale2x2(false);
   display.drawString(65 + x, 8 + y, "SIMDI");
   display.drawXbm(x+7,y+7, 50, 50, getIconFromString(weather.getCurrentIcon()));
   display.setFontScale2x2(true);
   display.drawString(64+ x, 20 + y, String(weather.getCurrentTemp()) + "C"); 
}

const char* getIconFromString(String icon) {
   //"clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, or partly-cloudy-night"
  if (icon == "clear-day") {
    return clear_day_bits;
  } else if (icon == "clear-night") {
    return clear_night_bits;  
  } else if (icon == "rain") {
    return rain_bits;  
  } else if (icon == "snow") {
    return snow_bits;  
  } else if (icon == "sleet") {
    return sleet_bits;  
  } else if (icon == "wind") {
    return wind_bits;  
  } else if (icon == "fog") {
    return fog_bits;  
  } else if (icon == "cloudy") {
    return cloudy_bits;  
  } else if (icon == "partly-cloudy-day") {
    return partly_cloudy_day_bits;  
  } else if (icon == "partly-cloudy-night") {
    return partly_cloudy_night_bits;  
  } 
  return cloudy_bits;
}

void drawFrame2(int x, int y) {
   display.setFontScale2x2(false);
   display.drawString(65 + x, 0 + y, "BUGUN");
   display.drawXbm(x,y, 60, 60, xbmtemp);
   display.setFontScale2x2(true);
   display.drawString(64 + x, 14 + y, String(weather.getCurrentTemp()) + "C");
   display.setFontScale2x2(false);
   display.drawString(66 + x, 40 + y, String(weather.getMinTempToday()) + "C/" + String(weather.getMaxTempToday()) + "C");  

}

void drawFrame3(int x, int y) {
   display.drawXbm(x+7,y+7, 50, 50, getIconFromString(weather.getIconTomorrow()));
   display.setFontScale2x2(false);
   display.drawString(65 + x, 7 + y, "YARIN");
   display.setFontScale2x2(true);
   display.drawString(64+ x, 20 + y, String(weather.getMaxTempTomorrow()) + "C");     
}

void drawSpinner(int count, int active) {
  for (int i = 0; i < count; i++) {
    const char *xbm;
    if (active == i) {
       xbm = active_bits;
    } else {
       xbm = inactive_bits;  
    }
    display.drawXbm(64 - (12 * count / 2) + 12 * i,56, 8, 8, xbm);
  }   
}

