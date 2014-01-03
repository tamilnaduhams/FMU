#include <Servo.h>

#define LED_PIN 13
#define MOTOR_1_PIN 3
#define MOTOR_2_PIN 4
#define MOTOR_3_PIN 20
#define MOTOR_4_PIN 21

#define CALIBRATE_ESC
#define THROTTLE_MIN 700
#define THROTTLE_MAX 2000

boolean LED_STATUS = false;
int throttle = 0;
Servo motor[4];

void initializeESC()
{
    motor[0].attach(MOTOR_1_PIN);
    motor[1].attach(MOTOR_2_PIN);
    motor[2].attach(MOTOR_3_PIN);
    motor[3].attach(MOTOR_4_PIN);
}

void calibrateESC()
{
    throttle = THROTTLE_MAX;

    motor[0].writeMicroseconds(THROTTLE_MAX);
    motor[1].writeMicroseconds(THROTTLE_MAX);
    motor[2].writeMicroseconds(THROTTLE_MAX);
    motor[3].writeMicroseconds(THROTTLE_MAX);

    LED_STATUS = true;
    for (int i = 0; i < 15; i++) {
        digitalWrite(LED_PIN, LED_STATUS);
        LED_STATUS = !LED_STATUS;
        delay(500);
    }
    for (int i = 0; i < 25; i++) {
        digitalWrite(LED_PIN, LED_STATUS);
        LED_STATUS = !LED_STATUS;
        delay(100);
    }

    motor[0].writeMicroseconds(THROTTLE_MIN);
    motor[1].writeMicroseconds(THROTTLE_MIN);
    motor[2].writeMicroseconds(THROTTLE_MIN);
    motor[3].writeMicroseconds(THROTTLE_MIN);
    
    LED_STATUS = true;
    for (int i = 0; i < 25; i++) {
        digitalWrite(LED_PIN, LED_STATUS);
        LED_STATUS = !LED_STATUS;
        delay(200);
    }
}

void setup()
{
    Serial.begin(38400);
    
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, true);
    
    Serial.println("Initialize ESC");
    initializeESC();
    
    #ifdef CALIBRATE_ESC
        Serial.println("Calibrate ESC");
        calibrateESC();
    #endif
    
    LED_STATUS = true;
    digitalWrite(LED_PIN, LED_STATUS);
    Serial.println("ESC ready, engine start in 5s");
    delay(5000);
}

void loop()
{
    throttle = 30;
    motor[0].write(throttle);
    motor[1].write(throttle);
    motor[2].write(throttle);
    motor[3].write(throttle);
    delay(15);
}

