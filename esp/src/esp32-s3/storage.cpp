#include "storage.h"
#include "html.h"

void loadSettings()
{
  bluetooth_jam_method = prefs.getInt("bt_configs", 0);
  drone_jam_method     = prefs.getInt("drone_configs", 1);
  display_setting      = prefs.getInt("display_configs", 0);
  wifi_jam_method      = prefs.getInt("wifi_configs", 0);
  nrf_pa               = prefs.getInt("PA_configs", 0);
  misc_jam_method      = prefs.getInt("misc_configs", 0);
  logo                 = prefs.getInt("logo_configs", 0);
  access_point         = prefs.getInt("AP_configs", 0);
  buttons              = prefs.getInt("buttons_configs", 2);
  Separate_or_together = prefs.getInt("SorT_configs", 0);
  jam_delay            = prefs.getInt("jam_delay", 0);
  SCREEN_HEIGHT        = prefs.getInt("screen_height", 64);
  baudrate             = prefs.getInt("baudrate", 115200);

  SERIAL_UPDATE_BAUD(baudrate);

  if ( !prefs.isKey("SSIDs_Array") )
  {
    memset(SSIDs_Array, 0, sizeof(SSIDs_Array));
    for ( int i = 0; i < sizeof(default_SSIDs_Array) / sizeof(default_SSIDs_Array[0]) && i < 100; i++ )
    {
      if ( default_SSIDs_Array[i] == nullptr )
        break;
      strlcpy(SSIDs_Array[i], default_SSIDs_Array[i], 33);
    }
    prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));
  }
  else
  {
    prefs.getBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));

    if ( SSIDs_Array[0][0] == '\0' )
    {
      memset(SSIDs_Array, 0, sizeof(SSIDs_Array));
      for ( int i = 0; i < sizeof(default_SSIDs_Array) / sizeof(default_SSIDs_Array[0]) && i < 100; i++ )
      {
        if ( default_SSIDs_Array[i] == nullptr )
          break;
        strlcpy(SSIDs_Array[i], default_SSIDs_Array[i], 33);
      }
      prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));
    }
  }

  if ( access_point == 0 )
  {
    ssid     = prefs.getString("ssid", default_ssid);
    password = prefs.getString("password", default_password);
  }
}

void storeEEPROMAndSet(const char *index, int value, int &targetVar)
{
  prefs.putInt(index, value);
  targetVar = value;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void saveWiFiSettings(const char *new_ssid, const char *new_password)
{
  prefs.putString("ssid", new_ssid);
  prefs.putString("password", new_password);
}

void handleResetWiFiSettings()
{
  saveWiFiSettings(default_ssid, default_password);

  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)html_pls_reboot, html_pls_reboot_len);
  delay(1000);
  ESP.restart();
}
