#include "time_manager.h"
#include <Arduino.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

void setupTimeClient() {
    timeClient.begin();
}

void updateTime() {
    timeClient.update();
}

String getFormattedTime() {
    unsigned long epochTime = timeClient.getEpochTime();
    time_t now = epochTime;
    struct tm* tm_struct = gmtime(&now);

    char buf[25];
    strftime(buf, sizeof(buf), "%FT%TZ", tm_struct);
    return String(buf);
}
