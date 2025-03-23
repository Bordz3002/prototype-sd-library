#ifndef PROTOSD_H
#define PROTOSD_H
#include <Arduino.h>
#include <formatfloat.h>
#include <protoVECTOR.h>
#include <ctype.h>
#include <SPI.h>
#include <SD.h>
class protoSD{
private:
  File data;
  int pin;
  protoVECTOR vector;
  formatfloat formatter;
public:
    protoSD(int card_pin);
    int vector_size();
    float empty_float();
    String empty_string();
    char emtpy_char();
    int empty_int();
    void begin();
    bool find(const char* path);
    void create_directory(String path);
    void listfiles(String path);
    void remove_file(String path, String filename);
    void clearfile(String path, String filename);
    void write_string(String path, String filename, String content);
    String dump_string(String path, String filename, int index);
    void write_float(String path, String filename, float input);
    float dump_float(String path, String filename, int index);
    void write_char(String path, String filename, char input);
    char dump_char(String path, String filename, int index);
    void write_int(String path, String filename, int input);
    int dump_int(String path, String filename, int index);
};
#endif
