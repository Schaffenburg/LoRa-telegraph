/*
   based on
   OTAWebUpdater.ino Example from ArduinoOTA Library
   Rui Santos
   Complete Project Details http://randomnerdtutorials.com
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>

#include "webportal.h"
#include "credentials.h"
#include "OTAUpdate.h"

/*
   Login page
*/
const char *loginIndex =
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
    "<td><input><br></td>"
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
   Server Index Page
*/

const char *serverIndex =
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

/*
   setup function
*/
void setupOTA()
{
  /*return index page which is stored in serverIndex */
  server.on("/login", HTTP_GET, []()
            {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex); });
  server.on("/serverIndex", HTTP_GET, []()
            {
    server.sendHeader("Connection", "close");

    if (server.arg("k") != OTA_PWD)
      server.send(400, "text/html", "invalid credentials.");
    else
      server.send(200, "text/html", serverIndex); });
  /*handling uploading firmware file */
  server.on(
      "/update", HTTP_POST, []()
      {
    server.sendHeader("Connection", "close");
    if (server.arg("k") != OTA_PWD)
      server.send(400, "text/html", "invalid credentials.");
    else
      server.send(200, "text/html", serverIndex);

    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); },
      []()
      {
        HTTPUpload &upload = server.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN))
          { // start with max available size
            Update.printError(Serial);
          }
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
          /* flashing firmware to ESP*/
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
          {
            Update.printError(Serial);
          }
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
          if (Update.end(true))
          { // true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          }
          else
          {
            Update.printError(Serial);
          }
        }
      });

  server.begin();
}
