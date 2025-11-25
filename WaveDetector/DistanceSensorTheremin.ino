#include "Adafruit_VL53L0X.h"

#define LED1 2
#define LED2 26
#define XSHUT1 23
#define XSHUT2 18
#define ADDRESS1 0x30
#define ADDRESS2 0x31
// #define TIMEOUT 500

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
// Working on PWM
int pwmChannel = 0;
int freq = 5000;         // 5 kHz
int resolution = 8;      // duty: 0–255 wrong, its bits?

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  // pinMode(LED2, OUTPUT);
  pinMode(XSHUT1, OUTPUT);
  pinMode(XSHUT2, OUTPUT);
  delay(5000);
  Serial.println("Beginning");

  // wait until serial port opens for native USB devices
  while (!Serial) {
    delay(1);
  }
  
  // XSHUT down and up https://cdn-learn.adafruit.com/downloads/pdf/adafruit-vl53l0x-micro-lidar-distance-sensor-breakout.pdf
  digitalWrite(XSHUT1, LOW);
  digitalWrite(XSHUT2, LOW);
  delay(10);
  digitalWrite(XSHUT1, HIGH);
  digitalWrite(XSHUT2, HIGH);

  // ADDRESS1 on, 2 off, set 1 address
  digitalWrite(XSHUT2, LOW);

  Serial.println("Sensor 1 Boot Test");
  if (!lox1.begin(ADDRESS1)) {
    Serial.println(F("Failed to boot SENSOR1"));
    while(1);
  }

  digitalWrite(XSHUT2, HIGH);

  Serial.println("Sensor 2 Boot Test");
  if (!lox2.begin(ADDRESS2)) {
    Serial.println(F("Failed to boot SENSOR2"));
    while(1);
  }

  Serial.println("PWM initializing");

  ledcSetup(0, freq, resolution);
  ledcAttachPin(LED2, pwmChannel);

  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

int prevTime = 0;
int prevSensor = 0;
int curtime = 0;
int timeout = 1000;
int mode = 0; // 0 for gestures, 1 for height (volume)
int modeTime = -1;
int volume = 0;


void loop() {
  VL53L0X_RangingMeasurementData_t measure1;
  VL53L0X_RangingMeasurementData_t measure2;
  curtime = millis();

    
  Serial.print("Reading a measurement... \n");
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false);

  if (mode == 0) {
    if (measure1.RangeStatus != 4 && measure1.RangeMilliMeter < 600) {
      // If both sensors detected, check if held for 3 seconds. Switch mode if so
      if (measure2.RangeStatus != 4 && measure2.RangeMilliMeter < 600) {
        if (modeTime < 0) {
          modeTime = curtime + 3000;
        }
        else if (modeTime > 0 && modeTime <= curtime)
        {
          mode = 1;
          modeTime = -1;
          Serial.println("Mode Switched");
        }
      }
      else
      {
        modeTime = -1;
      }
      if (prevSensor == 0 || curtime > (prevTime + timeout)) {
        prevSensor = 1;
        prevTime = curtime;
      }
      else if (prevSensor == 2 && curtime < (prevTime + timeout)) {
        Serial.println("RIGHT WAVE DETECTED");
        digitalWrite(LED1, HIGH);
        delay(1000);
        digitalWrite(LED1, LOW);
        prevSensor = 0;
        prevTime = -1000;
      }
    }
    else if (measure2.RangeStatus != 4 && measure2.RangeMilliMeter < 600) {
      if (prevSensor == 0 || curtime > (prevTime + timeout)) {
        prevSensor = 2;
        prevTime = curtime;
      }
      else if (prevSensor == 1 && curtime < (prevTime + timeout)) {
        Serial.println("LEFT WAVE DETECTED");
        ledcWrite(pwmChannel, 255);
        delay(1000);
        ledcWrite(pwmChannel, 0);
        prevSensor = 0;
        prevTime = -1000;
      }
    }
  }
  else // Volume control mode
  {
    // Emulate volume control in the light at pin 26
    if (measure1.RangeStatus != 4 && measure1.RangeMilliMeter < 600) {
      volume = measure1.RangeMilliMeter / (600 / 255);
      Serial.println(volume);
      ledcWrite(pwmChannel, volume);
      if (measure1.RangeMilliMeter < 200) {
        if (modeTime < 0) {
          modeTime = millis() + 3000;
        }
        else if (modeTime < millis()) {
          mode = 0;
          modeTime = -1;
          ledcWrite(pwmChannel, 0);
        }
      }
      else
      {
        modeTime = -1;
      }
    }
  }
    
  delay(50);
}