#ifndef SENSOR_H
#define SENSOR_H

#include <ArduinoJson.h>
#include <DHT.h>


  // Necessário para manipulação de JSON


struct SensorData {
    String nome;
    String tipo;
    String modoOperacao;
    String unidade;
    int pin;
};

// Função para adicionar dados de sensor ao arquivo
String addSensor(const char* path, JsonDocument& doc);

// Função para ler os dados do sensor de um arquivo
void readSensor(const char* path);

void clearSensorData(const char* path);

int readLastSensorId();

void updateLastSensorId(int lastId);

String getAllSensorData(const char* path);

float readSensorValue(int pin, String modoOperacao, String tipo);

bool updateSensorById(const char* path, int sensorId, const SensorData& newData);

bool deleteSensorById(const char* path, int sensorId);

bool deleteSensorRequest(int sensorId);

float lerDHT11(DHT dht);


#endif
