#include "web.h"
#include "display.h"
#include "serial.h"

static String jsonEscape(const String &s)
{
  String out;
  out.reserve(s.length() + 8);
  for ( size_t i = 0; i < s.length(); i++ )
  {
    char c = s[i];
    switch ( c )
    {
    case '\\':
      out += "\\\\";
      break;
    case '"':
      out += "\\\"";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    default:
      out += c;
      break;
    }
  }
  return out;
}

void handleRoot()
{
  server.send(200, "text/plain", "nRF24 Jammer - control via app (HTTP API).");
}

void HandleWebCommand()
{
  String command = server.arg("command");
  command.trim();
  command.toLowerCase();
  CommandsHandler(command, true);
}

void handlerSetSettings()
{
  baudrate             = server.arg("baudrate").toInt();
  logo                 = server.arg("logo").toInt();
  display_setting      = server.arg("display").toInt();
  SCREEN_HEIGHT        = server.arg("screen_height").toInt();
  buttons              = server.arg("button").toInt();
  Separate_or_together = server.arg("sweep").toInt();
  nrf_pa               = server.arg("pa").toInt();
  bluetooth_jam_method = server.arg("bt_method").toInt();
  drone_jam_method     = server.arg("drone_method").toInt();
  misc_jam_method      = server.arg("misc_method").toInt();
  ssid                 = server.arg("ssid");
  password             = server.arg("password");
  access_point         = server.arg("ap").toInt();
  jam_delay            = server.arg("delay").toInt();

  String beacon_list = server.arg("beacon");
  String ce_list     = server.arg("ce");
  String csn_list    = server.arg("csn");

  auto parseTokens = [](const String &str, auto callback, int maxCount) -> int {
    if ( str.length() == 0 )
      return 0;

    int count = 0;
    int start = 0;
    while ( count < maxCount )
    {
      int sep = str.indexOf('|', start);
      if ( sep == -1 )
        sep = str.length();

      String token = str.substring(start, sep);
      token.trim();

      if ( token.length() > 0 )
      {
        callback(token, count++);
      }

      if ( sep == str.length() )
        break;
      start = sep + 1;
    }
    return count;
  };

  nrf24_count = parseTokens(ce_list, [](const String &token, int idx) { ce_pins[idx] = token.toInt(); }, 30);

  parseTokens(csn_list, [](const String &token, int idx) { csn_pins[idx] = token.toInt(); }, 30);

  memset(SSIDs_Array, 0, sizeof(SSIDs_Array));
  parseTokens(beacon_list, [](const String &token, int idx) {
    String ssid_str = token.length() > 31 ? token.substring(0, 31) : token;
    strlcpy(SSIDs_Array[idx], ssid_str.c_str(), sizeof(SSIDs_Array[idx])); }, 100);

  prefs.putInt("baudrate", baudrate);
  prefs.putInt("logo_configs", logo);
  prefs.putInt("display_configs", display_setting);
  prefs.putInt("screen_height", SCREEN_HEIGHT);
  prefs.putInt("buttons_configs", buttons);
  prefs.putInt("SorT_configs", Separate_or_together);
  prefs.putInt("PA_configs", nrf_pa);
  prefs.putInt("bt_configs", bluetooth_jam_method);
  prefs.putInt("drone_configs", drone_jam_method);
  prefs.putInt("misc_configs", misc_jam_method);
  prefs.putInt("nRF24_count", nrf24_count);
  prefs.putString("ssid", ssid);
  prefs.putString("password", password);
  prefs.putInt("AP_configs", access_point);
  prefs.putInt("jam_delay", jam_delay);

  prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));
  prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));

  SERIAL_UPDATE_BAUD(baudrate);

  if ( SCREEN_HEIGHT == 32 )
  {
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x02);
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x1F);
  }
  else
  {
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x12);
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x3F);
  }

  server.send(200, "text/plain", "OK");
}

void RescanHandler()
{
  scan_wifi_APs(WiFiScanChannels, false);
  handleRoot();
}

void sendHtmlAndExecute(const uint8_t *htmlData, size_t len, void (*action)())
{
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)htmlData, len);
  delay(500);
  if ( action )
    action();
}

void attackHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap)
{
  (void)bitmap;
  uiTitle(htmlResponse, "Press a button to stop");
  sendHtmlAndExecute(html_jam, html_jam_len, attackFunction);
  updateDisplay(menu_number);
}

void attackScanHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap)
{
  (void)bitmap;
  uiTitle(htmlResponse, "Press a button to stop");
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)html_jam, html_jam_len);
  delay(500);
  if ( access_point == 0 )
    WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);
  delay(100);
  NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
  attackFunction();
  updateDisplay(menu_number);
}

