#ifndef NETWORK_H
#define NETWORK_H

#include <WiFiManager.h>
#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


void setupWiFi();
void configureWebServer(AsyncWebServer &server);
void postAllData(const char* url, const char* configPath, const char* sensorPath, const char* actuatorPath);


#endif