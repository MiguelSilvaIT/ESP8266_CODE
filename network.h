#ifndef NETWORK_H
#define NETWORK_H

#include <WiFiManager.h>
#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


void setupWiFi();
void configureWebServer(AsyncWebServer &server);
void postAllData(String url, const char* configPath, const char* sensorPath, const char* actuatorPath);
bool deleteSensorRequest(int sensorId);
String readESPConfig();


#endif
