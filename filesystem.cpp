#include "filesystem.h"
#include <Arduino.h>

void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

bool handleESPConfig(JsonDocument& doc) {
  Serial.println("Inicio Handle Config");
  File configFile = LittleFS.open(config_path, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  // Escrevendo o cabeçalho do arquivo CSV
  configFile.println("Name;Description;Central IP");

  const char* name = doc["nome"].as<const char*>();
  const char* description = doc["descricao"].as<const char*>();
  const char* centralIP = doc["ipCentral"].as<const char*>();

  // Formatando os dados como uma linha CSV
  String data = String(name) + ";" + description + ";" + centralIP;
  configFile.print(data);
  configFile.close();

  // Reabrir o arquivo para leitura e verificar o conteúdo
  configFile = LittleFS.open("/config.txt", "r");
  if (!configFile) {
    Serial.println("Failed to open config file for reading");
    return false;
  }

  Serial.println("Saved Configuration:");
  String fileContent;
  while (configFile.available()) {
    fileContent += char(configFile.read());
  }
  configFile.close();

  // Imprimir o conteúdo do arquivo
  Serial.print(fileContent);

  return true;
}


