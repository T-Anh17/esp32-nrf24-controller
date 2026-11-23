#include "config.h"
#include "display.h"
#include "menu.h"
#include "nrf_config.h"
#include "serial.h"
#include "storage.h"
#include "web.h"

// Entry point. setup() only wires the subsystems together and loop() only
// drives navigation + servicing; the actual work lives in the feature modules
// (storage, nrf_config, web, display, menu, jam, scan, attack, serial).

void setup()
{
  Serial.begin(115200);
  // ESP32-S3 native USB-CDC: opening the serial monitor resets the board, and
  // logs printed before the host re-opens the port are lost. Wait briefly for
  // the monitor to attach so the boot log is visible, but keep it short so a
  // standalone (no-monitor) power-up boots fast. If Serial is already up we
  // skip the wait entirely; otherwise give it up to 400ms.
  unsigned long serialWaitStart = millis();
  while ( !Serial && millis() - serialWaitStart < 400 )
    delay(10);
  Serial.println("\n=== nRF24 Jammer boot ===");

  prefs.begin("configs", false);

  // Arduino-ESP32 v3's disableCore0WDT() unsubscribes the core-0 idle task but
  // leaves its WDT hook calling esp_task_wdt_reset() on that now-missing task,
  // flooding the USB-CDC console with "task_wdt: esp_task_wdt_reset(705): task
  // not found" and stalling boot so the Wi-Fi AP never comes up. Tear the Task
  // WDT down entirely instead: the jam loops block the loop task with only
  // delayMicroseconds() (no yield), which would otherwise trip the WDT.
  esp_task_wdt_deinit();

  // Blue: firmware is booting. Proves the RGB LED is alive before anything else.
  setRGB(0, 0, 30);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  handlenRF24Init();
  if ( nrf24_count == 0 )
    handlenRF24Scan();

  loadSettings();
  startWebServer();
  initButtons();
  initOLED();
  showHomeScreen();

  // Green: setup finished, device is ready.
  setRGB(0, 30, 0);
}

void loop()
{
  btnOK.tick();
  btnNext.tick();
  btnPrevious.tick();

  // --- Button debug log --- print each button pin level whenever it changes.
  // Pin map: OK=GPIO21, Next=GPIO47, Previous=GPIO48. GyverButton sets these
  // pins to INPUT_PULLUP, so idle=1 and pressed=0.
  static int lastOK = -1, lastNext = -1, lastPrev = -1;
  int        okLvl   = digitalRead(21);
  int        nextLvl = digitalRead(47);
  int        prevLvl = digitalRead(48);
  if ( okLvl != lastOK )
  {
    Serial.printf("[BTN] OK  (GPIO21) = %d  (%s)\n", okLvl, okLvl == 0 ? "PRESSED" : "released");
    lastOK = okLvl;
  }
  if ( nextLvl != lastNext )
  {
    Serial.printf("[BTN] NEXT(GPIO47) = %d  (%s)\n", nextLvl, nextLvl == 0 ? "PRESSED" : "released");
    lastNext = nextLvl;
  }
  if ( prevLvl != lastPrev )
  {
    Serial.printf("[BTN] PREV(GPIO48) = %d  (%s)\n", prevLvl, prevLvl == 0 ? "PRESSED" : "released");
    lastPrev = prevLvl;
  }

  SerialCommands();

  if ( access_point == 0 )
  {
    if ( webServerStarted )
    {
      server.handleClient();
    }
    if ( dnsServerStarted )
    {
      dnsServer.processNextRequest();
    }
  }
  if ( buttons == 0 )
  {
    if ( btnOK.isSingle() )
    {
      menu_number = (menu_number + 1) % 7;
      updateDisplay(menu_number);
    }
    if ( btnOK.isHolded() )
    {
      executeAction(menu_number);
      btnOK.resetStates();
    }
  }
  else if ( buttons == 1 )
  {
    if ( btnOK.isSingle() )
    {
      executeAction(menu_number);
    }
    if ( btnNext.isSingle() )
    {
      menu_number = (menu_number + 1) % 7;
      updateDisplay(menu_number);
    }
  }
  else if ( buttons == 2 )
  {
    if ( btnOK.isSingle() )
    {
      executeAction(menu_number);
    }

    if ( btnNext.isSingle() )
    {
      menu_number = (menu_number + 1) % 7;
      updateDisplay(menu_number);
    }

    if ( btnPrevious.isSingle() )
    {
      menu_number = (menu_number - 1 + 7) % 7;
      updateDisplay(menu_number);
    }
  }
}
