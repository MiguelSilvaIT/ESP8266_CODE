#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "FS.h"
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "config.h"
#include "utils.h"

// Inicializa o sistema de arquivos LittleFS
void initFS();

bool handleESPConfig(JsonDocument& doc);


#endif
