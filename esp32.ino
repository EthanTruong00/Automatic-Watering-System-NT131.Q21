#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6SHeWCWKU"
#define BLYNK_TEMPLATE_NAME "Smart Watering System"
#define BLYNK_AUTH_TOKEN "EGcX0nlkD4x9Pm9G4Zvvn_jVLK6S1t1K" 

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "DHT.h"
#include <time.h> 

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Testdoan"; 
char pass[] = "tankietne123"; 

#define RELAY_ON LOW   
#define RELAY_OFF HIGH 

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;

const int pinChau[3] = {32, 33, 34};
const int pinVan[3] = {25, 26, 27};
const int pinBom = 14;
const int mucKho = 4095;
const int mucUot = 1350;

int doAm[3];
float nhietDoKK = 0;
float doAmKK = 0;
int cheDo[3] = {1, 1, 1}; 
int nguongProfile[3] = {60, 60, 60}; 
int gioHen = 7, phutHen = 0, doAmChung = 75, gioPhutDaTuoi = -1;     
bool dangTuoiHenGio = false;
bool vanHoanThanh[3] = {false, false, false};

int layNguongTuoi(int loaiCay) {
  switch (loaiCay) {
    case 0: return 90;  
    case 1: return 70;  
    case 2: return 70;  
    case 3: return 50;  
    case 4: return 60;  
    case 5: return 60;  
    case 6: return 65;  
    case 7: return 60;  
    case 8: return 60;  
    case 9: return 60;  
    case 10: return 0;  
    case 11: return 0;  
    default: return 0; 
  }
}

void docCamBien() {
  for (int i = 0; i < 3; i++) {
    if (nguongProfile[i] == 0) {
      doAm[i] = 0; Blynk.virtualWrite(i + 1, 0); continue;                      
    }
    int raw = analogRead(pinChau[i]);
    doAm[i] = map(raw, mucKho, mucUot, 0, 100);
    doAm[i] = constrain(doAm[i], 0, 100);
    Blynk.virtualWrite(i + 1, doAm[i]); 
  }
  float h = dht.readHumidity(); float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)) {
    nhietDoKK = t; doAmKK = h;
    Blynk.virtualWrite(V11, t); Blynk.virtualWrite(V12, h);
  }
}


void xuLyLogic() {

}

BLYNK_CONNECTED() { Blynk.syncAll(); }

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) { digitalWrite(pinVan[i], RELAY_OFF); pinMode(pinVan[i], OUTPUT); }
  digitalWrite(pinBom, RELAY_OFF); pinMode(pinBom, OUTPUT);
  dht.begin(); Blynk.begin(auth, ssid, pass); 
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  timer.setInterval(2000L, docCamBien); timer.setInterval(3000L, xuLyLogic);   
}
void loop() { Blynk.run(); timer.run(); }