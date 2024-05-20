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

bool isPinUsed(const char* path, String pin) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return false;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    std::vector<String> data = split(line, ';');
    if (data.size() > 3 && data[3] == pin) {  // Assuming pin is the fourth element in the stored data
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
      if (data.size() >= 3) {  // Garante que há pelo menos três partes na linha
        obj["Nome"] = data[0];
        obj["Descricao"] = data[1];
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

String getCentralIP() {
  File configFile = LittleFS.open("/config.txt", "r");
  if (!configFile) {
    Serial.println("Failed to open config file for reading");
    return "";  // Retorna uma string vazia se não conseguir abrir o arquivo
  }

  // Ler a linha do cabeçalho para ignorá-la
  configFile.readStringUntil('\n');

  // Ler a linha seguinte que contém os dados
  String data = configFile.readStringUntil('\n');
  configFile.close();

  // Extrair o IP Central
  int firstSemiColon = data.indexOf(';');
  int secondSemiColon = data.indexOf(';', firstSemiColon + 1);

  // Substring desde o caractere após o primeiro ponto e vírgula até o segundo ponto e vírgula
  String centralIP = data.substring(secondSemiColon + 1);

  return centralIP;
}

void readSensorsAndActuators(JsonArray& sensors, JsonArray& actuators, const char* sensorPath, const char* actuatorPath) {



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
      std::vector<String> fields = split(line, ';');

      // Atribuir campos ao objeto JSON
      if (fields.size() >= 9) {
        sensor["ID"] = fields[0];
        sensor["Nome"] = fields[1];
        sensor["Tipo"] = fields[2];
        sensor["Pin"] = fields[3];
        sensor["ModoOperacao"] = fields[4];
        sensor["Valor"] = readSensorValue(fields[3].toInt(), fields[4]);
        sensor["DataCriacao"] = fields[6];
        sensor["DispositivoId"] = fields[7];
        sensor["Unidade"] = fields[8];
        sensor["isDeleted"] = fields[9];
      }
    }
  }
  sensorFile.close();


  // Ler atuadores
  File actuatorFile = LittleFS.open(actuatorPath, "r");
  if (!actuatorFile) {
    Serial.println("Falha ao abrir o arquivo de atuadores.");
    return;
  }

  while (actuatorFile.available()) {
    line = actuatorFile.readStringUntil('\n');

    // Ignorar cabeçalho e linhas vazias
    if (line.length() > 0 && !line.startsWith("ID;")) {
      JsonObject actuator = actuators.createNestedObject();
      std::vector<String> fields = split(line, ';');

      // Atribuir campos ao objeto JSON
      if (fields.size() >= 9) {
        actuator["ID"] = fields[0];
        actuator["Nome"] = fields[1];
        actuator["Tipo"] = fields[2];
        actuator["Pin"] = fields[3];
        actuator["ModoOperacao"] = fields[4];
        actuator["Valor"] = readAtuadorValue(fields[3].toInt(), fields[4]);
        actuator["DataCriacao"] = fields[6];
        actuator["DispositivoId"] = fields[7];
        actuator["Unidade"] = fields[8];
      }
    }
  }
  actuatorFile.close();
}


String combineAllDataToJson(const char* configPath, const char* sensorPath, const char* actuatorPath) {
  // Cria um documento JSON dinâmico de tamanho adequado
  DynamicJsonDocument doc(2048);  // Ajuste o tamanho conforme necessário

  // Lê a configuração e armazena no objeto JSON
  String config = readConfig(configPath, doc);


  String macAddress = WiFi.macAddress();
  String assignedIP = WiFi.localIP().toString();

  doc["MacAddress"] = macAddress;
  doc["IPAtribuido"] = assignedIP;

  // Remover a linha "Config" do objeto JSON
  doc.remove("Config");

  // Cria arrays JSON para sensores e atuadores
  JsonArray sensors = doc.createNestedArray("Sensores");
  JsonArray actuators = doc.createNestedArray("Atuadores");

  // Lê sensores e atuadores dos arquivos correspondentes
  readSensorsAndActuators(sensors, actuators, sensorPath, actuatorPath);

  // Serializa o documento JSON para uma String
  String jsonString;
  serializeJson(doc, jsonString);

  // Imprime e retorna a string JSON resultante
  return jsonString;
}

void initCentralIP() {
  centralIP = getCentralIP();
  centralIP.trim();
}

bool sendPostRequest(const char* url, const String& payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    http.begin(client ,url);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on HTTP request: " + String(httpResponseCode));
    }
    
    http.end();
    return httpResponseCode > 0;
  } else {
    Serial.println("WiFi not connected");
    return false;
  }
}
