#ifndef CONFIG_H
#define CONFIG_H

#include "DNSServer.h"
#include "GyverButton.h"
#include "NimBLEDevice.h"
#include "Preferences.h"
#include "RF24.h"
#include "Update.h"
#include "WebServer.h"
#include "WiFi.h"
#include "esp_random.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"

// SSD1306 OLED over I2C. ESP32-S3-WROOM-1 DevKitC wiring: SDA=GPIO9, SCL=GPIO8.
// (Both GPIO8 and GPIO9 are clean, non-strapping GPIOs; usable as I2C as long as
// the OLED module provides the SDA/SCL pull-ups. The init below runs a
// bus-recovery + auto-swap pass, so SDA/SCL never have to be matched by hand.)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT_MAX 64
#define OLED_RESET -1
#define OLED_SDA 9
#define OLED_SCL 8
inline int SCREEN_HEIGHT;

// Onboard addressable RGB LED (WS2812) on GPIO38.
#define RGB_LED_PIN 38
inline void setRGB(uint8_t r, uint8_t g, uint8_t b)
{
  rgbLedWrite(RGB_LED_PIN, r, g, b);
}

#include <Adafruit_SSD1306.h>
#include <Wire.h>

// drawBitmap is called with decodeLZSS(...), a nullptr stub (the compressed
// logo/jam-icon assets are not bundled in this repo). Skip the draw when the
// pointer is null so the text UI still renders without the decorative images.
// Set true only once an SSD1306 is actually found on the I2C bus. While false,
// every I2C write to the panel is skipped so a missing/broken OLED does not
// flood the console with i2c_master_transmit ESP_ERR_INVALID_STATE each frame.
inline bool oledPresent = false;

class OledDisplay : public Adafruit_SSD1306
{
public:
  OledDisplay() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT_MAX, &Wire, OLED_RESET) {}
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
  {
    if ( bitmap )
      Adafruit_SSD1306::drawBitmap(x, y, bitmap, w, h, color);
  }
  // These two are the only calls that actually drive the I2C bus from the UI;
  // skip them when no panel is present to keep the serial console clean.
  void display()
  {
    if ( oledPresent )
      Adafruit_SSD1306::display();
  }
  void ssd1306_command(uint8_t c)
  {
    if ( oledPresent )
      Adafruit_SSD1306::ssd1306_command(c);
  }
};

// Bitmap assets are not bundled; decodeLZSS resolves to nullptr, which the
// OledDisplay::drawBitmap override above skips.
#define decodeLZSS(x) ((const uint8_t *)nullptr)

#define default_ssid "nRF24-Tool"
#define default_password "123456788"
#define Version_Number "V0.0.1"
#define Version_Name "esp32-s3"

// ESP32-S3 native USB uses a virtual baud rate.
#if ARDUINO_USB_CDC_ON_BOOT
#define SERIAL_UPDATE_BAUD(x) ((void)(x))
#else
#define SERIAL_UPDATE_BAUD(x) Serial.updateBaudRate(x)
#endif

inline String ssid;
inline String password;

inline int menu_number;
inline int baudrate;
inline int display_setting;
inline int logo;
inline int access_point;
inline int buttons;
inline int Separate_or_together;

inline int bluetooth_jam_method;
inline int drone_jam_method;
inline int wifi_jam_method;
inline int misc_jam_method;
inline int nrf_pa;

inline byte       bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
inline byte       ble_channels[]       = {2, 26, 80};
inline const char jam_text[]           = "xxxxxxxxxxxxxxxx";
inline int        jam_delay;

inline String BLE_MAC[100];
inline int    BLE_RSSI[100];
inline String BLE_Name[100];
inline String BLE_ManufacturerData[100];
inline String BLE_Payload[100];

inline int   nrf24_count;
inline int   ce_pins[30];
inline int   csn_pins[30];
inline RF24 *radios[30];

inline bool webServerStarted = false;
inline bool dnsServerStarted = false;

// Đặt true khi nhận lệnh "stop" qua web trong lúc đang jam, để vòng lặp jam
// (jamHandler) thoát ra. Cần cho app điều khiển qua Wi-Fi vì vòng lặp jam chặn loop().
inline volatile bool webStop = false;

inline char   SSIDs_Array[100][33];
inline String APs_array[14];
inline int    WiFiScanChannels[14];
inline int    NumberChannels;
inline char default_SSIDs_Array[100][33] = {
    "TP-Link_5G",
    "VNPT-9F2A",
    "Viettel_5G",
    "FPT_Telecom",
    "Home_WiFi",
    "iPhone cua Linh",
    "HUAWEI-5G-3F2A",
    "Phong Ngu 2",
    "DIRECT-7A-HP M283",
    "Tenda_5G_8821",
    "GigaNet_FPT",
    "Galaxy S23 Ultra",
    "ASUS_RT-AX1800U",
    "Xiaomi_AX3000_5G",
    "Linksys_2.4GHz",
    "VNPT_Fiber",
    "Viettel_Home",
    "NETGEAR58-5G",
    "dlink-2.4G-A1",
    "Phong Khach",
};

inline int  channel1 = 0;
inline int  channel2 = 0;
inline int  flag     = 0;
inline bool hspi     = false;

inline SPIClass        *hp = nullptr;
// ESP32-S3-DevKitC-1 buttons: OK=GPIO21, Next=GPIO47, Previous=GPIO48.
// Buttons are kept off GPIO19/GPIO20 (native USB D-/D+, USB-CDC on boot), so
// they can never work as normal GPIO buttons.
// (GPIO48 is the reference board's onboard RGB, but this board drives RGB on
// GPIO38, so GPIO48 is free here.)
inline GButton          btnOK(21);
inline GButton          btnNext(47);
inline GButton          btnPrevious(48);
inline WebServer        server(80);
inline DNSServer        dnsServer;
inline OledDisplay      display;
inline Preferences      prefs;

#endif