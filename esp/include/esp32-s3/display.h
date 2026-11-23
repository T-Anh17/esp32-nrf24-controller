#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

// Low-level OLED helpers shared by every UI path. The compressed bitmap assets
// are not bundled (decodeLZSS resolves to nullptr), so the whole UI is rendered
// as text instead: uiTitle() draws a titled status screen and updateDisplay()
// draws the home menu entry for a given index.

// I2C bus recovery + address scan + panel bring-up + self-test. Sets oledPresent.
void initOLED();

// Splash + initial Wi-Fi AP scan, then show the first menu entry.
void showHomeScreen();

// Draw a titled screen: title (word-wrapped) at the top, optional hint at the
// bottom line. Used by the jam/attack/menu screens in place of the icons.
void uiTitle(const String &title, const String &hint = "");

// Draw the home menu entry for the current menu_number as text.
void updateDisplay(int menuNum);

#endif
