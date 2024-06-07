#include "network.h"
#include "filesystem.h"  // Para funções do sistema de arquivos
#include "config.h"
#include "utils.h"


void setupWiFi() {
  WiFiManager wifiManager;


  if (!wifiManager.autoConnect("ESP8266AP")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
  }
  Serial.println("Connected to WiFi!");
}



void configureWebServer(AsyncWebServer& server) {

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");


  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/index.html", "text/html", false);
  });


  server.on(
    "/addDevice", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      static String jsonData;
      for (size_t i = 0; i < len; i++) {
        jsonData += (char)data[i];
      }
      if (index + len == total) {
        Serial.println("-------------------------");
        Serial.println(jsonData);
        Serial.println("-------------------------");

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, jsonData);

        if (error) {
          Serial.print("Erro de serialização:");
          Serial.println(error.c_str());
          request->send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
        }
        jsonData = "";

        
        
        const char* path = doc["tipoDispositivo"] == "sensor" ? sensors_path : atuadores_path;


        String result = addDevice(path, doc);

        if (result == "Pin is already in use") {
          request->send(409, "application/json", result);
        } else if (result == "Failed to open file for writing" || result == "Failed to open file for appending" || result == "Append failed") {
          request->send(500, "application/json", result);
        } else {
          request->send(200, "application/json", result);
        }
      }
    });



  server.on("/ip", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", WiFi.localIP().toString());
  });


  //Obter todos os sensores
  server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest* request) {
    String sensorData = getAllDeviceData(sensors_path);  // Substitua pelo caminho correto do arquivo
    request->send(200, "application/json", sensorData);
  });

  server.on("/deleteSensor", HTTP_DELETE, [](AsyncWebServerRequest* request) {
    if (!request->hasParam("id")) {
      request->send(400, "application/json", "{\"message\":\"Sensor ID missing\"}");
      return;
    }
    int sensorId = request->getParam("id")->value().toInt();  // Extract the sensor ID from the request

    if (deleteDeviceById(sensors_path, sensorId)) {
      request->send(200, "application/json", "{\"message\":\"Sensor deleted successfully\"}");
    } else {
      request->send(404, "application/json", "{\"message\":\"Sensor not found\"}");
    }
  });


  //ATUADORES

  server.on("/atuadores", HTTP_GET, [](AsyncWebServerRequest* request) {
    String atuadorData = getAllDeviceData(atuadores_path);  // Substitua pelo caminho correto do arquivo
    request->send(200, "application/json", atuadorData);
  });



  server.on("/toggleAtuador", HTTP_POST, [](AsyncWebServerRequest* request) {
    int atuadorPin = 0;
    int novoValor = 0;
    String modoOperacao = "";

    if (request->hasParam("pin") && request->hasParam("valor") && request->hasParam("modoOperacao")) {
      atuadorPin = request->getParam("pin")->value().toInt();
      novoValor = request->getParam("valor")->value().toInt();
      modoOperacao = request->getParam("modoOperacao")->value();

      // Call function to toggle actuator state
      if (setAtuadorValue(atuadorPin, novoValor, modoOperacao)) {
        request->send(200, "application/json", "{\"message\":\"Actuator state updated successfully\"}");
      } else {
        request->send(500, "application/json", "{\"message\":\"Failed to update actuator state\"}");
      }
    } else {
      request->send(400, "application/json", "{\"message\":\"Missing parameters\"}");
    }
  });



  server.on("/deleteAtuador", HTTP_DELETE, [](AsyncWebServerRequest* request) {
    if (!request->hasParam("id")) {
      request->send(400, "application/json", "{\"message\":\"Atuador ID missing\"}");
      return;
    }
    int atuadorId = request->getParam("id")->value().toInt();  // Extract the atuador ID from the request

    if (deleteDeviceById(atuadores_path, atuadorId)) {
      request->send(200, "application/json", "{\"message\":\"Atuador deleted successfully\"}");
    } else {
      request->send(404, "application/json", "{\"message\":\"Atuador not found\"}");
    }
  });

  //--FIM SECÇÃO ATUADORES--


  //--DISPOSITIVO--

  server.on(
    "/esp/config", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      static String jsonData;
      for (size_t i = 0; i < len; i++) {
        jsonData += (char)data[i];
      }
      if (index + len == total) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, jsonData);
        if (error) {
          request->send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
          jsonData = "";
          return;
        }


        jsonData = "";  // Clear the buffer for the next message

        // Handle the configuration data
        if (!handleESPConfig(doc)) {
          request->send(500, "application/json", "{\"message\":\"Failed to store configuration\"}");
        } else {
          request->send(200, "application/json", "{\"message\":\"Configuration saved successfully\"}");
          initCentralIP();
        }
      }
    });


  
  
  server.on("/esp", HTTP_GET, [](AsyncWebServerRequest* request) {
    
    String configData = readESPConfig();
    Serial.print("configData-->");
    Serial.println(configData);
    
    if (configData == "") {
      request->send(500, "application/json", "{\"message\":\"Failed to read configuration\"}");
    } else {
      request->send(200, "application/json", configData);
    }
  });

  server.onNotFound([](AsyncWebServerRequest* request) {
    Serial.println("OPTIONS PRE-FLIGHT REQUEST RECEIVED");
    Serial.println(request->method());
    if (request->method() == 64) {
      request->send(200);
    } else {
      request->send(404, "application/json", "{\"message\":\"Chegou mas não encontrei nada\"}");
    }
  });

  server.serveStatic("/", LittleFS, "/");
}




void postAllData(String url, const char* configPath, const char* sensorPath, const char* actuatorPath) {
  // Preparar o corpo da requisição com os dados JSON
  String jsonData = combineAllDataToJson(configPath, sensorPath, actuatorPath);

  // Utilizar o método sendPostRequest para enviar a requisição
  if (sendPostRequest(url.c_str(), jsonData)) {
    Serial.println("Dados enviados com sucesso.");
  } else {
    Serial.println("Falha ao enviar os dados.");
  }
}
