#include "nrf_config.h"
#include "jam.h"
#include <algorithm>

void handlenRF24Init()
{
  if ( !prefs.isKey("nRF24_count") || prefs.getInt("nRF24_count", 0) <= 0 )
  {
    // Default config for ESP32-S3-WROOM-1 board: 3x E01-2G4M27D modules.
    // M1: CE=1, CSN=2 (on J3, short stubs)   M2: CE=18, CSN=17   M3: CE=6, CSN=7
    // (SPI shared SCK=12/MOSI=11/MISO=13)
    nrf24_count = 3;
    ce_pins[0]  = 1;
    csn_pins[0] = 2;
    ce_pins[1]  = 18;
    csn_pins[1] = 17;
    ce_pins[2]  = 6;
    csn_pins[2] = 7;
    prefs.putInt("nRF24_count", nrf24_count);
    prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
    prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
    return;
  }

  nrf24_count = prefs.getInt("nRF24_count", 1);

  if ( prefs.isKey("nRF24_ce_pins") )
  {
    prefs.getBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  }
  else
  {
    ce_pins[0] = 1;
    prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  }

  if ( prefs.isKey("nRF24_csn_pins") )
  {
    prefs.getBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
  }
  else
  {
    csn_pins[0] = 2;
    prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
  }
}

void handlenRF24Scan()
{
  HSPI_init();

  // ESP32-S3-WROOM-1 safe candidate pins for CE/CSN (exclude flash 26-32,
  // octal PSRAM 35-37, USB 19-20, UART 43-44, SPI 11-13, I2C 8-9,
  // buttons 21/47/48, RGB LED 38)
  const int candidate_pins[] = { 1, 2, 3, 6, 7, 10, 15, 16, 17, 18 };
  const int candidate_count  = sizeof(candidate_pins) / sizeof(candidate_pins[0]);

  auto readReg = [&](int csnPin, uint8_t reg) {
    hp->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(csnPin, LOW);
    hp->transfer(0x00 | reg);
    uint8_t val = hp->transfer(0xFF);
    digitalWrite(csnPin, HIGH);
    hp->endTransaction();
    return val;
  };
  auto writeReg = [&](int csnPin, uint8_t reg, uint8_t val) {
    hp->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(csnPin, LOW);
    hp->transfer(0x20 | reg);
    hp->transfer(val);
    digitalWrite(csnPin, HIGH);
    hp->endTransaction();
  };

  for ( int i = 0; i < candidate_count; i++ )
  {
    int p = candidate_pins[i];
    pinMode(p, OUTPUT);
    digitalWrite(p, HIGH);
  }

  int csn_pins_count = 0;
  for ( int i = 0; i < candidate_count; i++ )
  {
    int csn = candidate_pins[i];

    writeReg(csn, 0x05, 0x33);
    uint8_t test = readReg(csn, 0x05);
    if ( test != 0x33 )
      continue;

    writeReg(csn, 0x05, 0x44);
    test = readReg(csn, 0x05);
    if ( test != 0x44 )
      continue;

    csn_pins[csn_pins_count] = csn;
    csn_pins_count++;
  }

  for ( int i = 0; i < candidate_count; i++ )
  {
    int  p      = candidate_pins[i];
    bool is_csn = false;
    for ( int j = 0; j < csn_pins_count; j++ )
    {
      if ( csn_pins[j] == p )
      {
        is_csn = true;
        break;
      }
    }
    if ( !is_csn )
    {
      digitalWrite(p, LOW);
    }
  }

  if ( csn_pins_count == 0 )
  {
    HSPI_deinit();
    return;
  }

  int ce_pins_count = 0;
  for ( int i = 0; i < candidate_count; i++ )
  {
    int  ce  = candidate_pins[i];
    auto csn = std::find(csn_pins, csn_pins + csn_pins_count, ce);
    if ( csn != csn_pins + csn_pins_count )
      continue;

    for ( int j = 0; j < csn_pins_count; j++ )
    {
      int csn_pin = csn_pins[j];

      writeReg(csn_pin, 0x01, 0x00);
      writeReg(csn_pin, 0x00, 0x0E);
      delay(5);

      writeReg(csn_pin, 0x07, 0x70);

      hp->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
      digitalWrite(csn_pin, LOW);
      hp->transfer(0xE1);
      digitalWrite(csn_pin, HIGH);

      digitalWrite(csn_pin, LOW);
      hp->transfer(0xA0);
      hp->transfer(0x55);
      digitalWrite(csn_pin, HIGH);
      hp->endTransaction();

      digitalWrite(ce, HIGH);
      delayMicroseconds(50);
      digitalWrite(ce, LOW);

      delay(5);

      uint8_t status = readReg(csn_pin, 0x07);
      if ( (status & 0x20) == 0 )
        continue;

      ce_pins[ce_pins_count] = ce;
      ce_pins_count++;
    }
  }

  if ( ce_pins_count != csn_pins_count )
  {
    HSPI_deinit();
    return;
  }

  nrf24_count = ce_pins_count;

  prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
  prefs.putInt("nRF24_count", nrf24_count);

  HSPI_deinit();
}
