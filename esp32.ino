#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "TMPL6SHeWCWKU"
#define BLYNK_TEMPLATE_NAME "Smart Watering System"
#define BLYNK_AUTH_TOKEN    "EGcX0nlkD4x9Pm9G4Zvvn_jVLK6S1t1K"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"
#include <Preferences.h>

const char* WIFI_SSID = "Testdoan";
const char* WIFI_PASS = "tankietne123";

#define RELAY_ON  HIGH
#define RELAY_OFF LOW

#define DHTPIN  4
#define DHTTYPE DHT22

const int PIN_VAN[3]  = {25, 26, 27};
const int PIN_CHAU[3] = {32, 33, 35};
const int PIN_BOM     = 14;

// Giá trị ADC thực tế của cảm biến
const int ADC_KHO = 4095;
const int ADC_UOT = 1350;

DHT      dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
BlynkTimer timer;
Preferences prefs;

float    nhietDoKK = 0, doAmKK = 0;
int      doAmDat[3] = {0, 0, 0}; 

int      cheDo[3]   = {0, 0, 0};
int      loaiCay[3]     = {10, 10, 10};
int      nguongDoAm[3]  = {0,  0,  0};

int      gioHen[2]  = {7,  24};
int      phutHen[2] = {0,  0};
int      thoiGianTuoi = 5;       
bool     dangTuoiHenGio = false;
int      phutDaTuoi     = -1;
unsigned long thoiDiemBatDau = 0;

bool vanState[3] = {false, false, false};

inline int relayMuc(int vanIndex, bool batLen) {
  if (vanIndex == 0) batLen = !batLen; 
  return batLen ? RELAY_ON : RELAY_OFF;
}

int layNguong(int loai) {
  switch (loai) {
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
    default: return 0;  
  }
}
// ---------------------------------------------------------
// THUẬT TOÁN BẢO VỆ PHẦN CỨNG
// ---------------------------------------------------------
void ngatBomVaTatVan(int i) {
    digitalWrite(PIN_BOM, RELAY_OFF);
    Blynk.virtualWrite(V7, 0);
    delay(500); // Xả áp suất chống phụt nước
    digitalWrite(PIN_VAN[i], relayMuc(i, false));
    vanState[i] = false;
    Blynk.virtualWrite(V4 + i, 0);
}

void tatVan(int i, bool buocTat = false) {
  if (buocTat || cheDo[i] != 1) {
    bool cacVanKhacDong = true;
    for(int j=0; j<3; j++) {
       if (i != j && vanState[j]) cacVanKhacDong = false;
    }
    if (cacVanKhacDong) {
        ngatBomVaTatVan(i);
    } else {
        digitalWrite(PIN_VAN[i], relayMuc(i, false));
        vanState[i] = false;
        Blynk.virtualWrite(V4 + i, 0);
    }
  }
}

void moVan(int i) {
  digitalWrite(PIN_VAN[i], relayMuc(i, true));
  vanState[i] = true;
  Blynk.virtualWrite(V4 + i, 1);
}

bool coVanNaoMo() {
  for (int i = 0; i < 3; i++)
    if (vanState[i]) return true;
  return false;
}

bool coVanAutoMo() {
  for (int i = 0; i < 3; i++) {
    if (vanState[i] && (cheDo[i] == 1 || dangTuoiHenGio)) return true;
  }
  return false;
}
// ---------------------------------------------------------
// AUTO-SAVE LÊN FLASH MEMORY
// ---------------------------------------------------------
void luuCauHinh() {
  prefs.begin("garden", false);
  for (int i = 0; i < 3; i++) {
    prefs.putInt(("cd" + String(i)).c_str(), cheDo[i]);
    prefs.putInt(("lc" + String(i)).c_str(), loaiCay[i]);
  }
  prefs.putInt("gh0", gioHen[0]); prefs.putInt("ph0", phutHen[0]);
  prefs.putInt("gh1", gioHen[1]); prefs.putInt("ph1", phutHen[1]);
  prefs.putInt("tgt", thoiGianTuoi);
  prefs.end();
}

