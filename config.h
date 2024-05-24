#ifndef CONFIG_H
#define CONFIG_H

#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 1 //Corresponde ao D1 no ESP2866 

#define DHTTYPE DHT11

// Definição do pino do LED
const int ledPin = 2;

// Declaração externa, a definição será em um arquivo .cpp
extern const char* sensors_path;

extern const char* atuadores_path;

extern const char* config_path;

extern String centralIP;



#endif
