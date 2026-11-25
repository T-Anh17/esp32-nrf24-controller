# Kế hoạch

## Đã xong
- [x] Port firmware sang ESP32-S3 (ESP32-S3-WROOM-1), chân nRF24 (SCK=12/MISO=13/MOSI=11), build + nạp OK.
- [x] Hiển thị trạng thái qua màn OLED SSD1306 (I2C).
- [x] Chuyển cấu trúc dự án: `esp/` + `app/` + `docs/` (giống soundpad).

## Đang làm
- [ ] **App React Native** (transport: Wi-Fi HTTP, dùng API có sẵn của firmware).
  - Màn: Kết nối/Dashboard · Chọn tấn công · Màn đang chạy · WiFi tools · Cài đặt.
  - Bước hiện tại: duyệt wireframe → dựng app.

## Việc tiếp
- [x] `docs/wiring.html` + `docs/schematic.html`: sơ đồ board ESP32-S3-WROOM-1 (3 nRF + OLED + 3 nút) chi tiết.
- [ ] Dọn `esp/html/`: bỏ trang browser không dùng + gỡ route tương ứng trong firmware.

## Transport (đã chốt)
App kết nối Wi-Fi AP `jammer` → gọi HTTP API firmware. Không dùng browser, không BLE.

## Chân nRF24 → ESP32-S3-WROOM-1
SCK=12, MISO=13, MOSI=11 (cố định firmware, dùng chung) · 3 module CE/CSN riêng: #1 1/2, #2 18/17, #3 6/7 · VCC=3V3, GND=GND. OLED SSD1306 SDA=8/SCL=9. Nút OK=0/Next=21/Prev=47. LED RGB GPIO38.