void miscChannelsHandler()
{
  int channel1 = server.arg("start").toInt();
  int channel2 = server.arg("stop").toInt();

  sendHtmlAndExecute(html_jam, html_jam_len);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Start");
  display.setCursor(96, 0);
  display.println("Stop");
  display.setCursor(52, 0);
  display.println("Width");
  display.setCursor(8, 10);
  display.println(String(channel1));
  display.setCursor(60, 10);
  display.println(String(channel2 - channel1));
  display.setCursor(100, 10);
  display.println(String(channel2));
  display.setCursor(0, 20);
  display.println("Jamming Started");
  display.display();

  jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), nullptr, false, false, true, false, channel1, channel2);
  updateDisplay(menu_number);
}

void wifiChannelsHandler()
{
  int channel = server.arg("channel").toInt();

  uiTitle("WiFi Jam", "ch " + String(channel));
  sendHtmlAndExecute(html_jam, html_jam_len);

  jamHandler(wifi_channel, "Jamming WiFi channel", nullptr, false, false, false, false, channel - 1, 0);
  updateDisplay(menu_number);
}

void wifiDeauthChannelsHandler()
{
  int channel = server.arg("channel").toInt();

  uiTitle("WiFi Deauth", "ch " + String(channel));
  sendHtmlAndExecute(html_jam, html_jam_len);

  wifi_deauth_channel(channel);
  updateDisplay(menu_number);
}

void handleApiStatus()
{
  String json;
  json.reserve(1024);

  json += "{\"nrf24_count\":" + String(nrf24_count) + ",";

  String btText;
  switch ( bluetooth_jam_method )
  {
  case 0:
    btText = "21 channels";
    break;
  case 1:
  case 2:
    btText = "80 channels";
    break;
  default:
    btText = "-";
  }
  json += "\"bt_channels_text\":\"" + btText + "\",";
  json += "\"wifi_jam_method\":" + String(wifi_jam_method) + ",";

  json += "\"ssids_by_channel\":[";
  for ( int ch = 0; ch < 14; ch++ )
  {
    json += "\"" + jsonEscape(String(APs_array[ch])) + "\"";
    if ( ch < 13 )
      json += ",";
  }
  json += "],";

  json += "\"wifi_channels\":[";
  for ( int ch = 0; ch < 14; ch++ )
  {
    json += String(WiFiScanChannels[ch]);
    if ( ch < 13 )
      json += ",";
  }
  json += "]";

  json += "}";
  server.send(200, "application/json", json);
}

void handleApiSettings()
{
  String json;
  json.reserve(2048);

  json += "{\"version\":\"" + String(Version_Number) + "\",";
  json += "\"baudrate\":" + String(baudrate) + ",";
  json += "\"logo\":" + String(logo) + ",";
  json += "\"display\":" + String(display_setting) + ",";
  json += "\"screen_height\":" + String(SCREEN_HEIGHT) + ",";
  json += "\"button\":" + String(buttons) + ",";
  json += "\"sweep\":" + String(Separate_or_together) + ",";
  json += "\"pa\":" + String(nrf_pa) + ",";
  json += "\"bt\":" + String(bluetooth_jam_method) + ",";
  json += "\"drone\":" + String(drone_jam_method) + ",";
  json += "\"misc\":" + String(misc_jam_method) + ",";
  json += "\"delay\":" + String(jam_delay) + ",";
  json += "\"ap\":" + String(access_point) + ",";
  json += "\"nrf24_count\":" + String(nrf24_count) + ",";

  json += "\"ssid\":\"" + jsonEscape(ssid) + "\",";
  json += "\"password\":\"" + jsonEscape(password) + "\",";

  json += "\"beacon\":[";
  bool firstB = true;
  for ( int i = 0; i < 100; i++ )
  {
    if ( SSIDs_Array[i][0] == '\0' )
      break;
    if ( !firstB )
      json += ",";
    json += "\"" + jsonEscape(String(SSIDs_Array[i])) + "\"";
    firstB = false;
  }
  json += "],";

  json += "\"ce_pins\":[";
  for ( int i = 0; i < nrf24_count; i++ )
  {
    json += String(ce_pins[i]);
    if ( i < nrf24_count - 1 )
      json += ",";
  }
  json += "],";

  json += "\"csn_pins\":[";
  for ( int i = 0; i < nrf24_count; i++ )
  {
    json += String(csn_pins[i]);
    if ( i < nrf24_count - 1 )
      json += ",";
  }
  json += "]";

  json += "}";
  server.send(200, "application/json", json);
}

