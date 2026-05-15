# 🌿 Smart Watering System - Hệ Thống Tưới Thông Minh Đa Vùng

![ESP32](https://img.shields.io/badge/ESP32-323232?style=for-the-badge&logo=espressif&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Blynk](https://img.shields.io/badge/Blynk_IoT-1DB954?style=for-the-badge&logo=blynk&logoColor=white)

Đồ án môn học **Embedded System & Wireless Network (NT131.P21)** - Đại học Công nghệ Thông tin (UIT).
Hệ thống tưới cây tự động giám sát qua IoT, ứng dụng thuật toán điều khiển thông minh để quản lý 3 vùng trồng độc lập chỉ với 1 máy bơm trung tâm.

* **Nhóm thực hiện:** Nhóm 07
* **Thành viên:** Trương Tấn Kiệt (24520917) - Lê Quang Minh (24521059)
* **Giảng viên hướng dẫn:** ThS. Đặng Lê Bảo Chương

---

## ✨ Tính Năng Nổi Bật (Core Features)

Hệ thống không chỉ dừng lại ở mức Bật/Tắt Relay cơ bản mà được thiết kế các thuật toán bảo vệ và tối ưu hóa chuẩn công nghiệp:

* 🛡️ **Ma trận an toàn Bơm - Van:** Cơ chế bảo vệ đường ống. Bơm trung tâm sẽ bị *ép tắt* nếu cả 3 Van đều khóa. Tích hợp quy trình **Khởi động mềm** (chống sụt áp) và **Ngắt xả áp suất** (tắt bơm trước, delay 0.5s xả áp rồi mới đóng van).
* 🌡️ **Thuật toán Bù nhiệt độ:** Khi nhiệt độ môi trường (DHT22) vượt mức 34°C, hệ thống tự động cộng thêm 10% vào ngưỡng tưới mục tiêu để bù đắp lượng nước bốc hơi nhanh, giúp cây chống sốc nhiệt.
* 💾 **Ghi nhớ trạng thái (NVS Persistence):** Cấu hình loại cây, chế độ (Auto/Manual) và lịch hẹn giờ được ghi trực tiếp vào bộ nhớ Flash (`Preferences.h`). Mạch tự phục hồi đúng trạng thái sau sự cố mất điện.
* ⏱️ **Hẹn giờ Offline (Local RTC):** Tích hợp module phần cứng DS3231. Hệ thống vẫn tự động tưới đúng giờ ngay cả khi mất kết nối mạng WiFi/Cloud.
* 📱 **Giao diện IoT thời gian thực:** Điều khiển và giám sát thông số môi trường từ xa qua ứng dụng Blynk (Độ trễ < 1s). Tích hợp Profile các loại cây (người dùng chọn tên cây, hệ thống tự nạp ngưỡng độ ẩm 50% - 90%).

---

## 🛠️ Yêu Cầu Phần Cứng (Hardware Requirements)

* Vi điều khiển: **ESP32 NodeMCU**
* Module thời gian thực: **DS3231 RTC**
* Cảm biến: **DHT22** (Nhiệt độ/Độ ẩm khí), **Cảm biến độ ẩm đất điện dung x3**
* Cơ cấu chấp hành: **Relay 4-Kênh 5V**, **Bơm chìm 12V**, **Van điện từ (Solenoid Valve) 12V x3**
* Nguồn: Nguồn tổ ong 12V DC, Module hạ áp (Buck Converter) LM2596 (12V xuống 5V).

---

## 📚 Thư Viện Cài Đặt (Dependencies)

Để biên dịch thành công mã nguồn, vui lòng cài đặt các thư viện sau trong Arduino IDE:
1. `BlynkSimpleEsp32.h` (Quản lý kết nối Blynk Cloud)
2. `DHT.h` (Của Adafruit)
3. `RTClib.h` (Của Adafruit - giao tiếp module DS3231)
4. `Preferences.h` (Tích hợp sẵn trong core ESP32)

---

## 🚀 Hướng Dẫn Sử Dụng (Quick Start)

1. **Clone repository này về máy:**
   ```bash
   git clone [https://github.com/your-username/smart-watering-system.git](https://github.com/your-username/smart-watering-system.git)