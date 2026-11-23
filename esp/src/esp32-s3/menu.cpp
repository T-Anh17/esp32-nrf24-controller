#include "menu.h"
#include "attack.h"
#include "display.h"
#include "jam.h"
#include "scan.h"
#include "storage.h"
#include "web.h"

// Hint line shown under a selectable screen, worded for the active button mode.
static const char *selectHint()
{
  return ( buttons == 0 ) ? "OKx1:next  hold:run" : "OK:run  Next:scroll";
}

void initButtons()
{
  btnOK.setTickMode(false);
  btnNext.setTickMode(false);
  btnPrevious.setTickMode(false);
  btnOK.setClickTimeout(120);
  btnNext.setClickTimeout(120);
  btnPrevious.setClickTimeout(120);
  btnOK.setTimeout(400);
  btnNext.setTimeout(400);
  btnPrevious.setTimeout(400);
}

void settings()
{
  int  current_item  = 0;
  int  scroll_offset = 0;
  bool in_edit_mode  = false;

  auto draw_settings = [&]() {
    int total_items = (buttons == 0) ? 13 : 12;
    if ( current_item >= total_items )
      current_item = total_items - 1;

    display.clearDisplay();

    int max_visible = (SCREEN_HEIGHT == 64) ? 6 : 3;
    int line_height = 10;
    int start_y     = 1;

    if ( current_item < scroll_offset )
    {
      scroll_offset = current_item;
    }
    else if ( current_item >= scroll_offset + max_visible )
    {
      scroll_offset = current_item - max_visible + 1;
    }

    for ( int i = 0; i < max_visible; i++ )
    {
      int item_idx = scroll_offset + i;
      if ( item_idx >= total_items )
        break;

      int y = start_y + i * line_height;

      if ( item_idx == current_item )
      {
        display.fillRect(0, y - 1, 122, line_height - 1, WHITE);
        display.setTextColor(BLACK, WHITE);
      }
      else
      {
        display.setTextColor(WHITE, BLACK);
      }

      display.setCursor(4, y);

      switch ( item_idx )
      {
      case 0:
        display.print("Logo: ");
        display.print(logo == 0 ? "Show" : "Hide");
        break;
      case 1:
        display.print("Display: ");
        display.print(display_setting == 0 ? "On" : "Off");
        break;
      case 2:
        display.print("Height: ");
        display.print(SCREEN_HEIGHT);
        break;
      case 3:
        display.print("Buttons: ");
        display.print(buttons == 0 ? "1 Btn" : (buttons == 1 ? "2 Btn" : "3 Btn"));
        break;
      case 4:
        display.print("Sweep: ");
        display.print(Separate_or_together == 0 ? "Sep" : "Together");
        break;
      case 5:
        display.print("PA Level: ");
        display.print(nrf_pa == 0 ? "MAX" : (nrf_pa == 1 ? "HIGH" : (nrf_pa == 2 ? "LOW" : "MIN")));
        break;
      case 6:
        display.print("BT Mode: ");
        display.print(bluetooth_jam_method == 0 ? "21ch" : (bluetooth_jam_method == 1 ? "80ch rand" : "80ch brute"));
        break;
      case 7:
        display.print("Drone: ");
        display.print(drone_jam_method == 0 ? "Random" : "Brute");
        break;
      case 8:
        display.print("Misc: ");
        display.print(misc_jam_method == 0 ? "CW" : "Packet");
        break;
      case 9:
        display.print("AP Mode: ");
        display.print(access_point == 0 ? "On" : "Off");
        break;
      case 10:
        display.print("Baudrate: ");
        display.print(baudrate);
        break;
      case 11:
        display.print("Jam Delay: ");
        if ( in_edit_mode && current_item == 11 )
        {
          display.print("<");
          display.print(jam_delay);
          display.print("ms>");
        }
        else
        {
          display.print(jam_delay);
          display.print("ms");
        }
        break;
      case 12:
        display.print(" [ Save ]");
        break;
      }
    }

    display.drawFastVLine(125 + 1, 0, SCREEN_HEIGHT, WHITE);
    int thumb_h = (SCREEN_HEIGHT * max_visible) / total_items;
    if ( thumb_h < 4 )
      thumb_h = 4;
    int thumb_y = map(current_item, 0, total_items - 1, 0, SCREEN_HEIGHT - thumb_h);
    display.fillRect(125, thumb_y, 3, thumb_h, WHITE);

    display.display();
  };

  auto apply_and_save_item = [&](int item_idx, int delta) {
    switch ( item_idx )
    {
    case 0:
      logo = (logo + 1) % 2;
      prefs.putInt("logo_configs", logo);
      break;
    case 1:
      display_setting = (display_setting + 1) % 2;
      prefs.putInt("display_configs", display_setting);
      if ( display_setting )
      {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
      }
      else
      {
        display.ssd1306_command(SSD1306_DISPLAYON);
      }
      break;
    case 2:
      SCREEN_HEIGHT = (SCREEN_HEIGHT == 64) ? 32 : 64;
      prefs.putInt("screen_height", SCREEN_HEIGHT);
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
      break;
    case 3:
      buttons = (buttons + 1) % 3;
      prefs.putInt("buttons_configs", buttons);
      break;
    case 4:
      Separate_or_together = (Separate_or_together + 1) % 2;
      prefs.putInt("SorT_configs", Separate_or_together);
      break;
    case 5:
      nrf_pa = (nrf_pa + 1) % 4;
      prefs.putInt("PA_configs", nrf_pa);
      break;
    case 6:
      bluetooth_jam_method = (bluetooth_jam_method + 1) % 3;
      prefs.putInt("bt_configs", bluetooth_jam_method);
      break;
    case 7:
      drone_jam_method = (drone_jam_method + 1) % 2;
      prefs.putInt("drone_configs", drone_jam_method);
      break;
    case 8:
      misc_jam_method = (misc_jam_method + 1) % 2;
      prefs.putInt("misc_configs", misc_jam_method);
      break;
    case 9:
      access_point = (access_point + 1) % 2;
      prefs.putInt("AP_configs", access_point);
      break;
    case 10: {
      const uint32_t baudrate_list[] = {9600, 19200, 38400, 57600, 115200, 921600};
      const int      baudrate_count  = sizeof(baudrate_list) / sizeof(baudrate_list[0]);
      int            idx             = 0;
      for ( int i = 0; i < baudrate_count; i++ )
      {
        if ( baudrate_list[i] == (uint32_t)baudrate )
        {
          idx = i;
          break;
        }
      }
      idx      = (idx + delta + baudrate_count) % baudrate_count;
      baudrate = baudrate_list[idx];
      prefs.putInt("baudrate", baudrate);
      SERIAL_UPDATE_BAUD(baudrate);
      break;
    }
    case 11:
      jam_delay += delta * 10;
      if ( jam_delay > 10000 )
        jam_delay = 0;
      if ( jam_delay < 0 )
        jam_delay = 10000;
      prefs.putInt("jam_delay", jam_delay);
      break;
    }
  };

  draw_settings();

  while ( true )
  {
    int total_items = (buttons == 0) ? 13 : 12;

    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();

    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() )
      {
        current_item = (current_item + 1) % total_items;
        draw_settings();
      }
      else if ( btnOK.isHolded() )
      {
        if ( current_item == 12 )
        {
          break;
        }
        else if ( current_item == 11 )
        {
        }
        else
        {
          apply_and_save_item(current_item, 1);
          draw_settings();
        }
      }
    }

    else if ( buttons == 1 )
    {
      if ( in_edit_mode )
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          apply_and_save_item(11, 1);
          draw_settings();
        }
        else if ( btnOK.isSingle() || btnOK.isHolded() )
        {
          in_edit_mode = false;
          draw_settings();
        }
      }
      else
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          current_item = (current_item + 1) % total_items;
          draw_settings();
        }
        else if ( btnOK.isSingle() )
        {
          if ( current_item == 11 )
          {
            in_edit_mode = true;
            draw_settings();
          }
          else
          {
            apply_and_save_item(current_item, 1);
            draw_settings();
          }
        }
        else if ( btnOK.isHolded() )
        {
          break;
        }
      }
    }

    else if ( buttons == 2 )
    {
      if ( in_edit_mode )
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          apply_and_save_item(11, 1);
          draw_settings();
        }
        else if ( btnPrevious.isSingle() || btnPrevious.isHold() )
        {
          if ( btnPrevious.isHold() )
            delay(100);
          apply_and_save_item(11, -1);
          draw_settings();
        }
        else if ( btnOK.isSingle() || btnOK.isHolded() )
        {
          in_edit_mode = false;
          draw_settings();
        }
      }
      else
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          current_item = (current_item + 1) % total_items;
          draw_settings();
        }
        else if ( btnPrevious.isSingle() || btnPrevious.isHold() )
        {
          if ( btnPrevious.isHold() )
            delay(100);
          current_item = (current_item - 1 + total_items) % total_items;
          draw_settings();
        }
        else if ( btnOK.isSingle() )
        {
          if ( current_item == 11 )
          {
            in_edit_mode = true;
            draw_settings();
          }
          else
          {
            apply_and_save_item(current_item, 1);
            draw_settings();
          }
        }
        else if ( btnOK.isHolded() )
        {
          break;
        }
      }
    }
  }

  display.setTextColor(WHITE, BLACK);
  updateDisplay(menu_number);
}

