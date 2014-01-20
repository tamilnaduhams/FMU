#include <Servo.h>
#include <SPI.h>
#include <Ping.h>
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include <Wire.h>
#endif

#include "UltraSonicProximity.h"
#include "KKMulticopterBoard.h"

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24Carrier.h"
#include "SensorData.h"

#define AIL_PIN         3
#define ELE_PIN         4
#define THR_PIN         5
#define RUD_PIN         6
#define TRIGGER_PIN     7
#define ECHO_PIN        8
#define RF24_CE_PIN     9
#define RF24_CSN_PIN    10
#define B_LED_PIN         14
#define G_LED_PIN         15
#define R_LED_PIN         16
#define MPU6050_INT_PIN 20

#define ALPHA         0.5
#define MAX_DISTANCE  250
#define MAX_PLAY_TIME 60000

KKMulticopterBoard  kkboard(AIL_PIN, ELE_PIN, THR_PIN, RUD_PIN);

MPU6050Data mpu6050Data;

int         near_ground_height = 0;
int         last_near_ground_height = 0;
double      velocity[3];

unsigned long since = millis();
unsigned long lastSensorRead = millis();
unsigned long lastPrint = millis();
unsigned long lastSendRF24 = millis();

void setup()
{
    delay(5000);
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
#endif
    Serial.begin(38400);
    
    pinMode(R_LED_PIN, OUTPUT);
    pinMode(G_LED_PIN, OUTPUT);
    pinMode(B_LED_PIN, OUTPUT);
    LED_COLOR(1,1,1);
    
    Serial.println("Setup RF24");
    setupRF24();

    Serial.println("Setup MPU");
    if (!setupMPU()) {
        Serial.println("MPU6050 failed initialization");
        LED_COLOR(1,0,0);
        while (true) delay(100000);
    }

    Serial.println("Setup Flight Board");
    setupControl();

    Serial.println("Ready in 5 seconds");
    blinkTimeout(5, 2);
    LED_COLOR(0,1,0);
    since = millis();
}

void loop()
{
    readSensors();
    calculateData();

    lastSensorRead = millis();

    if (millis() - since > MAX_PLAY_TIME) {
        //land(10);
    } 
    else {
        //holdAt(30);
    }
    if (millis() - lastPrint > 200) {
        print();
        lastPrint = millis();
    }
    if (millis() - lastSendRF24 > 100) {
        sendSensorData();
        lastSendRF24 = millis();
    }
}

//========== Sensor data functions ==========
void readSensors() {
    readMPU();
    readUltraSound();
}

void calculateData() {
    if (near_ground_height > 0 && near_ground_height <= MAX_DISTANCE) {
        double v = (near_ground_height - last_near_ground_height) * 10.0 / (millis() - lastSensorRead);
        velocity[2] = complementary_filter(v, velocity[2], ALPHA);
    } 
    else {
        velocity[2] = -343.2;
    }
}