void handleApiVersion()
{
  String json = "{\"version\":\"" + String(Version_Number) + "\"}";
  server.send(200, "application/json", json);
}

void handleFileUpload()
{
  HTTPUpload          &upload         = server.upload();
  static unsigned long lastUpdate     = 0;
  const unsigned long  updateInterval = 10;

  if ( upload.status == UPLOAD_FILE_START )
  {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if ( !Update.begin(UPDATE_SIZE_UNKNOWN) )
    {
      Update.printError(Serial);
      return;
    }
    else
    {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      lastUpdate = millis();
    }
  }
  else if ( upload.status == UPLOAD_FILE_WRITE )
  {
    if ( Update.write(upload.buf, upload.currentSize) != upload.currentSize )
    {
      Update.printError(Serial);
    }

    if ( millis() - lastUpdate >= updateInterval )
    {
      lastUpdate = millis();
    }
  }
  else if ( upload.status == UPLOAD_FILE_END )
  {
  }
  else if ( upload.status == UPLOAD_FILE_ABORTED )
  {
    Update.abort();
    Serial.println("Update aborted");
  }
}

void startWebServer()
{
  if ( access_point != 0 )
    return;

  WiFi.mode(WIFI_AP);
  delay(100);

  if ( WiFi.softAP(ssid.c_str(), password.c_str()) )
  {
    IPAddress apIp = WiFi.softAPIP();
    if ( apIp != IPAddress((uint32_t)0) )
    {
      dnsServerStarted = dnsServer.start(53, "*", apIp);
    }
  }

  server.on("/", handleRoot);
  server.on("/bluetooth_jam", []() { jamHandler(bluetooth_jam, String("Bluetooth Jamming"), nullptr, true, false); });
  server.on("/drone_jam", []() { jamHandler(drone_jam, String("Drone Jamming"), nullptr, true, false); });
  server.on("/wifi_jam", []() { jamHandler(wifi_jam, String("WiFi Jamming"), nullptr, false, false); });
  server.on("/wifi_deauth_all", []() { attackHandler(String("WiFi Deauthing"), wifi_deauth_all, nullptr); });
  server.on("/wifi_scan_jam", []() { jamHandler(wifi_scan_jam, String("WiFi Jamming"), nullptr, false, false, false, true); });
  server.on("/wifi_deauth_scan", []() { attackScanHandler(String("WiFi Deauthing"), wifi_deauth_scan, nullptr); });
  server.on("/wifi_random_spam", []() { attackScanHandler(String("WiFi Beacon Spam"), wifi_beacon_spam_random, nullptr); });
  server.on("/wifi_array_spam", []() { attackScanHandler(String("WiFi Beacon Spam"), wifi_beacon_spam_array, nullptr); });
  server.on("/ble_advertising_jam", []() { jamHandler(ble_advertising_jam, String("BLE Jamming"), nullptr, false, false); });
  server.on("/ble_data_jam", []() { jamHandler(ble_data_jam, String("BLE Jamming"), nullptr, true, false); });
  server.on("/zigbee_jam", []() { jamHandler(zigbee_jam, String("Zigbee Jamming"), nullptr, false, false); });
  server.on("/misc_jam", miscChannelsHandler);
  server.on("/wifi_selected_jam", wifiChannelsHandler);
  server.on("/wifi_selected_deauth", wifiDeauthChannelsHandler);
  server.on("/rescan", RescanHandler);

  server.on("/api/status", handleApiStatus);
  server.on("/api/settings", handleApiSettings);
  server.on("/api/version", handleApiVersion);

  server.on("/OTA", []() {
    server.sendHeader("Content-Encoding", "gzip");
    server.send_P(200, "text/html", (const char *)html_ota, html_ota_len);
  });
  server.on("/WebCommand", HandleWebCommand);

  server.on("/generate_204", handleRoot);
  server.on("/redirect", handleRoot);
  server.on("/hotspot-detect.html", handleRoot);
  server.on("/canonical.html", handleRoot);
  server.on("/ncsi.txt", handleRoot);

  server.on(
      "/update", HTTP_POST, []() {
        if ( Update.end(true) )
        {
          Serial.printf("Update Success: %u bytes\n", Update.size());
          server.send(200, "text/plain", "Update Success");
          delay(100);
          uiTitle("Update OK", "Rebooting...");
          delay(2000);
          ESP.restart();
        }
        else
        {
          Update.printError(Serial);
          server.send(500, "text/plain", "Update Failed");
        }
      },
      handleFileUpload
  );

  server.begin();
  webServerStarted = true;
}
