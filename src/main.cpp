#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting...");
}

void loop()
{
  Serial.println("Still running...");
  delay(2000);
}