# nRF24 Jammer — ESP32-S3 + React Native

Bộ công cụ nghiên cứu RF 2.4GHz trên **LilyGo T-Display-S3** (chạy headless) với module **NRF24L01+PA+LNA**, điều khiển bằng **app React Native** qua Wi-Fi (HTTP API).

> ⚠️ Chỉ dùng cho nghiên cứu/giáo dục trong môi trường được phép. Gây nhiễu sóng vô tuyến là bất hợp pháp ở hầu hết các nước.

## Cấu trúc

| Thư mục | Nội dung |
|---------|----------|
| `esp/` | Firmware ESP32-S3 (PlatformIO + Arduino). Phát Wi-Fi AP, expose HTTP API điều khiển. |
| `app/` | App React Native điều khiển (chọn chức năng, cấu hình, gửi lệnh). |
| `docs/` | Tài liệu: sơ đồ nối dây (`wiring.html`). |

## Kết nối

- Firmware: `cd esp && pio run -e esp32-s3 -t upload`
- Phần cứng: xem [docs/wiring.html](docs/wiring.html)
- App ↔ ESP: Wi-Fi AP `nRF24-Tool` (mật khẩu `123456788`) → HTTP API (`/api/status`, `/WebCommand`, …)
