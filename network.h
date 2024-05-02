#ifndef NETWORK_H
#define NETWORK_H

#include <WiFiManager.h>
#define WEBSERVER_H
#include <ESPAsyncWebServer.h>


void setupWiFi();
void configureWebServer(AsyncWebServer &server);

#endif