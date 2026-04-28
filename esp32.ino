#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

// Định nghĩa chân cảm biến
const int pinChau1 = 32;
const int pinChau2 = 33;
const int pinChau3 = 34;

// Mốc thông số để quy đổi sang phần trăm (bạn có thể tinh chỉnh sau)
const int mucKho = 3500; // Giá trị khi để ngoài không khí
const int mucUot = 1200; // Giá trị khi nhúng ngập nước

void setup() {
  Serial.begin(115200);

  // Kích hoạt mạch RTC
  if (!rtc.begin()) {
    Serial.println("Lỗi: Không tìm thấy mạch RTC DS3231! Kiểm tra lại dây SDA, SCL.");
    while (1) delay(10); // Dừng hệ thống nếu không thấy RTC
  }

  // RTC bị mất điện tự động lấy giờ từ máy tính nạp vào
  if (rtc.lostPower()) {
    Serial.println("RTC mất nguồn, đang tự động đồng bộ giờ máy tính...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // ĐỌC THỜI GIAN
  DateTime now = rtc.now();

  // 2. ĐỌC GIÁ TRỊ CẢM BIẾN
  int raw1 = analogRead(pinChau1);
  int raw2 = analogRead(pinChau2);
  int raw3 = analogRead(pinChau3);

  // QUY ĐỔI SANG PHẦN TRĂM (0-100%) bằng hàm map()
  int phanTram1 = map(raw1, mucKho, mucUot, 0, 100);
  int phanTram2 = map(raw2, mucKho, mucUot, 0, 100);
  int phanTram3 = map(raw3, mucKho, mucUot, 0, 100);

  // Chốt giới hạn: không cho số tụt xuống âm hoặc vọt lố 100%
  phanTram1 = constrain(phanTram1, 0, 100);
  phanTram2 = constrain(phanTram2, 0, 100);
  phanTram3 = constrain(phanTram3, 0, 100);

  // IN KẾT QUẢ RA MÀN HÌNH CHUẨN ĐẸP
  Serial.print(now.year(), DEC); Serial.print('/');
  Serial.print(now.month(), DEC); Serial.print('/');
  Serial.print(now.day(), DEC); Serial.print(" ");
  Serial.print(now.hour(), DEC); Serial.print(':');
  Serial.print(now.minute(), DEC); Serial.print(':');
  Serial.print(now.second(), DEC);

  Serial.print("  ||  Độ ẩm chậu 1: "); Serial.print(phanTram1); Serial.print("%");
  Serial.print(" | Chậu 2: "); Serial.print(phanTram2); Serial.print("%");
  Serial.print(" | Chậu 3: "); Serial.print(phanTram3); Serial.println("%");

  // 
  delay(2000); 
}