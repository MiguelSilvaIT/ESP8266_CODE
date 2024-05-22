#include "sensor.h"
#include "config.h"
#include "time_manager.h"
#include "utils.h"



String addSensor(const char* path, JsonDocument& doc) {
    // Check if the file exists; if not, create the file
    String pin = doc["pin"];
    if (isPinUsed(path, pin)) {
        Serial.println("Pin is already in use");
        return "Pin is already in use";  // Stop adding the new sensor if the pin is used
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

    int lastId = readLastSensorId();  // Read the last used ID
    lastId++;                         // Increment the ID
    doc["id"] = lastId;               // Update the ID in the JSON document

    // Open the file to append
    File file = LittleFS.open(path, "a");
    if (!file) {
        Serial.println("Failed to open file for appending");
        return "Failed to open file for appending";
    }

    // Format the received data into a string
    String dataString = String(doc["id"].as<int>()) + ";" + String((const char*)doc["nome"]) + ";" 
                        + String((const char*)doc["tipo"]) + ";" + String((const char*)doc["pin"]) + ";"
                        + String((const char*)doc["modoOperacao"]) + ";" + String((float)doc["valor"]) + ";"
                        + String((const char*)doc["dtCriacao"]) + ";" + String((int)doc["dispositivoId"]) + ";" 
                        + String((const char*)doc["unidade"]) + ";false";

    // Append the formatted string to the file
    if (file.println(dataString)) {
        Serial.println("Sensor data appended");
        updateLastSensorId(lastId);
        file.close();
        return "Sensor data appended successfully";
    } else {
        Serial.println("Append failed");
        file.close();
        return "Append failed";
    }
}

void readSensor(const char* path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println("End of read");
  file.close();
}

void clearSensorData(const char* path) {
  if (LittleFS.remove(path)) {
    Serial.println("File removed successfully");
  } else {
    Serial.println("Failed to remove file");
  }
}

int readLastSensorId() {

  if (!LittleFS.exists("/last_id.txt")) {
    // Se o arquivo não existir, retorne -1 (ou 0, se você preferir começar do ID 1)
    Serial.println("No ID file found, starting from ID 0");
    return -1;  // Ou return 0 se preferir começar os IDs de 1
  }

  File file = LittleFS.open("/last_id.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading last ID");
    return -1;  // Retorna -1 se não puder abrir o arquivo
  }

  String idStr = file.readStringUntil('\n');
  file.close();

  Serial.print("Last Sensor Id -->");
  Serial.println(idStr);

  return idStr.toInt();  // Converte a string lida para int e retorna
}

void updateLastSensorId(int lastId) {
  File file = LittleFS.open("/last_id.txt", "w");
  if (!file) {
    Serial.println("Failed to open file for updating last ID");
    return;
  }

  file.println(lastId);  // Escreve o novo último ID no arquivo
  file.close();
  Serial.println("Last ID updated successfully to " + String(lastId));
}

String getAllSensorData(const char* path) {
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

                obj["Valor"] = readSensorValue(data[3].toInt(), data[4]);
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




float readSensorValue(int pin, String tipo) {
  if (tipo == "Analogico") {
    // Serial.print("1023 - Valor Analogico Lido: ");
    // Serial.println(1023 - analogRead(pin));
    return 1023 - analogRead(pin);
  } else if (tipo == "Digital") {
    return digitalRead(pin);
  }
  return 0;  // Retorna 0 como default se o tipo de sensor não for reconhecido
}

bool deleteSensorById(const char* filePath, int targetID) {
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
        file.close();
        return true;
    } else {
        Serial.println("ID não encontrado no ficheiro");
        return false;
    }
}



bool updateSensorById(const char* path, int sensorId, const SensorData& newData) {
  File file = LittleFS.open(path, "r+");
  if (!file) {
    Serial.println("Failed to open file for reading and writing");
    return false;
  }

  String output;
  String line = file.readStringUntil('\n');  // Ler e manter o cabeçalho
  output += line + "\n";

  bool updated = false;
  while (file.available()) {
    line = file.readStringUntil('\n');
    std::vector<String> data = split(line, ';');
    if (data.size() < 10) continue;  // Verifica se todas as partes necessárias estão presentes

    int currentId = data[0].toInt();
    if (currentId == sensorId) {
      // Atualiza os dados do sensor
      data[1] = newData.nome;
      data[2] = newData.tipo;
      data[3] = String(newData.pin);
      data[4] = newData.modoOperacao;
      data[8] = newData.unidade;
      data[9] = getFormattedTime();  // Atualiza a data da última observação

      updated = true;
    }

    // Reconstrói a linha
    line = String();
    for (int i = 0; i < data.size(); i++) {
      line += data[i] + (i < data.size() - 1 ? ";" : "");
    }
    output += line + "\n";
  }

  if (!updated) {
    Serial.println("Sensor ID not found for update.");
    file.close();
    return false;
  }

  // Reescreve os dados atualizados no arquivo
  file.seek(0);
  file.write((const uint8_t*)output.c_str(), output.length());
  file.close();
  return true;
}

bool deleteSensorRequest(int sensorId) {
  // Verifique se a conexão WiFi está ativa
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }

  // Obtenha o MAC address
  String macAddress = WiFi.macAddress();

  // Obtenha o IP Central


  // Verifique se o IP Central não está vazio
  if (centralIP.length() == 0) {
    Serial.println("Central IP is empty");
    return false;
  }

  // Formate a URL com o MAC address e o sensor ID
  String url = "http://192.168.58.1:3000/api/sensors/delete";


  // Construir o payload JSON
  //String payload = "{\"macAddress\":\"" + macAddress + "\", \"sensorId\":" + String(sensorId) + "}";


  // Utilizar o método sendPostRequest para enviar a requisição
  if (sendPostRequest("http://192.168.1.41:3000/", "ola")) {
    Serial.println("Requisição de exclusão enviada com sucesso.");
    return true;
  } else {
    Serial.println("Falha ao enviar a requisição de exclusão.");
    return false;
  }
}
