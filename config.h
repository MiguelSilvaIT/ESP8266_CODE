#ifndef CONFIG_H
#define CONFIG_H

#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// Definição do pino do LED
const int ledPin = 2;

// Declaração externa, a definição será em um arquivo .cpp
extern const char* sensors_path;

extern const char* atuadores_path;

#endif