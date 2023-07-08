#include "StoreCSV.h"
#include "Server.h"

std::vector<DataEntry> entries;

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
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim(); // remove potential CR/LF

    // Parse line
    int pos = line.indexOf(",");
    if (pos != -1) {
      String timestamp = line.substring(0, pos);
      String name = line.substring(pos + 1);

      // Add to entries
      entries.push_back(DataEntry(timestamp.c_str(), name.c_str()));
    }
  }

  // Dump entries to Serial
  for (const auto& entry : entries) {
    Serial.printf("Timestamp: %s, Name: %s\n", entry.timestamp.c_str(), entry.name.c_str());
  }

  // Close the file
  file.close();
}
