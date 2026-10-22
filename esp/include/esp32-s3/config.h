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

// No-op display shim for headless builds.
inline constexpr int BLACK = 0, WHITE = 1, INVERSE = 2;
inline constexpr int SSD1306_SWITCHCAPVCC = 0x02;
inline constexpr int SSD1306_DISPLAYOFF = 0xAE, SSD1306_DISPLAYON = 0xAF;
inline constexpr int SSD1306_SETCOMPINS = 0xDA, SSD1306_SETMULTIPLEX = 0xA8;

struct NullDisplay
{
  template <class... A> bool   begin(A...) { return true; }
  void                         clearDisplay() {}
  void                         display() {}
  template <class... A> void   drawBitmap(A...) {}
  template <class... A> void   drawFastVLine(A...) {}
  template <class... A> void   fillRect(A...) {}
  template <class... A> size_t print(A...) { return 0; }
  template <class... A> size_t println(A...) { return 0; }
  template <class... A> void   setCursor(A...) {}
  template <class... A> void   setTextColor(A...) {}
  template <class... A> void   setTextSize(A...) {}
  template <class... A> void   ssd1306_command(A...) {}
};

// Display bitmaps are not compiled in headless builds.
#define decodeLZSS(x) ((const uint8_t *)nullptr)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT_MAX 64
inline int SCREEN_HEIGHT;
#define OLED_RESET -1

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
// T-Display-S3 buttons: OK=GPIO0, Next=GPIO14, Previous=GPIO21.
inline GButton          btnOK(0);
inline GButton          btnNext(14);
inline GButton          btnPrevious(21);
inline WebServer        server(80);
inline DNSServer        dnsServer;
inline NullDisplay      display;
inline Preferences      prefs;

#endif