#include <Arduino.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "credentials.h"
#include "modes.h"
#include "webportal.h"
#include "sleep.h"

WebServer server(80);

int nextWlan = 0;
bool printedWlan = false;

const char *mainIndex =
    "<iframe src=\"/login\" style=\"height: 50%; width: 40%; border: none;\"></iframe>"
    "<iframe src=\"/smsg\" style=\"height: 50%; width: 40%;border: none;\"></iframe>"
    "<iframe src=\"/messages\" style=\"height: 100%; width: 60%; position:absolute;top:50%; left:0; border: none; position: absolute;\"></iframe>";

const char *sendMsgIndex =
    "<form action='/smsg' method='get'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
    "<tr>"
    "<td colspan=2>"
    "<center><font size=4><b>Send A Message</b></font></center>"
    "<br>"
    "</td>"
    "<br>"
    "<br>"
    "</tr>"
    "<td>Text:</td>"
    "<td><input size=25 name='m'><br></td>"
    "</tr>"
    "<br>"
    "<br>"
    "</tr>"
    "<tr>"
    "<td><input type='submit' value='Send'></td>"
    "</tr>"
    "</table>"
    "</form>";

const char *messagesListP1 =
    "<br><br><br><br><br><br>"
    "<table width=\"80%\" bgcolor=\"A09F9F\" align=\"center\">"
    "<tbody>"
    "<tr>"
    "<td colspan=\"1\">"
    "<center>"
    "<font size=\"4\"><b>Read:</b></font>"
    "</center>"
    "<br>"
    "</td>"
    "</tr>";

const char *messagesListP2 =
    "</tbody>"
    "</table></form><div style=\"display: block;\">"
    "</div>";

void wlanConnected(int now)
{
  // Wait for connection
  if (nextWlan < now && WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");

    nextWlan = now + 500;
  }
  else
  {
    if (!printedWlan)
    {
      printedWlan = true;
      Serial.print("\nConnected to ");
      Serial.println(WIFI_SSID);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      /*use mdns for host name resolution*/
      if (!MDNS.begin(HOST))
        Serial.println("Error setting up MDNS responder, MDNS will not be used");
      else
        Serial.println("mDNS responder started");
    }
  }
}

String web_msg;

void setupWeb()
{
  // Connect to WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.print("Connecting to WiFi");

  server.on("/", HTTP_GET, []()
            {    
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", mainIndex); });

  server.on("/smsg", HTTP_GET, []()
            {
    String val = server.arg("m");
    if(val != "") {
        for(uint8_t i = 0 ; i < val.length() ; i++ ) {
            handleWord(val[i]);
        }
    }

     server.sendHeader("Connection", "close");
    server.send(200, "text/html", sendMsgIndex); });

  server.on("/deepsleep", HTTP_GET, []()
            {
    if (server.arg("k") != OTA_PWD) {
      server.send(400, "text/html", "invalid credentials.");
    } else {
      server.send(200, "text/html", "derp sleep");
      
      go_to_sleep(); 
    } });

  server.on("/messages", HTTP_GET, []()
            {
              String buf = messagesListP1;

              //<tr><td>&nbsp;&nbsp;57378401</td><td></td><td>Text was written<br></td></tr>
              buf += "<tr><td>";
              buf += web_msg;
              buf += "<br></td></tr>";

              buf += messagesListP2;

              server.send(200, "text/html", buf); // Send the headers
            });
}

void addMsg(String str)
{
  web_msg += str;
  web_msg += " ";

  int len = web_msg.length();
  if (len > MAXWEBMSG)
  {
    web_msg = web_msg.substring(len - MAXWEBMSG);
  }
}
