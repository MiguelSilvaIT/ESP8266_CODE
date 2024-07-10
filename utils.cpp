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
    File configFile = LittleFS.open("/config.txt", "r");
    if (!configFile) {
        Serial.println("Falha ao abrir o arquivo de configuração para leitura");
        return "";  // Retorna uma string vazia se não conseguir abrir o arquivo
    }

    // Ler a linha do cabeçalho para ignorá-la
    if (configFile.available()) {
        String header = configFile.readStringUntil('\n');
    
    } else {
        Serial.println("Arquivo de configuração está vazio ou corrompido");
        configFile.close();
        return "";
    }

    // Ler a linha seguinte que contém os dados
    if (configFile.available()) {
        String data = configFile.readStringUntil('\n');
    
        int firstSemiColon = data.indexOf(';');
        if (firstSemiColon == -1) {
            Serial.println("Erro ao encontrar o primeiro ponto e vírgula");
            configFile.close();
            return "";
        }


        int secondSemiColon = data.indexOf(';', firstSemiColon + 1);
        if (secondSemiColon == -1) {
            Serial.println("Erro ao encontrar o segundo ponto e vírgula");
            configFile.close();
            return "";
        }


        // Substring desde o caractere após o segundo ponto e vírgula até o final
        String centralIP = data.substring(secondSemiColon + 1);
        centralIP.trim();  // Remover quaisquer espaços em branco ao redor


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
        sensor["Valor"] = readDeviceValue(fields[3].toInt(), fields[4], fields[2]);
        sensor["DataCriacao"] = fields[6];
        sensor["DispositivoId"] = fields[7];
        sensor["Unidade"] = fields[8];
        sensor["isDeleted"] = fields[9];
        sensor["is_ESP_registed"] = 1;
        
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
        actuator["Valor"] = readDeviceValue(fields[3].toInt(), fields[4],fields[2] );
        actuator["DataCriacao"] = fields[6];
        actuator["DispositivoId"] = fields[7];
        actuator["Unidade"] = fields[8];
        actuator["isDeleted"] = fields[9];
        actuator["is_ESP_registed"] = 1;
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

String sendGETRequest(const char* url){
   if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String response;
    http.begin(client ,url);
    http.addHeader("Content-Type", "application/json");
    Serial.print("URL-->");
    Serial.println(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on HTTP request: " + String(httpResponseCode));
    }
    
    http.end();
    return response;
  } else {
    Serial.println("WiFi not connected");
    return "WiFi not connected";
  }
}




// Função modificada para extrair rowid e enviar mapeamento para o servidor
bool checkForUnregisteredDevices(const char* path) {
  String devices = sendGETRequest(path);
  Serial.println("Devices-->");
  Serial.println(devices);

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, devices);

  if (error) {
    Serial.print("Erro de serialização: ");
    Serial.println(error.c_str());
    return false;
  }

  //caso seja recebido o seguinte JSON "{"sensores":[],"atuadores":[]}"
 if (doc["sensores"].size() == 0 && doc["atuadores"].size() == 0) {
    Serial.println("Não há dispositivos não registrados");
    return false;
}

  // Arrays para armazenar os mapeamentos de ID
  std::vector<DeviceIDMapping> sensorMappings;
  std::vector<DeviceIDMapping> actuatorMappings;

  // Extrair rowid dos sensores (assumindo que você tem uma função para adicionar sensores)
  for (JsonObject sensor : doc["sensores"].as<JsonArray>()) {
    int central_id = sensor["central_id"];
    int newId = addDevice(sensors_path , sensor); // Função fictícia para adicionar sensor e retornar novo ID
    sensorMappings.push_back({central_id, newId});
  }

  // Extrair rowid dos atuadores (assumindo que você tem uma função para adicionar atuadores)
  for (JsonObject actuator : doc["atuadores"].as<JsonArray>()) {
    int central_id = actuator["central_id"];
    int newId = addDevice(atuadores_path , actuator); // Função fictícia para adicionar atuador e retornar novo ID
    actuatorMappings.push_back({central_id, newId});
  }

  // Enviar mapeamento para o servidor (assumindo que você tem uma função para isso)
  sendIDMappingToServer(sensorMappings, actuatorMappings);

  return true;
}

void sendIDMappingToServer(const std::vector<DeviceIDMapping>& sensorMappings, const std::vector<DeviceIDMapping>& actuatorMappings) {
  // Cria um documento JSON para armazenar os mapeamentos
  DynamicJsonDocument doc(2048);

  // Cria arrays JSON para sensores e atuadores
  JsonArray sensorArray = doc.createNestedArray("sensores");
  JsonArray actuatorArray = doc.createNestedArray("atuadores");

  //debug
  Serial.println("Mapeamentos de sensores:");
  for (const DeviceIDMapping& mapping : sensorMappings) {
    Serial.print("RowID: ");
    Serial.print(mapping.rowid);
    Serial.print(", NewID: ");
    Serial.println(mapping.newId);
  }

  Serial.println("Mapeamentos de atuadores:");
  for (const DeviceIDMapping& mapping : actuatorMappings) {
    Serial.print("RowID: ");
    Serial.print(mapping.rowid);
    Serial.print(", NewID: ");
    Serial.println(mapping.newId);
  }

  // Adiciona os mapeamentos de sensores ao array JSON
  for (const DeviceIDMapping& mapping : sensorMappings) {
    JsonObject sensor = sensorArray.createNestedObject();
    sensor["central_id"] = mapping.rowid;
    sensor["newId"] = mapping.newId;
  }

  // Adiciona os mapeamentos de atuadores ao array JSON
  for (const DeviceIDMapping& mapping : actuatorMappings) {
    JsonObject actuator = actuatorArray.createNestedObject();
    actuator["central_id"] = mapping.rowid;
    actuator["newId"] = mapping.newId;
  }

  // Serializa o documento JSON para uma string
  String payload;
  serializeJson(doc, payload);

  Serial.println("ID Mapping Payload:");
  Serial.println(payload);

  String url = "http://" + centralIP + ":3000/atualizar-ESPRegisted";

  sendPostRequest(url.c_str(), payload);
}

