# nRF24 Jammer — App điều khiển (React Native / Expo)

App điều khiển ESP32-S3 nRF24 jammer qua Wi-Fi (HTTP API của firmware). Giao diện **dark console** (hướng B).

## Yêu cầu
- Node.js 18+
- Expo Go trên điện thoại (Android/iOS), hoặc máy ảo

## Chạy
```bash
cd app
npm install
npm start
```
Quét QR bằng Expo Go. Điện thoại và máy tính cùng mạng khi phát triển; khi điều khiển thật thì **điện thoại nối Wi-Fi `jammer`** (AP của ESP).

## Kết nối ESP
- Mặc định firmware phát AP `jammer` tại `http://192.168.4.1`.
- Đổi địa chỉ trong **Cài đặt → Kết nối** nếu khác.

## Cấu trúc
```
app/
  App.tsx                 điều hướng (native-stack)
  src/theme.ts            màu / spacing (dark console)
  src/api/client.ts       gọi HTTP: /api/status, /WebCommand
  src/api/attacks.ts      định nghĩa loại tấn công -> lệnh firmware
  src/components/ui.tsx    Card, Button, Seg, Pill...
  src/screens/
    HubScreen.tsx         màn chính: trạng thái + danh sách tấn công
    AttackScreen.tsx      bắt đầu/dừng 1 tấn công, chọn method
    WifiScreen.tsx        quét AP, jam/deauth, beacon spam
    SettingsScreen.tsx    địa chỉ ESP, PA, jam delay, reboot
  _design/wireframes.html wireframe gốc (A & B) để tham chiếu
```

## Ánh xạ lệnh (gửi qua `/WebCommand?command=...`)
| Hành động | Lệnh |
|-----------|------|
| Bluetooth | `jam bt` |
| Drone | `jam drone` |
| BLE | `jam ble ad` / `jam ble data` |
| Zigbee | `jam zigbee` |
| Misc | `jam misc <start> <stop>` |
| WiFi | `jam wifi smart` / `jam wifi channel <n>` |
| Deauth | `attack wifi deauth smart` / `... channel <n>` |
| Beacon spam | `attack wifi spam random` / `... array` |
| Dừng | `stop` · Reboot: `reboot` · Quét WiFi: `scan wifi` |

> ⚠️ Lưu ý: firmware chạy vòng jam **blocking**, nên request HTTP khi bắt đầu jam có thể không trả về ngay — app coi là "đang chạy" và dùng `stop` để dừng. Nếu muốn trải nghiệm mượt hơn (phản hồi tức thì, trạng thái realtime) cần chỉnh firmware cho jam chạy ở task riêng.
