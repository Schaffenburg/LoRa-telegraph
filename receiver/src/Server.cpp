#include "credentials.h"
#include "neopixel.h"
#include "Receiver.h"
#include "pages.h"

String convert_utf8_to_iso8859_1(String utf8) {
  String iso8859_1 = "";
  for (int i = 0; i < utf8.length(); i++){
    char c = utf8[i]; // get the character
    if ((c & 0x80) == 0) { // is it a single-byte character?
      iso8859_1 += c; // if so, just append it
    } else if ((c & 0xE0) == 0xC0) { // is it a two-byte sequence?
      // extract the Unicode codepoint represented by the sequence
      int codepoint = ((c & 0x1F) << 6) | (utf8[++i] & 0x3F);
      if (codepoint <= 0xFF) { // is it within the ISO-8859-1 range?
        iso8859_1 += (char) codepoint; // if so, append the equivalent ISO-8859-1 character
      }
      // if not, we simply omit it
    } else {
      // it's a more-than-two-byte sequence, which is definitely outside the ISO-8859-1 range,
      // so we just skip the rest of the sequence
      if ((c & 0xF0) == 0xE0) i++;
      else if ((c & 0xF8) == 0xF0) i += 2;
      // if it's neither, it's a malformed sequence, so we skip it altogether
    }
  }
  return iso8859_1;
}


WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void handleWebClient() {
  server.handleClient();
  webSocket.loop();
}

void handleRoot() {
  server.send(200, "text/html", displayForm);
}

void handleLED() {
  Serial.print("handleLED:");
  if (server.arg("led") == "On") {
    neoPixelOn();
    Serial.println("On");
    server.send(200, "text/html", "Turn ON LED");
  } else if (server.arg("led") == "Off") {
    neoPixelOff();
    Serial.println("Off");
    server.send(200, "text/html", "Turn OFF LED");
  }
  server.send(500, "text/html", "Error in handleLED, no value found " + server.arg("led"));
}

void handleDisplayString() {
  String message;
  Serial.print("handleDisplayString:");
  if (server.hasArg("text")) {
    message = server.arg("text");
    Heltec.display->clear();
    Heltec.display->drawString(0 , 0 , message);
    Heltec.display->display();
    SoftSerial.write(CLEAR_DISPLAY);
    SoftSerial.print(convert_utf8_to_iso8859_1(message));
    Serial.println(message);
    server.send(200, "text/html", "Display: '" + message +"'");
  } else {
    server.send(500, "text/html", "Error in handleDisplayString, no text found");
  }
}

void handleRaw() {
  Serial.print("handleRaw:");
  if (server.hasArg("byte")) {
    int rawbyte = server.arg("byte").toInt();
    SoftSerial.write(rawbyte);
    Serial.println(String(rawbyte));
    server.send(200, "text/html", "Display byte: 0x" + String(rawbyte, HEX));
  } else {
    server.send(500, "text/html", "Error in handleRaw, no byte found");
  }
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
    SoftSerial.print(message);
    Serial.print("WS:");
    Serial.println(message);
  }
}

void setupWifi() {
  Heltec.display->clear();
  Heltec.display->drawString(0 , 0 , "Connecting to WiFI");
  Heltec.display->display();

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

  Heltec.display->clear();
  Heltec.display->drawString(0 , 0 , "Conntected to");
  Heltec.display->drawString(0 , 12 , ssid);
  Heltec.display->drawString(0 , 24 , "IP address:");
  Heltec.display->drawString(0 , 36 , WiFi.localIP().toString().c_str());
  Heltec.display->display();

  SoftSerial.print(WiFi.localIP().toString());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host))
    Serial.println("Error setting up MDNS responder, MDNS will not be used");
  else
    Serial.println("mDNS responder started");
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/StringForm", HTTP_POST, handleDisplayString);
  server.on("/RawForm", HTTP_POST, handleRaw);
  server.on("/LedForm", HTTP_POST, handleLED);
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