void misc()
{
  int  flag         = 0;
  auto display_info = [&](String info) {
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
    display.println(info);
    display.display();
  };

  display_info("");

  auto incrementChannel = [&](int &channel) {
    channel++;
    if ( channel > 125 )
    {
      channel = 0;
    }
  };
  auto reduceChannel = [&](int &channel) {
    channel--;
    if ( channel < 0 )
    {
      channel = 125;
    }
  };

  while ( true )
  {
    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();
    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() || btnOK.isHold() )
      {
        if ( btnOK.isHold() )
        {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      else if ( btnOK.isDouble() )
      {
        if ( flag == 0 )
        {
          flag = 1;
        }
        else
        {
          if ( channel1 > channel2 )
          {
            display_info("Error: Second < First");
            flag = 0;
          }
          else
          {
            display_info("Jamming Started");
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), nullptr, false, false, true, false, channel1, channel2);
            break;
          }
        }
      }
    }
    if ( buttons == 1 )
    {
      if ( btnNext.isSingle() || btnNext.isHold() )
      {
        if ( btnNext.isHold() )
        {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      else if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          flag = 1;
        }
        else
        {
          if ( channel1 > channel2 )
          {
            display_info("Error: Second < First");
            flag = 0;
          }
          else
          {
            display_info("Jamming Started");
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), nullptr, false, false, true, false, channel1, channel2);
            break;
          }
        }
      }
    }
    if ( buttons == 2 )
    {
      if ( btnNext.isSingle() || btnNext.isHold() )
      {
        if ( btnNext.isHold() )
        {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      if ( btnPrevious.isSingle() || btnPrevious.isHold() )
      {
        if ( btnPrevious.isHold() )
        {
          delay(100);
        }
        reduceChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      else if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          flag = 1;
        }
        else
        {
          if ( channel1 > channel2 )
          {
            display_info("Error: Second < First");
            flag = 0;
          }
          else
          {
            display_info("Jamming Started");
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), nullptr, false, false, true, false, channel1, channel2);
            break;
          }
        }
      }
    }
  }
}

