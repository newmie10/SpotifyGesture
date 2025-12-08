#define B0_PIN 34
#define B1_PIN 0
#define B2_PIN 35
// #define TIMEOUT 500
#include <Arduino.h>

// Working on PWM
int pwmChannel = 0;
int freq = 5000;         // 5 kHz
int resolution = 8;      // duty: 0–255 wrong, its bits?

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Beginning");

  pinMode(B0_PIN, INPUT_PULLUP);
  pinMode(B1_PIN, INPUT_PULLUP);
  pinMode(B2_PIN, INPUT_PULLUP);

  // wait until serial port opens for native USB devices
  while (!Serial) {
    delay(1);
  }
}


void loop() {
  bool b0Pressed = (digitalRead(B0_PIN) == LOW);
  bool b1Pressed = (digitalRead(B1_PIN) == LOW);
  bool b2Pressed = (digitalRead(B2_PIN) == LOW);

  if (b0Pressed) {
    Serial.println("PLAY");
    // e.g., Serial.println("PLAY");
    delay(200);  // simple debounce
  }

  if (b1Pressed) {
    Serial.println("PAUSE");
    // e.g., Serial.println("NEXT");
    delay(200);
  }

  if (b2Pressed) {
    Serial.println("NEXT");
    // e.g., Serial.println("PREV");
    delay(200);
  }
 
  delay(50);
}