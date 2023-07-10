#include "credentials.h"
#include "Server.h"
#include "pages.h"
#include "StoreCSV.h"
#include <WebServer.h>

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void handleWebClient() {
  server.handleClient();
  webSocket.loop();
}

void handleRoot() {
  server.send(200, "text/html", displayForm);
}

void handleDisplayString() {
  String message;
  if (server.hasArg("text")) {
    message = server.arg("text");
    Heltec.display->clear();
    Heltec.display->drawString(0 , 0 , message);
    Heltec.display->display();
  }
  server.send(200, "text/html", "Submitted");
}

void handleOTA() {
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", loginIndex);
}

void handleServerIndex() {
  server.sendHeader("Connection", "close");
  if(server.arg("k") != OTA_PWD)
    server.send(400, "text/html", "invalid credentials.");
  else
    server.send(200, "text/html", serverIndex);
}

void handleUpdate() {
  server.sendHeader("Connection", "close");
  if(server.arg("k") != OTA_PWD) {
    server.send(400, "text/html", "invalid credentials.");
  } else {
    server.send(200, "text/html", serverIndex);
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }
}

void handleUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

void CSVUploadPage() {
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", csvUploadPage);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){
    String message = String((char*) payload);
    Heltec.display->clear();
    Heltec.display->drawString(0 , 0 , message);
    Heltec.display->display();
  }
}

void setupWifi() {
  // Connect to WiFi network
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host))
    Serial.println("Error setting up MDNS responder, MDNS will not be used");
  else
    Serial.println("mDNS responder started");
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/displayString", HTTP_POST, handleDisplayString);
  server.on("/OTA", HTTP_GET, handleOTA);
  server.on("/serverIndex", HTTP_GET, handleServerIndex);
  server.on("/uploadCSV", HTTP_GET, CSVUploadPage);
  server.on("/uploadCSV", HTTP_POST, []() {
    server.send(200, "text/plain", "CSV Uploaded Successfully");
  }, handleCSVUpload);

  server.on("/update", HTTP_POST, []() {
    handleUpdate();
  }, handleUpload);
  server.begin();

  webSocket.onEvent(webSocketEvent);
  webSocket.begin();

}
