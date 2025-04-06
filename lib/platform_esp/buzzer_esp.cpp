#include <Arduino.h>
#include "buzzer_esp.h"

Buzzer::Buzzer(int pin) : pin(pin) {}

void Buzzer::buzz()
{
    tone(pin, 1047, 150); // 1047 Hz = C6 note
    delay(150);           // wait for tone to finish
    noTone(pin);          // ensure it's off
}