#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <NTPClient.h>
#include <WiFiUdp.h>

// Inicializa o cliente NTP
void setupTimeClient();

// Atualiza a hora usando o cliente NTP
void updateTime();

// Obtém a hora atual formatada em ISO 8601
String getFormattedTime();

#endif
