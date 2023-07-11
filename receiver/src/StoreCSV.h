// StoreCSV.h
#ifndef STORE_CSV_H
#define STORE_CSV_H

#include <LittleFS.h>
#include <vector>
#include <sstream>
#include "Receiver.h"

struct DataEntry {
    time_t timestamp;
    std::string name;

    DataEntry(time_t t, std::string n): timestamp(t), name(n) {}
};

extern std::vector<DataEntry> entries;
extern int current_index;

void handleCSVUpload();
void readCSVfromFS();

#endif
