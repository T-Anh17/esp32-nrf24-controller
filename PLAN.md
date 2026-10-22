# Kế hoạch

## Đã xong
- [x] Port firmware sang ESP32-S3 (T-Display-S3), chân nRF24 (SCK=12/MISO=13/MOSI=11), build + nạp OK.
- [x] Chạy headless: gỡ code OLED/bitmap, thay bằng stub no-op.
- [x] Chuyển cấu trúc dự án: `esp/` + `app/` + `docs/` (giống soundpad).

## Đang làm
- [ ] **App React Native** (transport: Wi-Fi HTTP, dùng API có sẵn của firmware).
  - Màn: Kết nối/Dashboard · Chọn tấn công · Màn đang chạy · WiFi tools · Cài đặt.
  - Bước hiện tại: duyệt wireframe → dựng app.

## Việc tiếp
- [ ] `docs/wiring.html`: sơ đồ board T-Display-S3 chi tiết (kiểu soundpad).
- [ ] Dọn `esp/html/`: bỏ trang browser không dùng + gỡ route tương ứng trong firmware.

## Transport (đã chốt)
App kết nối Wi-Fi AP `jammer` → gọi HTTP API firmware. Không dùng browser, không BLE.

## Chân nRF24 → T-Display-S3
SCK=12, MISO=13, MOSI=11 (cố định firmware) · CE/CSN đặt runtime (khuyến nghị 10/1) · VCC=3V3, GND=GND.