int addDevice(const char* path, JsonDocument doc) {
    String pin = doc["pin"];
    if (isPinUsed(path, pin)) {
        Serial.println("Pin is already in use");
        return -2;  
    }

    if (!LittleFS.exists(path)) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            Serial.println("Failed to open file for writing");
            return -3;
        }
        file.println("ID;Nome;Tipo;Pin;ModoOperacao;Valor;DataCriacao;DispositivoId;Unidade;isDeleted");
        file.close();
    }

    //mostra o conteudo do doc
    Serial.println("Doc-->");
    serializeJson(doc, Serial);
    Serial.println();
    

    int lastId = readLastDeviceId(id_path);
    lastId++;
    doc["id"] = lastId;

    File file = LittleFS.open(path, "a");
    if (!file) {
        Serial.println("Failed to open file for appending");
        return -4;
    }

    String dataString = String(doc["id"].as<int>()) + ";" + String((const char*)doc["nome"]) + ";"
                        + String((const char*)doc["tipo"]) + ";" + String((const char*)doc["pin"]) + ";"
                        + String((const char*)doc["modoOperacao"]) + ";" + String((float)doc["valor"]) + ";"
                        + getFormattedTime() + ";" + String((int)doc["dispositivoId"]) + ";"
                        + String((const char*)doc["unidade"]) + ";false";

    if (file.println(dataString)) {
        updateLastDeviceId(id_path, lastId);
        file.close();
        Serial.println("Device data appended successfully");
        //mostar conteudo do ficheiro
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return -1;
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            Serial.println(line);
        }
        file.close();

        return lastId;
    } else {
        Serial.println("Append failed");
        file.close();
        return -1;
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
}


float readDeviceValue(int pin, String modoOperacao, String tipo) {

  if (modoOperacao == "Analogico") 
  {
    float analogValue = 1023 - analogRead(pin);
   
    return analogValue;
  } 
  else if (modoOperacao == "Digital") {

    tipo.trim();
    int digitalValue = digitalRead(pin);
   
    return digitalValue;
  }
  Serial.println("ModoOperacao not recognized, returning default value 0");
  return 0;  // Retorna 0 como default se o tipo de sensor não for reconhecido
}

String getAllDeviceData(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "{}";  // Retorna um objeto JSON vazio em caso de falha
  }

  // Ignora o cabeçalho
  if (file.available()) {
    file.readStringUntil('\n');
  }

  DynamicJsonDocument doc(2048);  // Ajuste o tamanho conforme necessário
  JsonArray array = doc.to<JsonArray>();

  while (file.available()) {
    String line = file.readStringUntil('\n');
    std::vector<String> data = split(line, ';');  // Assumindo que seus dados estão separados por ponto-e-vírgula

    // Verificar se a variável isDeleted é false
    if (data.size() >= 10) {
      data[9].trim();  // Remove espaços em branco ao redor

      if (data[9] == "false") {
        JsonObject obj = array.createNestedObject();

        obj["Id"] = data[0].toInt();
        obj["Nome"] = data[1];
        obj["Tipo"] = data[2];
        obj["Pin"] = data[3];
        obj["ModoOperacao"] = data[4];
        //data[3].trim();
        obj["Valor"] = readDeviceValue(data[3].toInt(), data[4], data[2]);
        obj["DataCriacao"] = data[6];
        obj["DispositivoId"] = data[7].toInt();
        obj["Unidade"] = data[8];
        obj["DataUltimaObs"] = getFormattedTime();


        String unit = obj["DataUltimaObs"].as<String>();

        if (!unit.isEmpty() && unit[unit.length() - 1] == '\r') {
          unit.remove(unit.length() - 1);  // Remove o último caractere se for um carriage return
          obj["DataUltimaObs"] = unit;     // Atualiza o objeto JSON
        }
      }
    }
  }
  file.close();

  String result;
  serializeJson(doc, result);

  return result;
}

bool deleteDeviceById(const char* filePath, int targetID) {
  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar o sistema de ficheiros LittleFS");
    return false;
  }

  File file = LittleFS.open(filePath, "r");
  if (!file) {
    Serial.println("Erro ao abrir o ficheiro para leitura");
    return false;
  }

  String fileContent = "";
  bool updated = false;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.startsWith(String(targetID) + ";")) {
      // Atualiza a variável isDeleted para true
      int lastSemicolonIndex = line.lastIndexOf(';');
      line = line.substring(0, lastSemicolonIndex + 1) + "true";
      updated = true;
    }
    fileContent += line + "\n";
  }
  file.close();

  if (updated) {
    file = LittleFS.open(filePath, "w");
    if (!file) {
      Serial.println("Erro ao abrir o ficheiro para escrita");
      return false;
    }
    file.print(fileContent);
    //debug
    Serial.println("File content-->");
    Serial.println(fileContent);

    file.close();
    return true;
  } else {
    Serial.println("ID não encontrado no ficheiro");
    return false;
  }
}


bool setAtuadorValue(int pin, float value, String tipo) {

  if (tipo == "Analogico") {
    analogWrite(pin, (int)value);  
    return true;
  } else if (tipo == "Digital") {
    digitalWrite(pin, (int)value > 0 ? HIGH : LOW);
    return true;
  }
  
  return false; 
  
}


