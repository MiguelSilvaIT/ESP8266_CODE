#include "sensor.h"
#include "config.h"
#include "time_manager.h"
#include "utils.h"

#define READ_INTERVAL 200

 

unsigned long lastReadTime = 0;



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
        //data[3].trim();
        obj["Valor"] = readSensorValue(data[3].toInt(), data[4], data[2]);
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



float readSensorValue(int pin, String modoOperacao, String tipo) {



  if (modoOperacao == "Analogico") 
  {
    float analogValue = 1023 - analogRead(pin);
    Serial.print("Analog read value PIN-->: ");
    Serial.println(pin);
    return analogValue;
  } 
  else if (modoOperacao == "Digital") {

    tipo.trim();
    if (tipo == "DHT11") {
      // DHT dht(5, DHT11);  // Inicializa o sensor DHT11 no pino especificado
      // dht.begin();
      // delay(2000);
      // Serial.println("Lendo temperatura do DHT11...");
      // float temp = dht.readTemperature();  // Lê a temperatura

      // // Verifica se a leitura falhou e tenta novamente se falhou
      // if (isnan(temp)) {
      //   Serial.println("Falha na leitura do sensor DHT11, tentando novamente...");
      //   temp = dht.readTemperature();
      //   if (isnan(temp)) {
      //     Serial.println("Falha na segunda tentativa de leitura do DHT11");
      //     return -1;
      //   }
      // }

      //return temp;
    }
    int digitalValue = digitalRead(pin);
    Serial.print("Digital read value: ");
    Serial.println(digitalValue);
    return digitalValue;
  }
  Serial.println("ModoOperacao not recognized, returning default value 0");
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



float lerDHT11(DHT dht) {
  Serial.print("Lendo temperatura do DHT11...");
  float temp = dht.readTemperature();
  Serial.println(temp);

  // Verifica se a leitura falhou e tenta novamente se falhou
  if (isnan(temp)) {
    Serial.println("Falha na leitura do sensor DHT11, tentando novamente...");
    temp = dht.readTemperature();
    if (isnan(temp)) {
      Serial.println("Falha na segunda tentativa de leitura do DHT11");
    }
  }

  return temp;
}
