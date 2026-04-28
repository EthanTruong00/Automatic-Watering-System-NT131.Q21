# Đồ án Mạng máy tính & Truyền thông - UIT
**Hệ thống Tưới cây Thông minh (ESP32)**
*Thực hiện: Trương Tấn Kiệt (24520917) & Ân (24520046)*

## 1. Tình trạng dự án
- **Thiết bị điều khiển trung tâm:** ESP32 DevKit V1 (DOIT).
- **Tiến độ hiện tại (Milestone 1):** Đã hoàn thành xử lý tín hiệu đầu vào. Đọc thành công dữ liệu từ 3 cảm biến độ ẩm đất (đã quy đổi ra thang 0-100%) và thời gian thực từ module RTC. Mạch chạy ổn định, không bị chạm nguồn.

## 2. Sơ đồ đấu nối phần cứng hiện tại
*Lưu ý quan trọng: Đã tách riêng trạm 5V (lấy từ chân VIN của ESP32) và trạm GND trên bo test. Tuyệt đối không cắm chung đường điện để tránh sập nguồn.*

**A. 3 Cảm biến độ ẩm đất (Analog):**
- Nguồn: Chân VCC -> Trạm 5V | Chân GND -> Trạm GND.
- Tín hiệu Chậu 1 -> Chân **D32** (ESP32).
- Tín hiệu Chậu 2 -> Chân **D33** (ESP32).
- Tín hiệu Chậu 3 -> Chân **D34** (ESP32).

**B. Module thời gian thực RTC DS3231:**
- Nguồn: Chân VCC -> Trạm 5V | Chân GND -> Trạm GND.
- Chân SDA -> Chân **D21** (ESP32).
- Chân SCL -> Chân **D22** (ESP32).

**C. Module Relay 4 kênh (Dự kiến đấu ở Giai đoạn 2):**
- Các chân điều khiển IN1, IN2, IN3 dự kiến nối vào các chân D25, D26, D27 trên ESP32.

## 3. Các thông số Code quan trọng
- **Tốc độ Serial Monitor:** 115200 baud.
- **Thư viện sử dụng:** `RTClib` (của tác giả Adafruit) cho module DS3231.
- **Logic cảm biến độ ẩm:** - Đọc tín hiệu Analog (dải giá trị 0 - 4095).
  - Mức khô (để ngoài không khí): ~3500.
  - Mức ướt (nhúng ngập nước): ~1200.
  - Sử dụng hàm `map()` để đảo ngược và quy đổi dải giá trị này về thang 0-100%.

## 4. Ghi chú sửa lỗi (Troubleshooting)
- **Lỗi `Failed to connect... exit status 2`:** Xảy ra do mạch bị kẹt code cũ không thể chuyển sang chế độ nạp. 
  - *Cách fix:* Rút cáp USB -> Nhấn và giữ chặt nút BOOT trên mạch -> Cắm cáp lại (tay vẫn giữ BOOT) -> Bấm Upload -> Chờ đến khi phần trăm nạp (`%`) hiện ra mới thả tay.
- **Lỗi ESP32 bị sập nguồn / Serial Monitor không hiện chữ:** Xảy ra do cắm nhầm nguồn VCC sang GND trên bo test, hoặc các module hút quá nhiều điện lúc khởi động.
  - *Cách fix:* Rút toàn bộ module ra để "cách ly". Nạp code thành công rồi mới cắm nóng lại từng thiết bị để tìm ra module gây chạm mạch.