#include "atuador.h"
#include "config.h"
#include "time_manager.h"
#include "utils.h"




//Lê o ficheiro dos atuadores
void readAtuador(const char* path) {
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



String getAllAtuadorData(const char* path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "{}";  // Retorna um objeto JSON vazio em caso de falha
    }

    // Ignora o cabeçalho
    if (file.available()) {
        file.readStringUntil('\n');
    }

    DynamicJsonDocument doc(2048);  
    JsonArray array = doc.to<JsonArray>();

    while (file.available()) {
        String line = file.readStringUntil('\n');
        std::vector<String> data = split(line, ';');  

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

                obj["Valor"] = readAtuadorValue(data[3].toInt(), data[4]);
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


float readAtuadorValue(int pin, String tipo) {
  if (tipo == "Analogico") {
    // Serial.print("Leitura Analogica do PIN --> ");
    // Serial.println(pin);
    return 1023 - analogRead(pin);
  } else if (tipo == "Digital") {
    // Serial.print("Leitura Digital do PIN --> ");
    // Serial.println(pin);
    return digitalRead(pin);
  }
  return 0;  // Retorna 0 como default se o tipo de atuador não for reconhecido
}

bool setAtuadorValue(int pin, float value, String tipo) {
  Serial.print("Setting actuator value on pin: ");
  Serial.print(pin);
  Serial.print(" with value: ");
  Serial.print(value);
  Serial.print(" and type: ");
  Serial.println(tipo);

  if (tipo == "Analogico") {
    analogWrite(pin, (int)value);  // ESP8266 does support analogWrite on certain pins
    return true;
  } else if (tipo == "Digital") {
    digitalWrite(pin, (int)value > 0 ? HIGH : LOW);
    return true;
  }
  
  return false;  // If type is neither 'Analogico' nor 'Digital'
  
}



bool deleteAtuadorById(const char* filePath, int targetID) {
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

