#include "atuador.h"
#include "filesystem.h"
#include "config.h"
#include "time_manager.h"
#include "utils.h"


void addAtuador(const char* path, JsonDocument& doc) {
  // Verifica se o arquivo existe; se não, cria o arquivo

  int lastId = readLastAtuadorId();  // Lê o último ID usado
  lastId++;                         // Incrementa o ID
  doc["id"] = lastId;               // Atualiza o ID no documento JSON



  if (!LittleFS.exists(path)) {
    File file = LittleFS.open(path, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }
    file.println("ID;Nome;Tipo;Pin;ModoOperacao;Valor;DataCriacao;DispositivoId;UnidadeId;UltimaObs");
    file.close();  // Fecha o arquivo recém-criado
  }

  // Abre o arquivo para anexar (append)
  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  // Formata os dados recebidos em uma string
  String dataString = String(doc["id"].as<int>()) + ";" + String((const char*)doc["nome"]) + ";" + 
                       String((const char*)doc["tipo"]) + ";" + String((const char*)doc["pin"]) + ";" +
                       String((const char*)doc["modoOperacao"]) + ";" + String((float)doc["valor"]) + ";" +
                       String((const char*)doc["dtCriacao"]) + ";" + String((int)doc["dispositivoId"]) + ";" +
                       String((const char*)doc["unidadeId"]) + ";-1";


  // Anexa a string formatada ao arquivo
  if (file.println(dataString)) {
    Serial.println("Atuador data appended");
    updateLastAtuadorId(lastId);
  } else {
    Serial.println("Append failed");
  }

  file.close();  // Fecha o arquivo após a operação de anexar
}

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

void clearAtuadorData(const char* path) {
  if (LittleFS.remove(path)) {
    Serial.println("File removed successfully");
  } else {
    Serial.println("Failed to remove file");
  }
}

int readLastAtuadorId() {

  if (!LittleFS.exists("/last_atuador_id.txt")) {
    // Se o arquivo não existir, retorne -1 (ou 0, se você preferir começar do ID 1)
    Serial.println("No ID file found, starting from ID 0");
    return -1;  // Ou return 0 se preferir começar os IDs de 1
  }

  File file = LittleFS.open("/last_atuador_id.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading last ID");
    return -1;  // Retorna -1 se não puder abrir o arquivo
  }

  String idStr = file.readStringUntil('\n');
  file.close();

  Serial.print("Last Atuador Id -->");
  Serial.println(idStr);

  return idStr.toInt();  // Converte a string lida para int e retorna
}

void updateLastAtuadorId(int lastId) {
  File file = LittleFS.open("/last_atuador_id.txt", "w");
  if (!file) {
    Serial.println("Failed to open file for updating last ID");
    return;
  }

  file.println(lastId);  // Escreve o novo último ID no arquivo
  file.close();
  Serial.println("Last ID updated successfully to " + String(lastId));
}

String getAllAtuadorData(const char* path) {
  
      if (!updateAtuadorValues(atuadores_path)) {
        Serial.println("Failed to update atuador values");
        return "{Failed to update atuador values}";  // Retorna um objeto JSON vazio em caso de falha
    }
  
  
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
    obj["Valor"] = data[5].toFloat();
    obj["DataCriacao"] = data[6];
    obj["DispositivoId"] = data[7].toInt();
    obj["Unidade"] = data[8];
    obj["DataUltimaObs"] = data[9];
    
  

    String unit = obj["DataUltimaObs"].as<String>();

    if (!unit.isEmpty() && unit[unit.length() - 1] == '\r') {
      unit.remove(unit.length() - 1); // Removes the last character if it's a carriage return
      obj["DataUltimaObs"] = unit; // Update the JSON object
}
  }
  file.close();

  String result;
  serializeJson(doc, result);
  return result;
}


float readAtuadorValue(int pin, String tipo) {
    if (tipo == "Analogico") {
        return 1023 - analogRead(pin);
    } else if (tipo == "Digital") {
        return digitalRead(pin);
    }
    return 0;  // Retorna 0 como default se o tipo de atuador não for reconhecido
}

bool updateAtuadorValues(const char* path) {
    File file = LittleFS.open(path, "r+");
    if (!file) {
        Serial.println("Failed to open file for reading and writing");
        return false;
    }

    String output = "";
    String line = file.readStringUntil('\n');  // Ler e manter o cabeçalho
    output += line + "\n";

    while (file.available()) {
        line = file.readStringUntil('\n');
        std::vector<String> data = split(line, ';');
     

        int pin = data[3].toInt(); // Assumindo que o pin está na coluna 4
        String tipo = data[4]; // Assumindo que o tipo está na coluna 5
        float novoValor = readAtuadorValue(pin, tipo); // Lê o novo valor do atuador

        

        // Atualiza a linha com o novo valor
        data[5] = String(novoValor);
        data[9] = getFormattedTime();
        for (int i = 0; i < data.size(); i++) {
            output += data[i];
            if (i < data.size() - 1) output += ";";
        }
        output += "\n";
    }

    file.seek(0);  // Retorna ao início do arquivo
    file.write((const uint8_t*)output.c_str(), output.length()); // Escreve os dados atualizados
    file.close();  // Fecha o arquivo
    return true;
}


bool updateAtuadorById(const char* path, int atuadorId, const AtuadorData& newData) {
    File file = LittleFS.open(path, "r+");
    if (!file) {
        Serial.println("Failed to open file for reading and writing");
        return false;
    }

    String output;
    String line = file.readStringUntil('\n'); // Ler e manter o cabeçalho
    output += line + "\n";

    bool updated = false;
    while (file.available()) {
        line = file.readStringUntil('\n');
        std::vector<String> data = split(line, ';');
        if (data.size() < 10) continue; // Verifica se todas as partes necessárias estão presentes

        int currentId = data[0].toInt();
        if (currentId == atuadorId) {
            // Atualiza os dados do atuador
            data[1] = newData.nome;
            data[2] = newData.tipo;
            data[3] = String(newData.pin);
            data[4] = newData.modoOperacao;
            data[8] = newData.unidade;
            data[9] = getFormattedTime(); // Atualiza a data da última observação

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
        Serial.println("Atuador ID not found for update.");
        file.close();
        return false;
    }

    // Reescreve os dados atualizados no arquivo
    file.seek(0);
    file.write((const uint8_t*)output.c_str(), output.length());
    file.close();
    return true;
}


