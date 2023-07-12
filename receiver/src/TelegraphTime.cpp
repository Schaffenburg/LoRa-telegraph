#include "TelegraphTime.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void setupTime()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%d.%m.%Y %H:%M:%S");
}

void handleEventsTimer() {
    time_t now;

    static unsigned long lastLoopTime = millis();
    if(millis() - lastLoopTime < 1000) {
        return;
    }
    lastLoopTime = millis();

    DataEntry next_event = entries[current_index];

    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    Serial.print(&timeinfo, "%d.%m.%Y %H:%M:%S ");
    Serial.printf("(%ld), %ld seconds until next event\n", now, next_event.timestamp-now);

    if (next_event.timestamp <= now)
    {
        Serial.printf("New Event timestamp reached... displaying: %s\n", next_event.name.c_str());

        Heltec.display->clear();
        Heltec.display->drawString(0, 0 , "Nächster Auftritt:\n");
        Heltec.display->drawStringMaxWidth(0, 22, 128, next_event.name.c_str());
        Heltec.display->display();
        current_index++;
    }
}