void ble_select()
{
  auto display_info = [&](int flag) -> void {
    uiTitle(flag == 0 ? "BLE: Advertising" : "BLE: Data", selectHint());
  };

  auto display_jam = [&](bool advertising) -> void {
    if ( advertising )
    {
      jamHandler(ble_advertising_jam, String("BLE AD Jam"), nullptr, false, true);
    }
    else
    {
      jamHandler(ble_data_jam, String("BLE Data Jam"), nullptr, true, false);
    }
  };

  int flag = 0;

  display_info(flag);

  while ( true )
  {
    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();

    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() )
      {
        flag = (flag + 1) % 2;
        display_info(flag);
      }
      if ( btnOK.isHolded() )
      {
        if ( flag == 0 )
        {
          display_jam(true);
          break;
        }
        if ( flag == 1 )
        {
          display_jam(false);
          break;
        }
      }
    }
    else if ( buttons == 1 )
    {
      if ( btnNext.isSingle() )
      {
        flag = (flag + 1) % 2;
        display_info(flag);
      }
      if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          display_jam(true);
          break;
        }
        if ( flag == 1 )
        {
          display_jam(false);
          break;
        }
      }
    }
    else if ( buttons == 2 )
    {
      if ( btnNext.isSingle() )
      {
        flag = (flag + 1) % 2;
        display_info(flag);
      }
      if ( btnPrevious.isSingle() )
      {
        flag = (flag - 1 + 2) % 2;
        display_info(flag);
      }
      if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          display_jam(true);
          break;
        }
        if ( flag == 1 )
        {
          display_jam(false);
          break;
        }
      }
    }
  }
}

