#ifndef MENU_H
#define MENU_H

#include "config.h"

// On-device UI driven by the three buttons. Each screen renders as text (the
// bitmap assets are stubbed out) and blocks until the user leaves it.

void initButtons();
void settings();
void misc();
void ble_select();
void wifi_select();
void executeAction(int menuNum);

#endif
