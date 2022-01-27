#define TXD2 17
#define RXD2 16

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(500);
}

void loop()
{
  Serial2.print("from esp32");
  if (Serial2.available()) {
    Serial.println(Serial2.readString());
  }
  delay(1000);
}
