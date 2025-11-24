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

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
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

  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

int prevTime = 0;
int prevSensor = 0;
int curtime = 0;
int timeout = 1000;


void loop() {
  VL53L0X_RangingMeasurementData_t measure1;
  VL53L0X_RangingMeasurementData_t measure2;
  curtime = millis();

    
  Serial.print("Reading a measurement... \n");
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false);

  if (measure1.RangeStatus != 4 && measure1.RangeMilliMeter < 600) {
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
      digitalWrite(LED2, HIGH);
      delay(1000);
      digitalWrite(LED2, LOW);
      prevSensor = 0;
      prevTime = -1000;
    }
  }
    // Serial.print("Distance (mm)(1): "); Serial.println(measure1.RangeMilliMeter);
  // } else {
  //   digitalWrite(LED1, LOW);
  //   Serial.println(" 1 out of range ");
  // }

  // Sensor 2, LED only
  // if (measure2.RangeStatus != 4) {  // phase failures have incorrect data
  //   if (measure2.RangeMilliMeter < 600) {
  //     digitalWrite(LED2, HIGH);
  //   }
  //   Serial.print("Distance (mm)(2): "); Serial.println(measure2.RangeMilliMeter);
  // } else {
  //   digitalWrite(LED2, LOW);
  //   Serial.println(" 2 out of range ");
  // }
    
  delay(50);
}