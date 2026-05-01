#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6SHeWCWKU"
#define BLYNK_TEMPLATE_NAME "Smart Watering System"
#define BLYNK_AUTH_TOKEN "EGcX0nlkD4x9Pm9G4Zvvn_jVLK6S1t1K"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "RTClib.h"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "TruongCao"; 
char pass[] = "0918540916";

RTC_DS3231 rtc;
BlynkTimer timer;

// 2. KHAI BÁO CHÂN CẮM
const int pinChau[3] = {32, 33, 34};      // 3 Cảm biến độ ẩm
const int pinRelay[4] = {25, 26, 27, 14}; // Van 1, Van 2, Van 3, Máy Bơm

// 3. THÔNG SỐ HIỆU CHUẨN CẢM BIẾN
const int mucKho = 4095;
const int mucUot = 1350;

// 4. BIẾN LƯU TRỮ ĐỘ ẨM VÀ NGƯỠNG TƯỚI
int doAmThucTe[3];
int nguongTuoi[3] = {60, 60, 60}; // Ngưỡng mặc định nếu chưa chọn cây

// ---  1: CÀI ĐẶT PROFILE CÂY 
void thietLapProfile(int chau, int loaiCay) {
  switch (loaiCay) {
    case 1: nguongTuoi[chau] = 80; break; // Rau Muống (Rất ưa nước)
    case 2: nguongTuoi[chau] = 75; break; // Hành lá / Mồng Tơi
    case 3: nguongTuoi[chau] = 70; break; // Rau Cải / Xà Lách
    case 4: nguongTuoi[chau] = 60; break; // Rau Dền / Rau Thơm
    case 5: nguongTuoi[chau] = 50; break; // Rau Ngót (Chịu hạn tốt)
    default: nguongTuoi[chau] = 60; break;
  }
  Serial.print(">> Đã cập nhật Chậu "); Serial.print(chau + 1);
  Serial.print(" thành Profile số: "); Serial.print(loaiCay);
  Serial.print(" | Ngưỡng tưới: Dưới "); Serial.print(nguongTuoi[chau]);
  Serial.println("% sẽ bơm.");
}

// --- 2: ĐỌC CẢM BIẾN & GỬI LÊN APP ---
void updateData() {
  for (int i = 0; i < 3; i++) {
    int raw = analogRead(pinChau[i]);
    doAmThucTe[i] = map(raw, mucKho, mucUot, 0, 100);
    doAmThucTe[i] = constrain(doAmThucTe[i], 0, 100);
    Blynk.virtualWrite(i + 1, doAmThucTe[i]); // Đẩy số liệu lên V1, V2, V3
  }
}

// --- HÀM 3: LOGIC TỰ ĐỘNG TƯỚI THEO PROFILE ---
void autoLogic() {
  bool canBom = false;
  for (int i = 0; i < 3; i++) {
    // Nếu độ ẩm thực tế thấp hơn ngưỡng của cây -> Mở Van
    if (doAmThucTe[i] < nguongTuoi[i]) {
      digitalWrite(pinRelay[i], LOW); // LOW là bật Relay
      canBom = true; // Báo hiệu là đang có van mở, cần bật bơm
    } else {
      digitalWrite(pinRelay[i], HIGH); // Đủ nước rồi -> Đóng van
    }
  }
  // Nếu có ít nhất 1 van mở thì bật Máy Bơm (Relay số 4)
  if (canBom) {
    digitalWrite(pinRelay[3], LOW); 
  } else {
    digitalWrite(pinRelay[3], HIGH);
  }
}

// --- HÀM 4: NHẬN LỆNH TỪ APP ĐIỆN THOẠI ---
// Nhận lệnh chọn Cây từ Menu
BLYNK_WRITE(V8) { thietLapProfile(0, param.asInt()); }
BLYNK_WRITE(V9) { thietLapProfile(1, param.asInt()); }
BLYNK_WRITE(V10) { thietLapProfile(2, param.asInt()); }

// Nhận lệnh bấm Nút thủ công
BLYNK_WRITE(V4) { digitalWrite(pinRelay[0], !param.asInt()); }
BLYNK_WRITE(V5) { digitalWrite(pinRelay[1], !param.asInt()); }
BLYNK_WRITE(V6) { digitalWrite(pinRelay[2], !param.asInt()); }
BLYNK_WRITE(V7) { digitalWrite(pinRelay[3], !param.asInt()); }

// --- CÀI ĐẶT HỆ THỐNG ---
void setup() {
  Serial.begin(115200);
  
  // Tắt toàn bộ Relay khi mới khởi động
  for (int i = 0; i < 4; i++) {
    pinMode(pinRelay[i], OUTPUT);
    digitalWrite(pinRelay[i], HIGH);
  }

  rtc.begin(); // Khởi động đồng hồ thời gian
  Blynk.begin(auth, ssid, pass); // Kết nối mạng

  // Hẹn giờ chạy 2 vòng lặp (Giúp WiFi không bị rớt)
  timer.setInterval(2000L, updateData); // 2 giây đọc cảm biến 1 lần
  timer.setInterval(5000L, autoLogic);   // 5 giây kiểm tra để tưới 1 lần
}

void loop() {
  Blynk.run();
  timer.run();
}