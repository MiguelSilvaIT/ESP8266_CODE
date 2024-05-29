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
    data[9].trim();
    if (data.size() > 3 && data[3] == pin  && data[9] == "false") {  // Assuming pin is the fourth element in the stored data
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
    // Tenta abrir o arquivo de configuração
    Serial.println("Tentando abrir o arquivo de configuração...");
    File configFile = LittleFS.open("/config.txt", "r");
    if (!configFile) {
        Serial.println("Falha ao abrir o arquivo de configuração para leitura");
        return "";  // Retorna uma string vazia se não conseguir abrir o arquivo
    }

    // Ler a linha do cabeçalho para ignorá-la
    if (configFile.available()) {
        Serial.println("Lendo a linha do cabeçalho...");
        String header = configFile.readStringUntil('\n');
        Serial.print("Cabeçalho: ");
        Serial.println(header);
    } else {
        Serial.println("Arquivo de configuração está vazio ou corrompido");
        configFile.close();
        return "";
    }

    // Ler a linha seguinte que contém os dados
    if (configFile.available()) {
        Serial.println("Lendo a linha de dados...");
        String data = configFile.readStringUntil('\n');
        Serial.print("Dados lidos: ");
        Serial.println(data);

        // Extrair o IP Central
        Serial.println("Extraindo o IP Central...");
        int firstSemiColon = data.indexOf(';');
        if (firstSemiColon == -1) {
            Serial.println("Erro ao encontrar o primeiro ponto e vírgula");
            configFile.close();
            return "";
        }
        Serial.print("Índice do primeiro ponto e vírgula: ");
        Serial.println(firstSemiColon);

        int secondSemiColon = data.indexOf(';', firstSemiColon + 1);
        if (secondSemiColon == -1) {
            Serial.println("Erro ao encontrar o segundo ponto e vírgula");
            configFile.close();
            return "";
        }
        Serial.print("Índice do segundo ponto e vírgula: ");
        Serial.println(secondSemiColon);

        // Substring desde o caractere após o segundo ponto e vírgula até o final
        String centralIP = data.substring(secondSemiColon + 1);
        centralIP.trim();  // Remover quaisquer espaços em branco ao redor
        Serial.print("IP Central extraído: ");
        Serial.println(centralIP);

        configFile.close();
        return centralIP;
    } else {
        Serial.println("Arquivo de configuração não contém dados suficientes");
        configFile.close();
        return "";
    }
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
        sensor["Valor"] = readSensorValue(fields[3].toInt(), fields[4], fields[2]);
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
        actuator["isDeleted"] = fields[9];
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
    Serial.print("URL-->");
    Serial.println(url);
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


String addDevice(const char* path, JsonDocument& doc) {
    String pin = doc["pin"];
    if (isPinUsed(path, pin)) {
        Serial.println("Pin is already in use");
        return "Pin is already in use";  
    }

    if (!LittleFS.exists(path)) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            Serial.println("Failed to open file for writing");
            return "Failed to open file for writing";
        }
        file.println("ID;Nome;Tipo;Pin;ModoOperacao;Valor;DataCriacao;DispositivoId;Unidade;isDeleted");
        file.close();
    }

    int lastId = readLastDeviceId(id_path);
    lastId++;
    doc["id"] = lastId;

    File file = LittleFS.open(path, "a");
    if (!file) {
        Serial.println("Failed to open file for appending");
        return "Failed to open file for appending";
    }

    String dataString = String(doc["id"].as<int>()) + ";" + String((const char*)doc["nome"]) + ";"
                        + String((const char*)doc["tipo"]) + ";" + String((const char*)doc["pin"]) + ";"
                        + String((const char*)doc["modoOperacao"]) + ";" + String((float)doc["valor"]) + ";"
                        + String((const char*)doc["dtCriacao"]) + ";" + String((int)doc["dispositivoId"]) + ";"
                        + String((const char*)doc["unidade"]) + ";false";

    if (file.println(dataString)) {
        Serial.println("Device data appended");
        updateLastDeviceId(id_path, lastId);
        file.close();
        return "Device data appended successfully";
    } else {
        Serial.println("Append failed");
        file.close();
        return "Append failed";
    }
}


int readLastDeviceId(const char* path) {
    if (!LittleFS.exists(path)) {
        Serial.println("No ID file found, starting from ID 0");
        return -1; // Ou return 0 se preferir começar os IDs de 1
    }

    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading last ID");
        return -1; // Retorna -1 se não puder abrir o arquivo
    }

    String idStr = file.readStringUntil('\n');
    file.close();

    Serial.print("Last Device Id -->");
    Serial.println(idStr);

    return idStr.toInt(); // Converte a string lida para int e retorna
}

void updateLastDeviceId(const char* path, int lastId) {
    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.println("Failed to open file for updating last ID");
        return;
    }

    file.println(lastId); // Escreve o novo último ID no arquivo
    file.close();
    Serial.println("Last ID updated successfully to " + String(lastId));
}