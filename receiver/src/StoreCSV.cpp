#include "StoreCSV.h"

std::vector<DataEntry> entries;
int current_index;

void handleCSVUpload() {
  HTTPUpload& upload = server.upload();
  static File file;

  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Upload Start: %s\n", upload.filename.c_str());

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS. Formatting...");
        if (LittleFS.format()) {
            Serial.println("Format successful. Please try again.");
        } else {
            Serial.println("Format failed. Please check your hardware or partition scheme.");
        }
        return;
    }

    // Open the file on LittleFS
    file = LittleFS.open("/uploaded.csv", "w");

    if (!file) {
      Serial.println("Error opening file for writing");
      return;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.printf("Upload Data: %s\n", upload.filename.c_str());

    // Write the current chunk of data to the file
    file.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.printf("Upload End: %s, %u B\n", upload.filename.c_str(), upload.totalSize);

    // Close the file
    file.close();

    ESP.restart();
  }
}

void readCSVfromFS() {
  // Initialize LittleFS
  if (!LittleFS.begin()) {
      Serial.println("An Error has occurred while mounting LittleFS");
      return;
  }

  // Check if the file exists in the filesystem
  if (!LittleFS.exists("/uploaded.csv")) {
    Serial.println("No CSV file found. Returning.");
    return;
  }

  // Open file for reading
  File file = LittleFS.open("/uploaded.csv", "r");

  if (!file) {
    Serial.println("Failed to open CSV file for reading.");
    return;
  }

  Serial.println("Reading CSV file...");

  // Clear previous entries
  entries.clear();

  // Read line by line
  bool isHeader = true;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim(); // remove potential CR/LF

    // Ignore the header row
    if (isHeader) {
        isHeader = false;
        continue;
    }

    // Split line
    int col1pos = line.indexOf(";");
    int col2pos = line.indexOf(";", col1pos + 1);
    int col3pos = line.indexOf(";", col2pos + 1);
    int col4pos = line.indexOf(";", col3pos + 1);

    if (col3pos != -1) {
        String dateStr = line.substring(0, col1pos);
        String timeStr = line.substring(col1pos + 1, col2pos);
        String name = line.substring(col3pos + 1);
        if (col4pos > col3pos)
            name = line.substring(col3pos + 1, col4pos);

        struct tm timeinfo;
        strptime(dateStr.c_str(), "%Y-%m-%d  ", &timeinfo);
        int hour = timeStr.substring(0,2).toInt();
        int min = timeStr.substring(2).toInt();

        timeinfo.tm_hour = hour;
        timeinfo.tm_min = min;
        timeinfo.tm_sec = 0;
        time_t timestamp = mktime(&timeinfo);

        entries.push_back(DataEntry(timestamp, name.c_str()));
    }
  }

  // Dump entries to Serial
  for (const auto& entry : entries) {
    struct tm *tm_info = localtime(&entry.timestamp);
    char buffer[13];
    strftime(buffer, sizeof(buffer), "%d.%m. %H:%M", tm_info);
    Serial.printf("%s (%ld): %s\n", buffer, entry.timestamp, entry.name.c_str());
  }

  file.close();

  current_index = 0;
  time_t next_event_ts = 0;
  time_t now;
  time(&now);
  while (entries[current_index].timestamp < now) {
    current_index++;
  }
  Serial.printf("Current TS: %ld, Current index: %i, next event @ %ld: %s\n", now, current_index, entries[current_index].timestamp, entries[current_index].name.c_str());
}
