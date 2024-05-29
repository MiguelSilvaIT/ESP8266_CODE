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



// Função para ler os dados do sensor de um arquivo
void readSensor(const char* path);




String getAllSensorData(const char* path);

float readSensorValue(int pin, String modoOperacao, String tipo);


bool deleteSensorById(const char* path, int sensorId);


float lerDHT11(DHT dht);


#endif
