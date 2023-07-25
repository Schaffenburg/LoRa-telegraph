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
String ota_pw;

void handleWebClient() {
  server.handleClient();
  webSocket.loop();
}

void handleRoot() {
  server.send(200, "text/html", displayForm);
}

void tickerPage() {
  server.send(200, "text/html", tickerForm);
}

void pickerPage() {
  server.send(200, "text/html", colorPickerIndex);
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

std::vector<String> ticker_lines;
uint32_t ticker_timeout;

void handleTicker() {
  Serial.print("handleTicker:");
  if (server.hasArg("toggle") && server.arg("toggle") == "on") {
    ticker_lines.clear();
    if (server.hasArg("Line1")) {
      ticker_lines.push_back(server.arg("Line1"));
    }
    if (server.hasArg("Line2")) {
      ticker_lines.push_back(server.arg("Line2"));
    }
    if (server.hasArg("Line3")) {
      ticker_lines.push_back(server.arg("Line3"));
    }
    if (server.hasArg("timeout_ms")) {
      ticker_timeout = server.arg("timeout_ms").toInt();
    }
  } else {
    ticker_lines.clear();
  }
  server.send(200, "text/html", "Submitted tickerstrings");
}


void handleTickerEvent() {
    time_t now;

    if (ticker_lines.empty()) {
      return;
    }

    static unsigned long lastLoopTime = millis();
    static int idx = 0;
    if(millis() - lastLoopTime < ticker_timeout) {
        return;
    }
    lastLoopTime = millis();

    if (idx < ticker_lines.size())
    {
      SoftSerial.write(0x0A);
      SoftSerial.write(0x09);
      SoftSerial.write(0x0D);
      SoftSerial.print(ticker_lines[idx]);
      idx++;

    } else {
      idx = 0;
    }
}

void handleRaw() {
  Serial.print("handleRaw:");
  if (server.hasArg("byte")) {
    int rawbyte = server.arg("byte").toInt();
    SoftSerial.write(rawbyte);
    Serial.println(String(rawbyte));
    server.send(200, "text/html", "Display byte: 0x" + String(rawbyte, HEX));
    currentCharPos = 0;
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
  if(server.arg("k") != ota_pw)
    server.send(400, "text/html", "invalid credentials.");
  else
    server.send(200, "text/html", serverIndex);
}

void handleUpdate() {
  server.sendHeader("Connection", "close");
  if(server.arg("k") != ota_pw) {
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

void handleCredentials() {
    String ssid;
    String wifi_pw;
    if (server.hasArg("ssid") && server.hasArg("wifi_pw")) {
      ssid = server.arg("ssid");
      wifi_pw = server.arg("wifi_pw");
      ota_pw = server.arg("ota_pw");

      preferences.begin("wifi", false);
      preferences.putString("ssid", ssid);
      preferences.putString("wifi_pw", wifi_pw);
      preferences.putString("ota_pw", ota_pw);
      preferences.end();

      server.send(200, "text/html", "Submitted tickerstrings");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
}

void CSVUploadPage() {
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", csvUploadPage);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){
    String message = String((char*) payload);
    Serial.print("WS:");
    Serial.println(message);
    if (message.startsWith("{RGB}=")) {
      uint32_t rgb;
       rgb = strtol(message.substring(7,13).c_str(), 0, 16);
       fillPixel(rgb);
       return;
    }
    Heltec.display->clear();
    Heltec.display->drawString(0 , 0 , message);
    Heltec.display->display();
    SoftSerial.print(message);
  }
}

void startAPMode() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(fallback_ssid, fallback_password); // Using default credentials

  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String wifi_pw = preferences.getString("wifi_pw", "");
  String ota_pw = preferences.getString("ota_pw", "");
  preferences.end();

  char wifiCredentialsPage[512];
  sprintf(wifiCredentialsPage, wifiCredentialsTemplate, ssid.c_str(), wifi_pw.c_str(), ota_pw.c_str());

  server.send(200, "text/html", wifiCredentialsPage);

  server.on("/saveCredentials", HTTP_POST, []() {handleCredentials();});

  server.begin();
}

void connectToWiFi() {
  Heltec.display->clear();
  Heltec.display->drawString(0 , 0 , "Try connecting to WiFi");
  Heltec.display->display();

  Serial.print("Try Connecting to WiFi");

  // Attempt to connect to Wi-Fi with saved credentials
  preferences.begin("wifi", true);  // Begin preferences in read-only mode
  String ssid = preferences.getString("ssid", "");
  String wifi_pw = preferences.getString("wifi_pw", "");
  ota_pw = preferences.getString("ota_pw", "");
  preferences.end();

  if (ssid != "" && wifi_pw != "") {
    WiFi.begin(ssid.c_str(), wifi_pw.c_str());

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Failed to connect to Wi-Fi with saved credentials. Starting AP mode.");
      startAPMode();
    }
  } else {
    Serial.println("No saved credentials found. Starting AP mode.");
    startAPMode();
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

  Heltec.display->clear();
  Heltec.display->drawString(0 , 0 , "Conntected to");
  Heltec.display->drawString(0 , 12 , ssid);
  Heltec.display->drawString(0 , 24 , "IP address:");
  Heltec.display->drawString(0 , 36 , WiFi.localIP().toString().c_str());
  Heltec.display->display();

  SoftSerial.write(CLEAR_DISPLAY);
  SoftSerial.print(WiFi.localIP().toString());
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/ticker", HTTP_GET, tickerPage);
  server.on("/pickerForm", HTTP_GET, pickerPage);
  server.on("/tickerForm", HTTP_POST, handleTicker);
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
