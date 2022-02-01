#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "SD.h"

const char * ssid = "ChinaMobile_2888";
const char * pwd  = "74120100";

const char * http_username = "admin";
const char * http_password = "123456";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Open WiFi:
  WiFi.begin(ssid, pwd);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println(WiFi.localIP());

//  server.on();
//  server.serverStatic("/page.html", ).setCacheControl("max-age=600");
  
  server.begin();
  Serial.println("server started");


  // SD card init:
  if (!SD.begin(5)){
    Serial.println("SD init failed");
  }else {
    Serial.println("SD card ready");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
