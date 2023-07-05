#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "./credentials.h"

/*
 * Form to display strings page
 */
const char* displayForm = 
"<form action='/displayString' method='post'><textarea name='text' rows='2' cols='40'></textarea><br><input type='submit'></form>";

/*
 * Login page
 */
const char* loginIndex = 
 "<form action='/serverIndex' method='get'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 OTA Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input size=25><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Key:</td>"
            "<td><input type='Password' size=25 name='k'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>";
 
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: `/update?k=${(new URLSearchParams(window.location.search)).get(\"k\")}`,"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

WebServer server(80);

void handleWebClient() {
  server.handleClient();
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

void setupWebServer() {
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
    
  server.on("/", handleRoot);
  server.on("/displayString", HTTP_POST, handleDisplayString);
  server.on("/OTA", HTTP_GET, handleOTA);
  server.on("/serverIndex", HTTP_GET, handleServerIndex);
  server.on("/update", HTTP_POST, []() {
    handleUpdate();
  }, handleUpload);
  server.begin();
}
