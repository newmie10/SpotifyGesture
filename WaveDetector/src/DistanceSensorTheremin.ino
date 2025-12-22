#include "Adafruit_VL53L0X.h"
#include "screen.h"

#define LED1 2
#define LED2 26
#define XSHUT1 27
#define XSHUT2 33
#define ADDRESS1 0x30
#define ADDRESS2 0x31
#define ARR_SIZE 40
#define TFT_MOSI=23
#define TFT_SCLK=18
#define TFT_DC=2
#define TFT_RST=4

// #define TIMEOUT 500

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
// Working on PWM
int pwmChannel = 0;
int freq = 5000;         // 5 kHz
int resolution = 8;      // duty: 0–255 wrong, its bits?
int measurements[ARR_SIZE];

void setup() {
  for (int i = 0 ; i < ARR_SIZE ; i++) {
    measurements[i] = 2000;
  }

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
  ledcAttachPin(LED1, pwmChannel);

  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

int prevTime = 0;
int prevSensor = 0;
int curtime = 0;
int timeout = 1000;
int mode = -1; // 0 for gestures, 1 for height (volume)
int modeTime = -1;
int volume = 100;
int loopIndex = 0;
int sum = 0;
int vol = 50;
int volDelay = -1;

int arrayMean() {
  sum = 0;
  for (int i = 0 ; i < ARR_SIZE ; i++) {
    sum += measurements[i];
  }
  return sum / ARR_SIZE;
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure1;
  VL53L0X_RangingMeasurementData_t measure2;
  curtime = millis();

    
  // Serial.print("Reading a measurement... \n");
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false);
  if (measure1.RangeStatus != 4 && modeTime < curtime) {
    modeTime = curtime + 1;
    measurements[loopIndex % ARR_SIZE] = measure1.RangeMilliMeter;
    loopIndex++;
    // Serial.println(arrayMean());
    if (arrayMean() < 100) {
      mode *= -1;
      for (int i = 0 ; i < ARR_SIZE ; i++) {
        measurements[i] = 2000;
      }
    }
  }

  if (mode == -1) {
    ledcWrite(0, 0);
    if (measure1.RangeStatus != 4 && measure1.RangeMilliMeter < 300) {
      // If both sensors detected, check if held for 3 seconds. Switch mode if so
      if (prevSensor == 0 || curtime > (prevTime + timeout)) {
        prevSensor = 1;
        prevTime = curtime;
      }
      else if (prevSensor == 2 && curtime < (prevTime + timeout)) {
        Serial.println("NEXT");
        // digitalWrite(LED1, HIGH);
        // delay(1000);
        // digitalWrite(LED1, LOW);
        prevSensor = 0;
        prevTime = -1000;
      }
    }
    else if (measure2.RangeStatus != 4 && measure2.RangeMilliMeter < 300) {
      if (prevSensor == 0 || curtime > (prevTime + timeout)) {
        prevSensor = 2;
        prevTime = curtime;
      }
      else if (prevSensor == 1 && curtime < (prevTime + timeout)) {
        Serial.println("PREV");
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
    ledcWrite(pwmChannel, volume);
    // Emulate volume control in the light at pin 26
    if (measure2.RangeStatus != 4 && measure2.RangeMilliMeter < 300 && volDelay <= curtime) {
      vol = measure2.RangeMilliMeter / (1000 / 255);
      volume = vol > 100 ? 100 : vol;
      Serial.print("VOL ");
      Serial.println(volume);
      volDelay = curtime + 500;
    }
  }
}