void wifi_select()
{
  // Top-level WiFi attack picker labels (replaces the per-item icons).
  auto draw_top = [&](int m) -> void {
    const char *name = (m == 0)   ? "WiFi: All APs"
                       : (m == 1) ? "WiFi: Select AP"
                       : (m == 2) ? "Smart Attack"
                                  : "Beacon Spam";
    uiTitle(name, selectHint());
  };

  // Beacon-spam source picker labels.
  auto draw_beacon = [&](int m) -> void {
    uiTitle(m == 0 ? "Beacon: List SSIDs" : "Beacon: Random", selectHint());
  };

  auto display_info = [&](int flag, int wifi_points, int scroll_offset) -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("channel: " + String(flag));
    display.setCursor(0, 10);
    display.println("Wi-Fi APs: " + String(wifi_points));
    String apList   = APs_array[flag - 1];
    int    startIdx = 0;
    for ( int i = 0; i < scroll_offset; i++ )
    {
      int n = apList.indexOf('\n', startIdx);
      if ( n == -1 )
        break;
      startIdx = n + 1;
    }
    int maxLines;
    if ( SCREEN_HEIGHT == 32 )
    {
      maxLines = 2;
    }
    else
    {
      maxLines = 5;
    }

    int y = 20;
    for ( int i = 0; i < maxLines; i++ )
    {
      int    endIdx = apList.indexOf('\n', startIdx);
      String line;
      if ( endIdx == -1 )
      {
        line = apList.substring(startIdx);
      }
      else
      {
        line = apList.substring(startIdx, endIdx);
      }
      display.setCursor(0, y);
      display.println(line);
      y += 8;
      if ( endIdx == -1 )
        break;
      startIdx = endIdx + 1;
    }
    display.display();
  };

  auto scan_wifi = [&](int &channelCount, int *WiFi_channels) -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi APs");
    display.display();
    if ( access_point == 0 )
      WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    delay(100);
    int networks = scan_wifi_APs(WiFiScanChannels, false);
    display.setCursor(0, 10);
    display.println("Finded " + String(networks) + " APs");
    display.display();
    if ( access_point == 0 )
      WiFi.softAP(ssid.c_str(), password.c_str());
    delay(1000);
  };

  auto scan_wifi_channels = [&]() -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi ch's");
    display.display();
    if ( access_point == 0 )
      WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    delay(100);
    NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
    display.setCursor(0, 10);
    display.println("Finded " + String(NumberChannels) + " active ch's");
    display.display();
    if ( access_point == 0 )
      WiFi.softAP(ssid.c_str(), password.c_str());
    delay(1000);
  };

  auto count_lines = [](const String &s) -> int {
    int cnt = 1;
    for ( unsigned int i = 0; i < s.length(); i++ )
    {
      if ( s[i] == '\n' )
        cnt++;
    }
    return cnt;
  };

  int channelCount = 0;
  int menu_number  = 0;

  while ( true )
  {
    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();

    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() )
      {
        menu_number = (menu_number + 1) % 4;
        draw_top(menu_number);
      }
      if ( btnOK.isHolded() )
      {
        if ( menu_number == 0 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_jam, String("WiFi Jam"), nullptr, false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, nullptr);
          }
          return;
        }
        else if ( menu_number == 1 )
        {
          scan_wifi(channelCount, WiFiScanChannels);

          int           flag          = 1;
          int           scroll_offset = 0;
          unsigned long last_scroll   = 0;
          int           total_lines   = count_lines(APs_array[flag - 1]);

          display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);

          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();

            if ( btnOK.isSingle() )
            {
              flag++;
              if ( flag > 14 )
                flag = 1;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( total_lines > ((SCREEN_HEIGHT == 32) ? 2 : 5) && millis() - last_scroll >= 1000 )
            {
              scroll_offset++;
              int maxLines = (SCREEN_HEIGHT == 32) ? 2 : 5;
              if ( scroll_offset + maxLines > total_lines )
              {
                scroll_offset = 0;
              }
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnOK.isHolded() )
            {
              btnNext.tick();
              btnOK.tick();
              btnPrevious.tick();
              while ( !btnOK.isSingle() )
              {
                bool exit = jamHandler(wifi_channel, "Jamming WiFi channel", nullptr, false, true, false, false, flag - 1, 0);
                if ( btnOK.isSingle() || exit )
                  break;
                uiTitle("WiFi Deauth", "ch " + String(flag));
                wifi_deauth_channel(flag);
              }
              return;
            }
          }
        }
        else if ( menu_number == 2 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          scan_wifi_channels();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_scan_jam, String("WiFi Jam"), nullptr, false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, nullptr);
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          draw_beacon(menu_number);
          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();
            if ( btnOK.isSingle() )
            {
              menu_number = (menu_number + 1) % 2;
              draw_beacon(menu_number);
            }
            if ( btnOK.isHolded() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, nullptr);
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, nullptr);
                break;
              }
              break;
            }
          }
          return;
        }
      }
    }

    if ( buttons == 1 )
    {
      if ( btnNext.isSingle() )
      {
        menu_number = (menu_number + 1) % 4;
        draw_top(menu_number);
      }
      if ( btnOK.isSingle() )
      {
        if ( menu_number == 0 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_jam, String("WiFi Jam"), nullptr, false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, nullptr);
          }
          return;
        }
        else if ( menu_number == 1 )
        {
          scan_wifi(channelCount, WiFiScanChannels);

          int           flag          = 1;
          int           scroll_offset = 0;
          unsigned long last_scroll   = 0;
          int           total_lines   = count_lines(APs_array[flag - 1]);

          display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);

          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();

            if ( btnNext.isSingle() )
            {
              flag++;
              if ( flag > 14 )
                flag = 1;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( total_lines > ((SCREEN_HEIGHT == 32) ? 2 : 5) && millis() - last_scroll >= 1000 )
            {
              scroll_offset++;
              int maxLines = (SCREEN_HEIGHT == 32) ? 2 : 5;
              if ( scroll_offset + maxLines > total_lines )
              {
                scroll_offset = 0;
              }
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnOK.isSingle() )
            {
              btnNext.tick();
              btnOK.tick();
              btnPrevious.tick();
              while ( !btnOK.isSingle() )
              {
                bool exit = jamHandler(wifi_channel, "Jamming WiFi channel", nullptr, false, true, false, false, flag - 1, 0);
                if ( btnOK.isSingle() || exit )
                  break;
                uiTitle("WiFi Deauth", "ch " + String(flag));
                wifi_deauth_channel(flag);
              }
              return;
            }
          }
        }
        else if ( menu_number == 2 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          scan_wifi_channels();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_scan_jam, String("WiFi Jam"), nullptr, false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, nullptr);
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          draw_beacon(menu_number);
          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();
            if ( btnNext.isSingle() )
            {
              menu_number = (menu_number + 1) % 2;
              draw_beacon(menu_number);
            }
            if ( btnOK.isSingle() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, nullptr);
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, nullptr);
                break;
              }
              break;
            }
          }
          return;
        }
      }
    }

    if ( buttons == 2 )
    {
      if ( btnNext.isSingle() )
      {
        menu_number = (menu_number + 1) % 4;
        draw_top(menu_number);
      }
      if ( btnPrevious.isSingle() )
      {
        menu_number = (menu_number - 1 + 4) % 4;
        draw_top(menu_number);
      }

      if ( btnOK.isSingle() )
      {
        if ( menu_number == 0 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_jam, String("WiFi Jam"), nullptr, false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, nullptr);
          }
          return;
        }
        else if ( menu_number == 1 )
        {
          scan_wifi(channelCount, WiFiScanChannels);

          int           flag          = 1;
          int           scroll_offset = 0;
          unsigned long last_scroll   = 0;
          int           total_lines   = count_lines(APs_array[flag - 1]);

          display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);

          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();

            if ( btnNext.isSingle() )
            {
              flag++;
              if ( flag > 14 )
                flag = 1;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnPrevious.isSingle() )
            {
              flag--;
              if ( flag < 1 )
                flag = 14;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( total_lines > ((SCREEN_HEIGHT == 32) ? 2 : 5) && millis() - last_scroll >= 1000 )
            {
              scroll_offset++;
              int maxLines = (SCREEN_HEIGHT == 32) ? 2 : 5;
              if ( scroll_offset + maxLines > total_lines )
              {
                scroll_offset = 0;
              }
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnOK.isSingle() )
            {
              btnNext.tick();
              btnOK.tick();
              btnPrevious.tick();
              while ( !btnOK.isSingle() )
              {
                bool exit = jamHandler(wifi_channel, "Jamming WiFi channel", nullptr, false, true, false, false, flag - 1, 0);
                if ( btnOK.isSingle() || exit )
                  break;
                uiTitle("WiFi Deauth", "ch " + String(flag));
                wifi_deauth_channel(flag);
              }
              return;
            }
          }
        }
        else if ( menu_number == 2 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          scan_wifi_channels();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_scan_jam, String("WiFi Jam"), nullptr, false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, nullptr);
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          draw_beacon(menu_number);
          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();
            if ( btnNext.isSingle() )
            {
              menu_number = (menu_number + 1) % 2;
              draw_beacon(menu_number);
            }
            if ( btnPrevious.isSingle() )
            {
              menu_number = (menu_number - 1 + 2) % 2;
              draw_beacon(menu_number);
            }
            if ( btnOK.isSingle() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, nullptr);
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, nullptr);
                break;
              }
              break;
            }
          }
          return;
        }
      }
    }
  }
}

void executeAction(int menuNum)
{
  if ( nrf24_count <= 0 )
  {
    uiTitle("nRF24 not configured", "Set it in the web UI");
    while ( nrf24_count <= 0 )
    {
      if ( webServerStarted )
      {
        server.handleClient();
      }
      if ( dnsServerStarted )
      {
        dnsServer.processNextRequest();
      }
      delay(100);
    }
  }
  if ( menuNum == 5 )
  {
    misc();
    updateDisplay(menu_number);
    return;
  }
  if ( menuNum == 3 )
  {
    ble_select();
    updateDisplay(menu_number);
    return;
  }

  switch ( menu_number )
  {
  case 0:
    jamHandler(bluetooth_jam, String("Bluetooth Jamming"), nullptr, true, false);
    break;
  case 1:
    jamHandler(drone_jam, String("Drone Jamming"), nullptr, true, false);
    break;
  case 2:
    wifi_select();
    break;
  case 4:
    jamHandler(zigbee_jam, String("Zigbee Jamming"), nullptr, false, false);
    break;
  case 6:
    settings();
    break;
  case 7:
    prefs.putInt("AP_configs", 0);
    break;
  default:
    break;
  }
  if ( menuNum != 6 )
    updateDisplay(menuNum);
}
