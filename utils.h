#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <vector>
#include "filesystem.h"


std::vector<String> split(const String& data, char delimiter);

bool isPinUsed(const char* path, int pin);


String combineAllDataToJson(const char* configPath, const char* sensorPath, const char* actuatorPath);

void readSensorsAndActuators(JsonArray& sensors, JsonArray& actuators, const char* sensorPath, const char* actuatorPath);

String readConfig(const char* path, JsonDocument& doc);


#endif