#include "filesystem.h"
#include <Arduino.h>

void initFS() {
    if (!LittleFS.begin()) {
        Serial.println("An error has occurred while mounting LittleFS");
    } else {
        Serial.println("LittleFS mounted successfully");
    }
}

