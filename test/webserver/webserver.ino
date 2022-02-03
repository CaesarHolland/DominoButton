#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <time.h>


// Configuration structs:
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


// Customized functions:
char * readFile(String filename);
char * strToChr(String str);
void initConfig();
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void writeFile(fs::FS &fs, const char * path, const char * message);


// Global varibles:
int g_dirstatus = 1;

AsyncWebServer server(80);


void setup() {
  Serial.begin(115200);

  // SD card init:
  if (!SD.begin(5)) {
    Serial.println("SD init failed");
  }
  else {
    Serial.println("SD card ready");
    initConfig();
    g_dirstatus = listDir(SD, "/", 0);
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
  return;
}


char * readFile(String filename) {
  File file = SD.open(filename, FILE_READ);
  int filesize = file.size();
  char * json = (char *)malloc(filesize + 1);
  for (int i = 0; i < filesize, file.available(); i++) {
    json[i] = file.read();
  }
  file.close();
  return json;
}


int listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
// Failed to open directory
    return -1;
  }
  if(!root.isDirectory()){
// Not a directory
    return -2;
  }

  StaticJsonDocument<512> doc;
  File file = root.openNextFile();
  int i=0;
  while(file){
    if(file.isDirectory()){
      doc["list"][i]["type"] = "DIR";

      char filename[30];
      strcpy(filename, file.name());
      doc["list"][i]["filename"] = filename;
      Serial.println(file.name());
          
      time_t t= file.getLastWrite();
      char buf[20];
      strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
      doc["list"][i]["time"] = buf;
          
      i++;
//          if(levels){
//            // through levels varible we can recursively traverse dir but --- currently we can't extend the parent path name yet.
//            listDir(fs, file.name(), levels -1);
//          }
     }
     else {
       doc["list"][i]["type"] = "FILE";
       
       char filename[30];
       strcpy(filename, file.name());
       doc["list"][i]["filename"] = filename;
       
       doc["list"][i]["size"] = file.size();
       
       time_t t= file.getLastWrite();
       char buf[20];
       strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
       doc["list"][i]["time"] = buf;
       
       i++;
     }

     file = root.openNextFile();
  }

  file.close();

  // Save json into file:
  SD.remove("/file_sys.txt");
  File newFile = SD.open("/file_sys.txt", FILE_WRITE);
  serializeJson(doc, newFile);
  newFile.close();

  return 1;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}
