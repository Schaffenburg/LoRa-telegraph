// StoreCSV.h
#ifndef STORE_CSV_H
#define STORE_CSV_H

#include <LittleFS.h>
#include <vector>
#include <sstream>
#include "heltec.h"

struct DataEntry {
    std::string timestamp;
    std::string name;

    DataEntry(std::string t, std::string n): timestamp(t), name(n) {}
};

extern std::vector<DataEntry> entries;

void handleCSVUpload();
void readCSVfromFS();

#endif
