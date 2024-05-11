#include "utils.h"


std::vector<String> split(const String& data, char delimiter) {
  std::vector<String> result;
  int start = 0;
  int end = data.indexOf(delimiter);
  while (end != -1) {
    result.push_back(data.substring(start, end));
    start = end + 1;
    end = data.indexOf(delimiter, start);
  }
  result.push_back(data.substring(start));  // Adiciona o último elemento
  return result;
}

bool isPinUsed(const char* path, int pin) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        std::vector<String> data = split(line, ';');
        if (data.size() > 3 && data[3].toInt() == pin) {  // Assuming pin is the fourth element in the stored data
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

String readConfig(const char* path, JsonDocument& doc) {

    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        String result;
        serializeJson(doc, result);
        return result;
    }

    JsonObject obj = doc.to<JsonObject>();
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (!line.startsWith("Name") && line.length() > 0) {
            std::vector<String> data = split(line, ';');
            if (data.size() >= 3) { // Garante que há pelo menos três partes na linha
                obj["Name"] = data[0];
                obj["Description"] = data[1];
                obj["CentralIP"] = data[2];
            }
        }
    }
    file.close();

    String result;
    serializeJson(obj, result);
    Serial.println(result);
    return result;
}

void readSensorsAndActuators(JsonArray& sensors, JsonArray& actuators, const char* sensorPath, const char* actuatorPath) {
  

  Serial.println(sensorPath);

  // Abrir arquivo de sensores
  File sensorFile = LittleFS.open(sensorPath, "r");
  if (!sensorFile) {
    Serial.println("Falha ao abrir o arquivo de sensores.");
    return;
  }

  String line;
  // Ler linha por linha do arquivo
  while (sensorFile.available()) {
    line = sensorFile.readStringUntil('\n');


    // Ignorar cabeçalho e linhas vazias
    if (line.length() > 0 && !line.startsWith("ID;")) {
      JsonObject sensor = sensors.createNestedObject();
      int index = 0;

      // Dividir a linha em campos usando ';' como delimitador
      for (int start = 0, end = line.indexOf(';'); end != -1; start = end + 1, end = line.indexOf(';', start)) {
        String field = line.substring(start, end);
        // Serial.print("Campo ");
        // Serial.print(index);
        // Serial.print(": ");
        // Serial.println(field);

        // Atribuir campos ao objeto JSON com base no índice
        switch (index++) {
          case 0: sensor["ID"] = field.toInt(); break;
          case 1: sensor["Nome"] = field; break;
          case 2: sensor["Tipo"] = field; break;
          case 3: sensor["Pin"] = field.toInt(); break;
          case 4: sensor["ModoOperacao"] = field; break;
          case 5: sensor["Valor"] = field.toDouble(); break;
          case 6: sensor["DataCriacao"] = field; break;
          case 7: sensor["DispositivoId"] = field.toInt(); break;
          case 8: sensor["UnidadeId"] = field.toInt(); break;
        }
      }
      // Para o último campo após o último ';'
      String lastField = line.substring(line.lastIndexOf(';') + 1);
      // Serial.print("Último campo: ");
      // Serial.println(lastField);
    }
  }
  sensorFile.close();


  // Ler atuadores
  File actuatorFile = LittleFS.open(actuatorPath, "r");
  if (actuatorFile) {
    String line;
    while (actuatorFile.available()) {
      line = actuatorFile.readStringUntil('\n');
      if (line.length() > 0 && !line.startsWith("ID;")) { // Ignorando o cabeçalho
        JsonObject actuator = actuators.createNestedObject();
        int index = 0;
        for (int start = 0, end = line.indexOf(';'); end != -1; start = end + 1, end = line.indexOf(';', start)) {
          String field = line.substring(start, end);
          switch (index++) {
            case 0: actuator["ID"] = field.toInt(); break;
            case 1: actuator["Nome"] = field; break;
            case 2: actuator["Tipo"] = field; break;
            case 3: actuator["Pin"] = field.toInt(); break;
            case 4: actuator["ModoOperacao"] = field; break;
            case 5: actuator["Valor"] = field.toDouble(); break;
            case 6: actuator["DataCriacao"] = field; break;
            case 7: actuator["DispositivoId"] = field.toInt(); break;
            case 8: actuator["UnidadeId"] = field; break;
          }
        }
      }
    }
    actuatorFile.close();

  }
}

String combineAllDataToJson(const char* configPath, const char* sensorPath, const char* actuatorPath) {
  
  DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);  // Aumenta o tamanho se necessário

  // Configuração

  String config = readConfig(configPath, doc);
  doc["Config"] = config;


  // Sensores e atuadores

  JsonArray sensors = doc.createNestedArray("Sensors");
  JsonArray actuators = doc.createNestedArray("Actuators");



  readSensorsAndActuators(sensors, actuators, sensorPath, actuatorPath);

  // Serializa o documento JSON para uma String
  String jsonString;
  serializeJson(doc, jsonString);

  Serial.println(jsonString);
  return jsonString;
}