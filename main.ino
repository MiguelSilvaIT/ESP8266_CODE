#include <Arduino.h>

#include "filesystem.h"
#include "time_manager.h"
#include "sensor.h"
#include "network.h"
#include "config.h"
#include "utils.h"


// Definição dos pinos e variáveis globais

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

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

  //combineAllDataToJson(config_path, sensors_path, atuadores_path);
  delay(5000);

  if (WiFi.isConnected()) {
        postAllData("http://192.168.1.6:3000/", config_path, sensors_path, atuadores_path);
        delay(5000);  // Faz um pedido a cada 10 segundos
    }

  // Mantém o mDNS atualizado
  MDNS.update();
}