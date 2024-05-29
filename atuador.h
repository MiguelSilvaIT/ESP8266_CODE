#ifndef ATUADOR_H
#define ATUADOR_H

#include <ArduinoJson.h>  // Necessário para manipulação de JSON


struct AtuadorData {
    String nome;
    String tipo;
    String modoOperacao;
    String unidade;
    int pin;
};


// Função para ler os dados do atuador de um arquivo
void readAtuador(const char* path);

String getAllAtuadorData(const char* path);

float readAtuadorValue(int pin, String tipo);

bool setAtuadorValue(int pin, float value, String tipo);

bool deleteAtuadorById(const char* path, int atuadorId);


#endif
