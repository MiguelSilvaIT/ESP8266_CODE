#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <vector>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "filesystem.h"
#include "sensor.h"
#include "atuador.h"


std::vector<String> split(const String& data, char delimiter);

bool isPinUsed(const char* path, String pin);


String combineAllDataToJson(const char* configPath, const char* sensorPath, const char* actuatorPath);

void readSensorsAndActuators(JsonArray& sensors, JsonArray& actuators, const char* sensorPath, const char* actuatorPath);

String readConfig(const char* path, JsonDocument& doc);

String getCentralIP();

void initCentralIP();
bool sendPostRequest(const char* url, const String& payload);


#endif
