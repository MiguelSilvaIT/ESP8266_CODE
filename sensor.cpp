#include "sensor.h"
#include "config.h"
#include "time_manager.h"
#include "utils.h"



String addSensor(const char* path, JsonDocument& doc) {
  // Check if the file exists; if not, create the file


  if (!LittleFS.exists(path)) {

    int pin = doc["pin"].as<int>();
    if (isPinUsed(path, pin)) {
      Serial.println("Pin is already in use");
      return "Pin is already in use";  // Stop adding the new actuator if the pin is used
    }


    File file = LittleFS.open(path, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return "Failed to open file for writing";
    }
    file.println("ID;Nome;Tipo;Pin;ModoOperacao;Valor;DataCriacao;DispositivoId;UnidadeId");
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
  String dataString = String(doc["id"].as<int>()) + ";" + String((const char*)doc["nome"]) + ";" + String((const char*)doc["tipo"]) + ";" + String((const char*)doc["pin"]) + ";" + String((const char*)doc["modoOperacao"]) + ";" + String((float)doc["valor"]) + ";" + String((const char*)doc["dtCriacao"]) + ";" + String((int)doc["dispositivoId"]) + ";" + String((const char*)doc["unidadeId"]);

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
  while (file.available()) { Serial.write(file.read()); }
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

  //Serial.println("Inicio getAllSensorData");

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "{}";  // Retorna um objeto JSON vazio em caso de falha
  }


  //Ignora o cabeçalho
  if (file.available()) {
    file.readStringUntil('\n');
  }


  DynamicJsonDocument doc(2048);  // Ajuste o tamanho conforme necessário
  JsonArray array = doc.to<JsonArray>();



  while (file.available()) {


    String line = file.readStringUntil('\n');
    std::vector<String> data = split(line, ';');  // Assumindo que seus dados estão separados por ponto-e-vírgula
    JsonObject obj = array.createNestedObject();


    obj["Id"] = data[0].toInt();
    obj["Nome"] = data[1];
    obj["Tipo"] = data[2];
    obj["Pin"] = data[3];
    obj["ModoOperacao"] = data[4];

    // Serial.print("data[3]-->");
    // Serial.println(data[3]);

    // Serial.print("data[4]-->");
    // Serial.println(data[4]);

    obj["Valor"] = readSensorValue(data[3].toInt(), data[4]);
    obj["DataCriacao"] = data[6];
    obj["DispositivoId"] = data[7].toInt();
    obj["Unidade"] = data[8];
    obj["DataUltimaObs"] = getFormattedTime();


    String unit = obj["DataUltimaObs"].as<String>();

    if (!unit.isEmpty() && unit[unit.length() - 1] == '\r') {
      unit.remove(unit.length() - 1);  // Removes the last character if it's a carriage return
      obj["DataUltimaObs"] = unit;     // Update the JSON object
    }
  }
  file.close();

  String result;
  serializeJson(doc, result);

  //Serial.println("Fim so getAllSensorData");
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

bool deleteSensorById(const char* path, int sensorId) {
  File file = LittleFS.open(path, "r+");
  if (!file) {
    Serial.println("Failed to open file for reading and writing");
    return false;
  }

  String output;
  String line;
  bool found = false;

  while (file.available()) {
    line = file.readStringUntil('\n');
    // Remove '\r' se estiver presente no final da linha
    if (line.endsWith("\r")) {
      line = line.substring(0, line.length() - 1);
    }

    int currentId = line.substring(0, line.indexOf(';')).toInt();  // Assumindo que o ID seja sempre o primeiro item antes do ';'
    if (currentId != sensorId) {
      output += line + "\n";
    } else {
      found = true;  // Encontrou a linha a ser deletada
    }
  }

  file.close();  // Fecha o arquivo para resetar o cursor antes de reabri-lo para escrita

  if (!found) {
    return false;  // Se não encontrar o ID, retorna falso
  }

  // Reabre o arquivo para escrita e limpa seu conteúdo
  file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }

  file.print(output);  // Escreve a nova saída sem a linha do ID
  file.close();        // Fecha o arquivo após a operação de escrita
  return true;
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
