#ifndef SERIAL_H
#define SERIAL_H

#include "attack.h"
#include "config.h"
#include "html.h"
#include "jam.h"
#include "scan.h"

extern bool CommandsHandler(String command, bool web_command);
extern bool SerialCommands();
bool        handleJamCommand(String *args, int argCount, bool web_command, String &response);
bool        handleAttackCommand(String *args, int argCount, bool web_command, String &response);
bool        handleScanCommand(String *args, int argCount, bool web_command, String &response);
bool        handleSetCommand(String *args, int argCount, bool web_command, String &response);
bool        handleSetNrf24Command(String *args, int argCount, bool web_command, String &response);
bool        handleSetArray(String *args, int argCount, bool web_command, String &response);

#endif