void taiCauHinh() {
  prefs.begin("garden", true);
  for (int i = 0; i < 3; i++) {
    cheDo[i]   = prefs.getInt(("cd" + String(i)).c_str(), 0);
    loaiCay[i] = prefs.getInt(("lc" + String(i)).c_str(), 10);
    nguongDoAm[i] = layNguong(loaiCay[i]);
  }
  gioHen[0]   = prefs.getInt("gh0", 7);
  phutHen[0]  = prefs.getInt("ph0", 0);
  gioHen[1]   = prefs.getInt("gh1", 24); 
  phutHen[1]  = prefs.getInt("ph1", 0);
  thoiGianTuoi = prefs.getInt("tgt", 5);
  prefs.end();
}
// ---------------------------------------------------------
// GIAO TIẾP VỚI CLOUD BLYNK
// ---------------------------------------------------------
BLYNK_WRITE(V20) { if (param.asInt() == 1) { luuCauHinh(); Blynk.virtualWrite(V20, 0); } }
BLYNK_WRITE(V16) { cheDo[0] = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V17) { cheDo[1] = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V18) { cheDo[2] = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V13) { gioHen[0]  = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V14) { phutHen[0] = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V21) { gioHen[1]  = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V22) { phutHen[1] = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V15) { thoiGianTuoi = param.asInt(); luuCauHinh(); }
BLYNK_WRITE(V8)  { loaiCay[0] = param.asInt(); nguongDoAm[0] = layNguong(loaiCay[0]); luuCauHinh(); }
BLYNK_WRITE(V9)  { loaiCay[1] = param.asInt(); nguongDoAm[1] = layNguong(loaiCay[1]); luuCauHinh(); }
BLYNK_WRITE(V10) { loaiCay[2] = param.asInt(); nguongDoAm[2] = layNguong(loaiCay[2]); luuCauHinh(); }

BLYNK_WRITE(V4) {
  if (!dangTuoiHenGio && cheDo[0] == 0) {
    if (param.asInt()) moVan(0); else tatVan(0, true);
  }
}
BLYNK_WRITE(V5) {
  if (!dangTuoiHenGio && cheDo[1] == 0) {
    if (param.asInt()) moVan(1); else tatVan(1, true);
  }
}
BLYNK_WRITE(V6) {
  if (!dangTuoiHenGio && cheDo[2] == 0) {
    if (param.asInt()) moVan(2); else tatVan(2, true);
  }
}

BLYNK_WRITE(V7) {
  if (dangTuoiHenGio || coVanAutoMo()) {
    Blynk.virtualWrite(V7, digitalRead(PIN_BOM) == RELAY_ON ? 1 : 0);
    return;
  }
  if (param.asInt() == 1) {
    if (coVanNaoMo()) {
        delay(300); // Khởi động mềm thủ công
        digitalWrite(PIN_BOM, RELAY_ON);
    } else {
        Blynk.virtualWrite(V7, 0); 
    }
  } else {
    digitalWrite(PIN_BOM, RELAY_OFF);
  }
}

// ---------------------------------------------------------
// ĐỌC CẢM BIẾN THỰC TẾ
// ---------------------------------------------------------
void docCamBien() {
  for (int i = 0; i < 3; i++) {
    if (nguongDoAm[i] == 0) {
      doAmDat[i] = 0;
      Blynk.virtualWrite(i + 1, 0);
      continue;
    }
    int raw = analogRead(PIN_CHAU[i]);
    doAmDat[i] = constrain(map(raw, ADC_KHO, ADC_UOT, 0, 100), 0, 100);
    Blynk.virtualWrite(i + 1, doAmDat[i]);
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)) {
    nhietDoKK = t;
    doAmKK    = h;
    Blynk.virtualWrite(V11, t);
    Blynk.virtualWrite(V12, h);
  }
}