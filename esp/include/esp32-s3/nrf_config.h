#ifndef NRF_CONFIG_H
#define NRF_CONFIG_H

#include "config.h"

// nRF24 radio inventory. handlenRF24Init() restores the saved CE/CSN pin map
// (seeding a 3-module default on first boot); handlenRF24Scan() probes the
// candidate GPIOs over SPI to auto-detect wired modules when none are saved.

void handlenRF24Init();
void handlenRF24Scan();

#endif
