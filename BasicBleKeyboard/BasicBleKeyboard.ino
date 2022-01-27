//#define USE_NIMBLE
#include <BleKeyboard.h>
#include <ArduinoJson.h>

#define btn 18
#define led 2

BleKeyboard bleKeyboard;

String json_input = "{\"cmd\":\"cmd\n\"}";
StaticJsonDocument<256> doc;

void setup() {
  
  // GPIO init:------------------------------------
  pinMode(btn, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);


  // Serial and BleKeyboard init:------------------
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
  while (!bleKeyboard.isConnected());
  Serial.println("BLE Connected");
  digitalWrite(led, HIGH);


  // ArduinoJson init:-----------------------------
  DeserializationError err = deserializeJson(doc, json_input);
  if (err) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(err.f_str());
    return;
  }

//  attachInterrupt(digitalPinToInterrupt(btn), onClick, FALLING);
  
}

int g_status = 1;
int g_current_status = 0;

void loop() {
  
  g_status = digitalRead(btn);

  // OnClick Event:
  if (g_status == LOW && g_status != g_current_status){
    delay(200);
  
    if (digitalRead(btn) == LOW){
    
      // Make sure the button is pushed.
      if(bleKeyboard.isConnected()) {

        // Open "RUN":
        bleKeyboard.press(131);
        bleKeyboard.print("r");
        delay(100);
        bleKeyboard.releaseAll();
        delay(100);

        // Open specified program through "RUN" by value from JSON:
        String str = doc[String("cmd")];
        bleKeyboard.print(str);
        delay(1000);

        // Enter content(script):
        bleKeyboard.println("ipconfig");
        
        Serial.printf("pushed cmd: %s", str);
      }
    }
  }

  // Deinit
  g_current_status = g_status;
}
