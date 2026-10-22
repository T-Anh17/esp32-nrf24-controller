# Firmware nRF24 Jammer — LilyGo T-Display-S3

Firmware bộ công cụ nghiên cứu RF 2.4GHz, port cho **LilyGo T-Display-S3 (ESP32-S3)** chạy **headless** (không OLED), điều khiển qua **Wi-Fi AP + HTTP API** (dùng với app React Native ở [`../app`](../app)) hoặc qua **Serial**.

> ⚠️ Chỉ dùng cho nghiên cứu/giáo dục trong môi trường được phép. Gây nhiễu sóng vô tuyến là bất hợp pháp ở hầu hết các nước.

Chạy **headless** (không cần màn OLED) trên phần cứng ESP32-S3.

## Phần cứng

**Board:** LilyGo T-Display-S3 (ESP32-S3, USB-C native — không có chip USB-UART riêng).

**Module nRF24L01+ (PA/LNA):** chân SPI cố định trong firmware; CE/CSN tự dò lúc boot.

| Pin nRF24 | GPIO T-Display-S3 |
|-----------|-------------------|
| SCK  | 12 |
| MISO | 13 |
| MOSI | 11 |
| CE   | 10 *(khuyến nghị)* |
| CSN  | 1  *(khuyến nghị)* |
| VCC  | 3V3 |
| GND  | GND |

> Nên gắn tụ ~100µF giữa VCC–GND sát module để nguồn ổn định khi phát công suất cao.

**Nút bấm (tùy chọn — dùng khi không điều khiển bằng app):**

| Nút | GPIO |
|-----|------|
| OK | 0 (BOOT onboard) |
| Next | 14 (KEY onboard) |
| Previous | 21 (header, nối xuống GND) |

Sơ đồ nối dây chi tiết: [`../docs/wiring.html`](../docs/wiring.html)

## Build & nạp

Cần [PlatformIO](https://platformio.org/) (CLI hoặc extension trong VS Code).

```bash
cd esp
pio run -e esp32-s3 -t upload
```

- File `.bin` nằm ở `.pio/build/esp32-s3/`.
- Xoá sạch cấu hình đã lưu (NVS): `pio run -e esp32-s3 -t erase`.

## Điều khiển qua Wi-Fi (HTTP API)

1. Bật board → nó phát Wi-Fi AP:
   - **SSID:** `nRF24-Tool`
   - **Mật khẩu:** `123456788`
2. Kết nối, mở `http://192.168.4.1`.
3. App React Native ([`../app`](../app)) gọi HTTP API để chọn chức năng, cấu hình và gửi lệnh.

Một số endpoint: `/api/status`, `/api/settings`, `/api/version`, `/WebCommand`, `/OTA`, và các route jam (`/bluetooth_jam`, `/drone_jam`, `/wifi_jam`, `/wifi_deauth_all`, `/wifi_array_spam`, `/ble_data_jam`, `/zigbee_jam`, `/misc_jam`, …).

Đổi SSID/mật khẩu mặc định: sửa `default_ssid` / `default_password` trong [`include/esp32-s3/config.h`](include/esp32-s3/config.h). Default chỉ áp dụng khi NVS chưa lưu — nếu đã lưu thì chạy lệnh `-t erase` ở trên trước.

## Điều khiển qua Serial

1. Cắm USB-C, mở serial monitor ở **115200 baud**.
2. Gõ `help` để xem danh sách lệnh. Ví dụ: `info`, `scan`, `jam wifi all`, `jam bt`, `jam ble data`, `set ap 0`, `stop`, `reboot`.

## Cấu trúc thư mục

| Đường dẫn | Nội dung |
|-----------|----------|
| `src/esp32-s3/` | Mã nguồn (`.cpp`) |
| `include/esp32-s3/` | Header (`config.h`, …) + `html.h` tự sinh lúc build |
| `html/` | Trang HTML nhúng (nén thành `html.h` khi build) |
| `lib/` | Thư viện vendor: `RF24`, `NimBLE-Arduino`, `GyverButton` |
| `scripts/` | Script build (sinh `html.h`) |

## Chức năng jam

Bluetooth · BLE (advertising / data) · Drone · Wi-Fi (jam / deauth / beacon spam) · Zigbee · Misc (dải kênh tuỳ chọn).

## Credit

Firmware sử dụng các thư viện mã nguồn mở: RF24, NimBLE-Arduino, GyverButton.
