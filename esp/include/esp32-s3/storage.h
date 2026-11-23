#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"

// Persistent settings (NVS via Preferences). loadSettings() restores every
// configurable value at boot; the rest persist individual changes.

void loadSettings();
void storeEEPROMAndSet(const char *index, int value, int &targetVar);
void saveWiFiSettings(const char *new_ssid, const char *new_password);
void handleResetWiFiSettings();

#endif
