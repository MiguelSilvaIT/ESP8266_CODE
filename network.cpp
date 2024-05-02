#include "network.h"
#include "filesystem.h"  // Para funções do sistema de arquivos
#include "sensor.h"
#include "atuador.h"
#include "config.h"

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



  //ADICIONAR SENSOR
  server.on(

    "/addSensor", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      // Cria um documento JSON para armazenar os dados recebidos

      static String jsonData;  // Use static to preserve the value across calls
      for (size_t i = 0; i < len; i++) {
        jsonData += (char)data[i];
      }
      if (index + len == total) {  // Check if this is the last chunk
        // Now jsonData contains the complete JSON string

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
        jsonData = "";  // Clear the jsonData for the next message

        addSensor(sensors_path, doc);
      }


      readSensor(sensors_path);



      AsyncWebServerResponse* response = request->beginResponse(200, "application/json", "{\"message\":\"Sensor added successfully\"}");

      //addCORSHeaders(response);
      request->send(response);
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

  server.on("/clearSensorData", HTTP_DELETE, [](AsyncWebServerRequest* request) {
    clearSensorData(sensors_path);  // Chama a função para limpar os dados
    request->send(200, "application/json", "{\"message\":\"Sensor data cleared successfully\"}");
  });

  server.on(
    "/sensors", HTTP_PUT, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      static String jsonData;
      int sensorId;

      if (request->hasParam("id", true)) {
        sensorId = request->getParam("id", true)->value().toInt();
      } else {
        request->send(404, "application/json", "{\"message\":\"Sensor ID not found\"}");
      }
      for (size_t i = 0; i < len; i++) {
        jsonData += (char)data[i];
      }
      if (index + len == total) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, jsonData);
        if (error) {
          request->send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
          return;
        }
        jsonData = "";

        SensorData newData = {
          doc["nome"].as<String>(),
          doc["tipo"].as<String>(),
          doc["modoOperacao"].as<String>(),
          doc["unidade"].as<String>(),
          doc["pin"].as<int>(),
        };

        if (updateSensorById(sensors_path, sensorId, newData)) {
          request->send(200, "application/json", "{\"message\":\"Sensor updated successfully\"}");
        } else {
          request->send(500, "application/json", "{\"message\":\"Failed to update sensor\"}");
        }
      }
    });

  server.on(
    "/put", HTTP_PUT, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      Serial.println("Received PUT request");
      // Restante do código...
    });


  //Obter todos os sensores
  server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest* request) {
    String sensorData = getAllSensorData(sensors_path);  // Substitua pelo caminho correto do arquivo
    request->send(200, "application/json", sensorData);
  });


  //ATUADORES

  server.on("/atuadores", HTTP_GET, [](AsyncWebServerRequest* request) {
    String atuadorData = getAllAtuadorData(atuadores_path);  // Substitua pelo caminho correto do arquivo
    request->send(200, "application/json", atuadorData);
  });

  server.on(
    "/atuadores", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      // Cria um documento JSON para armazenar os dados recebidos

      static String jsonData;  // Use static to preserve the value across calls
      for (size_t i = 0; i < len; i++) {
        jsonData += (char)data[i];
      }
      if (index + len == total) {  // Check if this is the last chunk
        // Now jsonData contains the complete JSON string

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
        jsonData = "";  // Clear the jsonData for the next message

        addAtuador(atuadores_path, doc);
      }


      readAtuador(atuadores_path);



      AsyncWebServerResponse* response = request->beginResponse(200, "application/json", "{\"message\":\"Atuador added successfully\"}");

      //addCORSHeaders(response);
      request->send(response);
    });

  server.on("/clearAtuadorData", HTTP_DELETE, [](AsyncWebServerRequest* request) {
    clearAtuadorData(atuadores_path);  // Chama a função para limpar os dados
    request->send(200, "application/json", "{\"message\":\"Atuador data cleared successfully\"}");
  });

  server.on("/deleteSensor", HTTP_DELETE, [](AsyncWebServerRequest* request) {
    if (!request->hasParam("id")) {
      request->send(400, "application/json", "{\"message\":\"Sensor ID missing\"}");
      return;
    }
    int sensorId = request->getParam("id")->value().toInt();  // Extract the sensor ID from the request

    if (deleteSensorById(sensors_path, sensorId)) {
      request->send(200, "application/json", "{\"message\":\"Sensor deleted successfully\"}");
    } else {
      request->send(404, "application/json", "{\"message\":\"Sensor not found\"}");
    }
  });





  server.serveStatic("/", LittleFS, "/");
}
