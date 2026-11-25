#include "display.h"
#include "scan.h"
#include <Wire.h>

// Home-menu entry labels, indexed by menu_number (0..6). These replace the
// per-entry icons the stubbed-out bitmap assets used to draw.
static const char *const MENU_NAMES[7] = {
    "Bluetooth Jam",
    "Drone Jam",
    "WiFi Attacks",
    "BLE Jam",
    "Zigbee Jam",
    "Misc Jam",
    "Settings",
};

void uiTitle(const String &title, const String &hint)
{
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  // size 1 here: jam/attack/select titles are long and variable, so size 2
  // wraps them into an ugly block. Size 2 is kept only for the home menu.
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(title);
  if ( hint.length() > 0 )
  {
    display.setTextSize(1);
    display.setCursor(0, (SCREEN_HEIGHT >= 64) ? 56 : 24);
    display.print(hint);
  }
  display.display();
}

void updateDisplay(int menuNum)
{
  (void)menuNum;
  int idx = ( menu_number >= 0 && menu_number < 7 ) ? menu_number : 0;

  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Menu ");
  display.print(idx + 1);
  display.print("/7");

  display.setTextSize(2);
  display.setCursor(0, (SCREEN_HEIGHT >= 64) ? 22 : 12);
  display.println(MENU_NAMES[idx]);

  display.setTextSize(1);
  display.setCursor(0, (SCREEN_HEIGHT >= 64) ? 56 : 24);
  display.print(( buttons == 0 ) ? "OKx1:next hold:run" : "OK:run Next/Prev:move");

  display.display();
}

