#include "utils.h"


std::vector<String> split(const String& data, char delimiter) {
  std::vector<String> result;
  int start = 0;
  int end = data.indexOf(delimiter);
  while (end != -1) {
    result.push_back(data.substring(start, end));
    start = end + 1;
    end = data.indexOf(delimiter, start);
  }
  result.push_back(data.substring(start));  // Adiciona o último elemento
  return result;
}