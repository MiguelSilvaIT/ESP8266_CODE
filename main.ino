#include <Arduino.h>

#include "filesystem.h"
#include "time_manager.h"
#include "network.h"
#include "config.h"
#include "utils.h"


AsyncWebServer server(80);


DHT dht(5, DHT11);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);


  // Inicialização do sistema de arquivos
  initFS();
  
  dht.begin();


  // Configuração da rede e do servidor web
  setupWiFi();
  configureWebServer(server);

  // Inicializa o cliente NTP
  setupTimeClient();

  initCentralIP();



  // Configurações adicionais podem ser feitas aqui
  server.begin();
}

void loop() {

  delay(5000);

  //float t = lerDHT11(dht);



  // Atualização do NTP Client
  updateTime();

  //combineAllDataToJson(config_path, sensors_path, atuadores_path);
  if (WiFi.isConnected()) {
    // Obter o IP como String


    // Verificar se o IP foi obtido corretamente antes de fazer a chamada
    if (centralIP.length() > 0) {
      String url = "http://" + centralIP + ":3000/";
      String endpoint = "dispositivos-nao-registados";
      checkForUnregisteredDevices((url + endpoint).c_str());
      postAllData(url, config_path, sensors_path, atuadores_path);
    }
  }

  // Mantém o mDNS atualizado
  MDNS.update();
}
