#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <SD.h>
#include <ArduinoJson.h>


struct actionPackage {
  bool is_mute = false;
  char cur_act[10];
  char cur_act_inf[10];
  char content[20];
  char cur_bgm[20];
} g_action;

struct netConfig {
  int cur_wifi;
  char ssid[20] = "ChinaMobile_2888";
  char pwd[20]  = "74120100";
} g_net;


// Global functions:
char * readFile(String filename);
char * strToChr(String str);
void initConfig();


AsyncWebServer server(80);


void setup() {
  Serial.begin(115200);

  // SD card init:
  if (!SD.begin(5)) {
    Serial.println("SD init failed");
  } else {
    Serial.println("SD card ready");
    initConfig();
  }

  // Open WiFi:
  WiFi.begin(g_net.ssid, g_net.pwd);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println(WiFi.localIP());

  //  server.begin();
  Serial.println("server started");
}


void loop() {
  // put your main code here, to run repeatedly:

}


void initConfig() {
  StaticJsonDocument<256> doc;
  //  char * wifi_json = readFile("/net_conf.txt");
  File netFile = SD.open("/net_conf.txt", FILE_READ);
  DeserializationError err = deserializeJson(doc, netFile);
  if (err) {
    // light up the corresponding lighter
    return;
  }
  g_net.cur_wifi    = doc["current_wifi"];
  strlcpy(g_net.ssid,
          doc["wifi_list"][g_net.cur_wifi]["id"],
          sizeof(g_net.ssid));
  strlcpy(g_net.pwd,
          doc["wifi_list"][g_net.cur_wifi]["pwd"],
          sizeof(g_net.pwd));
  netFile.close();


  // init the action_package:
  File actionFile = SD.open("/action_conf.txt", FILE_READ);
  err = deserializeJson(doc, actionFile);
  if (err) {
    // lighting up the corresponding lighter
    return;
  }
  g_action.is_mute = doc["is_mute"];
  strlcpy(g_action.cur_bgm,
          doc["current_bgm"],
          sizeof(g_action.cur_bgm));
  strlcpy(g_action.cur_act,
          doc["current_action"],
          sizeof(g_action.cur_act));
  strlcpy(g_action.cur_act_inf,
          doc[g_action.cur_act],
          sizeof(g_action.cur_act_inf));
  strlcpy(g_action.content,
          doc["content"],
          sizeof(g_action.content));
  actionFile.close();
}


char * readFile(String filename) {
  File file = SD.open(filename, FILE_READ);
  g_current_file_size = file.size();
  char * json = (char *)malloc(g_current_file_size + 1);
  for (int i = 0; i < g_current_file_size, file.available(); i++) {
    json[i] = file.read();
  }
  file.close();
  return json;
}