void initOLED()
{
  // Report the idle state of the two lines with internal pull-ups engaged. Both
  // should read 1 (high). A 0 means that line is held low (short to GND, wrong
  // pin, or a slave stuck on the bus) -> every I2C transfer returns
  // ESP_ERR_INVALID_STATE and nothing on the bus can work.
  pinMode(OLED_SDA, INPUT_PULLUP);
  pinMode(OLED_SCL, INPUT_PULLUP);
  delay(5);
  int sdaLvl = digitalRead(OLED_SDA);
  int sclLvl = digitalRead(OLED_SCL);
  Serial.printf("I2C idle: SDA(GPIO%d)=%d SCL(GPIO%d)=%d  (1=OK, 0=STUCK LOW)\n",
                OLED_SDA, sdaLvl, OLED_SCL, sclLvl);

  // Bus recovery: if SDA is stuck low, a slave may be mid-transfer. Clock SCL up
  // to 9 times so it finishes/releases SDA, then send a STOP.
  if ( sdaLvl == LOW && sclLvl == HIGH )
  {
    Serial.println("SDA stuck low: attempting I2C bus recovery...");
    pinMode(OLED_SCL, OUTPUT);
    for ( int i = 0; i < 9 && digitalRead(OLED_SDA) == LOW; i++ )
    {
      digitalWrite(OLED_SCL, LOW);
      delayMicroseconds(5);
      digitalWrite(OLED_SCL, HIGH);
      delayMicroseconds(5);
    }
    pinMode(OLED_SDA, OUTPUT);
    digitalWrite(OLED_SDA, LOW);
    delayMicroseconds(5);
    digitalWrite(OLED_SCL, HIGH);
    delayMicroseconds(5);
    digitalWrite(OLED_SDA, HIGH); // SDA low->high while SCL high = STOP
    delayMicroseconds(5);
    Serial.printf("After recovery: SDA=%d\n", digitalRead(OLED_SDA));
  }

  // Scan the bus for the OLED. With native USB-CDC the whole boot log is printed
  // before the serial monitor attaches, so a single scan at boot is usually
  // missed and the result can never be seen. Retry for a few seconds, printing
  // every pass, so: (1) the scan is visible whenever the monitor is opened,
  // (2) a slow-powering / loose panel still gets picked up on a later pass.
  // Both SDA/SCL orientations are tried each pass, so the wiring is never guessed.
  auto scanBus = [](int sda, int scl) -> int {
    Wire.end();
    delay(5);
    Wire.begin(sda, scl);
    Wire.setClock(100000); // 100 kHz: tolerant of long/loose jumper wiring
    delay(5);
    for ( uint8_t addr = 1; addr < 127; addr++ )
    {
      Wire.beginTransmission(addr);
      if ( Wire.endTransmission() == 0 )
        return addr;
    }
    return -1;
  };

  int       useSda = OLED_SDA, useScl = OLED_SCL;
  int       foundAddr = -1;
  const int kScanTries = 20; // ~14 s worst case; stops the instant a panel answers
  for ( int attempt = 1; attempt <= kScanTries && foundAddr < 0; attempt++ )
  {
    Serial.printf("[I2C %2d/%d] SDA=GPIO%d SCL=GPIO%d ... ", attempt, kScanTries, OLED_SDA, OLED_SCL);
    foundAddr = scanBus(OLED_SDA, OLED_SCL);
    if ( foundAddr >= 0 )
    {
      useSda = OLED_SDA;
      useScl = OLED_SCL;
      Serial.printf("device at 0x%02X\n", foundAddr);
      break;
    }
    Serial.printf("none; swapped SDA=GPIO%d SCL=GPIO%d ... ", OLED_SCL, OLED_SDA);
    foundAddr = scanBus(OLED_SCL, OLED_SDA);
    if ( foundAddr >= 0 )
    {
      useSda = OLED_SCL;
      useScl = OLED_SDA;
      Serial.printf("device at 0x%02X (wires SWAPPED vs config)\n", foundAddr);
      break;
    }
    Serial.println("nothing either orientation; retrying...");
    delay(700);
  }

  if ( foundAddr >= 0 )
    Serial.printf(">>> OLED FOUND at 0x%02X  SDA=GPIO%d SCL=GPIO%d\n", foundAddr, useSda, useScl);
  else
    Serial.println(">>> OLED NOT FOUND after retries. Check: 3V3 + GND to the panel, "
                   "SDA/SCL continuity and common ground. (nRF24 SPI is on 11/12/13, "
                   "CE/CSN on 16/15/18/17/6/7 -- none clash with I2C 9/8.)");

  oledPresent = ( foundAddr >= 0 );

  // ALWAYS call begin() -- even when no panel was found -- because begin() is
  // what allocates the frame buffer. clearDisplay()/print()/drawBitmap() all
  // write into that buffer, so skipping begin() leaves it NULL and the first
  // clearDisplay() does memset(NULL,...) => StoreProhibited crash. When the
  // panel is absent, oledPresent stays false so the display()/ssd1306_command()
  // overrides skip the actual I2C; only begin()'s one-time init blips the bus.
  Wire.end();
  delay(5);
  Wire.begin(useSda, useScl);
  Wire.setClock(100000);
  display.begin(SSD1306_SWITCHCAPVCC, ( foundAddr >= 0 ) ? foundAddr : 0x3C, true, false);

  if ( !oledPresent )
  {
    Serial.println("OLED absent -> display output skipped (RGB = red).");
    setRGB(30, 0, 0);
  }

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

  // Splash screen: proves the panel + I2C wiring work without any bitmap asset.
  // Centered on both axes, large text. (The default GFX font is ASCII-only, so
  // the name is written without Vietnamese diacritics.)
  display.clearDisplay();
  display.setTextColor(WHITE);
  {
    const char *l1  = "Tran Anh";
    const char *l2  = "DIY";
    int         s1  = (SCREEN_HEIGHT >= 64) ? 2 : 1;
    int         s2  = (SCREEN_HEIGHT >= 64) ? 3 : 2;
    const int   gap = 2;
    int         h1  = 8 * s1;
    int         h2  = 8 * s2;
    int         top = (SCREEN_HEIGHT - (h1 + gap + h2)) / 2;
    if ( top < 0 )
      top = 0;
    int w1 = (int)strlen(l1) * 6 * s1;
    int w2 = (int)strlen(l2) * 6 * s2;
    display.setTextSize(s1);
    display.setCursor((SCREEN_WIDTH - w1) / 2, top);
    display.print(l1);
    display.setTextSize(s2);
    display.setCursor((SCREEN_WIDTH - w2) / 2, top + h1 + gap);
    display.print(l2);
  }
  display.display();
  delay(600);

  if ( display_setting )
  {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}

void showHomeScreen()
{
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("nRF24 Jammer");
  display.println(Version_Number);
  display.println("Scanning Wi-Fi APs");
  display.display();

  int networks = scan_wifi_APs(WiFiScanChannels, false);

  display.setCursor(0, 30);
  display.print("Found ");
  display.print(networks);
  display.println(" APs");
  display.display();
  delay(300);

  updateDisplay(menu_number);
}
