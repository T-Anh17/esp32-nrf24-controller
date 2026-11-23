#ifndef WEB_H
#define WEB_H

#include "config.h"

// HTTP control plane: route handlers plus startWebServer(), which brings up the
// soft-AP, registers every route and starts the server when AP mode is enabled.

void startWebServer();

void handleRoot();
void HandleWebCommand();
void handlerSetSettings();
void RescanHandler();

void sendHtmlAndExecute(const uint8_t *htmlData, size_t len, void (*action)() = nullptr);
void attackHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap);
void attackScanHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap);

void miscChannelsHandler();
void wifiChannelsHandler();
void wifiDeauthChannelsHandler();

void handleApiStatus();
void handleApiSettings();
void handleApiVersion();

void handleFileUpload();

#endif
