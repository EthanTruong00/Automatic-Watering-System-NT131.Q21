#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

const int pinChau1 = 32;
const int pinChau2 = 33;
const int pinChau3 = 34;

const int pinVan1 = 25;
const int pinVan2 = 26;
const int pinVan3 = 27;
const int pinBom  = 14;

// --- THÔNG SỐ ĐÃ ĐƯỢC BẠN HIỆU CHUẨN THỰC TẾ ---
const int mucKho = 4095; // Mức khô tuyệt đối
const int mucUot = 1350; // Mức nhúng ngập nước

void setup() {
  Serial.begin(115200);

  pinMode(pinVan1, OUTPUT);
  pinMode(pinVan2, OUTPUT);
  pinMode(pinVan3, OUTPUT);
  pinMode(pinBom, OUTPUT);
  
  digitalWrite(pinVan1, HIGH); 
  digitalWrite(pinVan2, HIGH); 
  digitalWrite(pinVan3, HIGH); 
  digitalWrite(pinBom, HIGH);  

  if (!rtc.begin()) {
    Serial.println("Loi RTC!");
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  // Đọc số thô
  int raw1 = analogRead(pinChau1);
  int raw2 = analogRead(pinChau2);
  int raw3 = analogRead(pinChau3);

  // Ép sang Phần trăm (0% - 100%)
  int phanTram1 = map(raw1, mucKho, mucUot, 0, 100);
  int phanTram2 = map(raw2, mucKho, mucUot, 0, 100);
  int phanTram3 = map(raw3, mucKho, mucUot, 0, 100);

  // Chốt không cho số nhảy lố
  phanTram1 = constrain(phanTram1, 0, 100);
  phanTram2 = constrain(phanTram2, 0, 100);
  phanTram3 = constrain(phanTram3, 0, 100);

  // IN KẾT QUẢ ĐÃ CHUẨN HÓA LÊN MÀN HÌNH
  Serial.print(now.hour(), DEC); Serial.print(':');
  Serial.print(now.minute(), DEC); Serial.print(':');
  Serial.print(now.second(), DEC);

  Serial.print("  ||  Độ ẩm chậu 1: "); Serial.print(phanTram1); Serial.print("%");
  Serial.print(" | Chậu 2: "); Serial.print(phanTram2); Serial.print("%");
  Serial.print(" | Chậu 3: "); Serial.print(phanTram3); Serial.println("%");

  // Vẫn giữ test Relay để nghe tiếng tạch tạch cho yên tâm
  digitalWrite(pinVan1, LOW);
  digitalWrite(pinVan2, LOW);
  digitalWrite(pinVan3, LOW);
  digitalWrite(pinBom, LOW);
  delay(1000); 

  digitalWrite(pinVan1, HIGH);
  digitalWrite(pinVan2, HIGH);
  digitalWrite(pinVan3, HIGH);
  digitalWrite(pinBom, HIGH);
  
  Serial.println("---------------------------------------------------");
  delay(2000); 
}