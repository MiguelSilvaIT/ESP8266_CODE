#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <vector>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "filesystem.h"
#include "time_manager.h"
#include "config.h"

// Estrutura para armazenar os IDs
struct DeviceIDMapping {
  int rowid;
  int newId;
};

std::vector<String> split(const String& data, char delimiter);

bool isPinUsed(const char* path, String pin);


String combineAllDataToJson(const char* configPath, const char* sensorPath, const char* actuatorPath);

void readSensorsAndActuators(JsonArray& sensors, JsonArray& actuators, const char* sensorPath, const char* actuatorPath);

String readConfig(const char* path, JsonDocument& doc);

String getCentralIP();

void initCentralIP();
bool sendPostRequest(const char* url, const String& payload);

int addDevice(const char* path, JsonDocument doc);
int readLastDeviceId(const char* path);
void updateLastDeviceId(const char* path, int lastId);

float readDeviceValue(int pin, String modoOperacao, String tipo);

String getAllDeviceData(const char* path);

bool deleteDeviceById(const char* filePath, int targetID);

bool setAtuadorValue(int pin, float value, String tipo);

String sendGETRequest(const char* url);

bool checkForUnregisteredDevices(const char* path);

void sendIDMappingToServer(const std::vector<DeviceIDMapping>& sensorMappings, const std::vector<DeviceIDMapping>& actuatorMappings);
#endif
