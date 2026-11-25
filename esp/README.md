# Firmware nRF24 Jammer — ESP32-S3-WROOM-1

Firmware bộ công cụ nghiên cứu RF 2.4GHz, port cho **ESP32-S3-WROOM-1 (ESP32-S3)**, điều khiển qua **Wi-Fi AP + HTTP API** (dùng với app React Native ở [`../app`](../app)) hoặc qua **Serial**.

> ⚠️ Chỉ dùng cho nghiên cứu/giáo dục trong môi trường được phép. Gây nhiễu sóng vô tuyến là bất hợp pháp ở hầu hết các nước.

Hiển thị trạng thái qua màn **OLED SSD1306** (I2C) trên phần cứng ESP32-S3.

## Phần cứng

**Board:** ESP32-S3-WROOM-1 N8R8 (ESP32-S3-DevKitC-1, USB-C native). Tránh GPIO 35/36/37 (PSRAM octal), 26–32 (flash).

**3× module nRF24L01+ (PA/LNA):** dùng chung bus SPI (SCK/MOSI/MISO cố định trong firmware), mỗi module có CE/CSN riêng.

| Tín hiệu | GPIO |
|----------|------|
| SCK *(chung)*  | 12 |
| MOSI *(chung)* | 11 |
| MISO *(chung)* | 13 |
| #1 CE / CSN | 16 / 15 |
| #2 CE / CSN | 18 / 17 |
| #3 CE / CSN | 6 / 7 |
| VCC | 3V3 |
| GND | GND |

> Nên gắn tụ ~100µF giữa VCC–GND sát mỗi module để nguồn ổn định khi phát công suất cao. Ba module phát cùng lúc nên dùng nguồn 3V3 riêng ≥2A.

**Màn OLED SSD1306** (I²C, addr 0x3C): SDA=9, SCL/SCK=46.

**LED RGB:** WS2812 onboard ở GPIO38 (không cần đấu dây).

**Nút bấm:**

| Nút | GPIO |
|-----|------|
| OK | 21 |
| Next | 20 |
| Previous | 19 |

Sơ đồ nối dây chi tiết: [`../docs/wiring.html`](../docs/wiring.html) · schematic: [`../docs/schematic.html`](../docs/schematic.html)

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
