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

// Função para adicionar dados de atuador ao arquivo
void addAtuador(const char* path, JsonDocument& doc);

// Função para ler os dados do atuador de um arquivo
void readAtuador(const char* path);

void clearAtuadorData(const char* path);

int readLastAtuadorId();

void updateLastAtuadorId(int lastId);

String getAllAtuadorData(const char* path);


float readAtuadorValue(char* pin, String tipo);

bool updateAtuadorValues(const char* path);

bool updateAtuadorById(const char* path, int atuadorId, const AtuadorData& newData);

#endif
