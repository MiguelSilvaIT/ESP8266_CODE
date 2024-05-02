#include <Arduino.h>

#include "filesystem.h"
#include "time_manager.h"
#include "sensor.h"
#include "network.h"
#include "config.h"


// Definição dos pinos e variáveis globais

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Inicialização do sistema de arquivos
  initFS();

  // Configuração da rede e do servidor web
  setupWiFi();
  configureWebServer(server);

  // Inicializa o cliente NTP
  setupTimeClient();

  // Configurações adicionais podem ser feitas aqui
  server.begin();
}

void loop() {
  // Atualização do NTP Client
  updateTime();

 

  // Mantém o mDNS atualizado
  MDNS.update();
}