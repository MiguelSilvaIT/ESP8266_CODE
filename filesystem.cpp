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
  configFile.println(data);
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


String readESPConfig() {
    File configFile = LittleFS.open("/config.txt", "r");
    if (!configFile) {
        Serial.println("Failed to open config file for reading");
        return "";
    }

    String header = configFile.readStringUntil('\n');  // Read the header line
    String line = configFile.readStringUntil('\n');    // Read the first (and presumably only) line of data

    configFile.close();

    if (line.length() == 0) {
        Serial.println("No data found in config file");
        return "";
    }

    // Parse the CSV line
    int nameEnd = line.indexOf(';');
    int descriptionEnd = line.indexOf(';', nameEnd + 1);
    
    if (nameEnd == -1 || descriptionEnd == -1) {
        Serial.println("Invalid config format");
        return "";
    }

    String name = line.substring(0, nameEnd);
    String description = line.substring(nameEnd + 1, descriptionEnd);
    String centralIP = line.substring(descriptionEnd + 1);

    // Create the JSON document
    DynamicJsonDocument doc(1024);
    doc["name"] = name;
    doc["description"] = description;
    doc["centralIP"] = centralIP;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}


