// Copyright (c) 2024 Gabor Smith. All rights reserved.
//
// This work is licensed under the terms of the MIT license.  
// For a copy, see <https://opensource.org/licenses/MIT>.

// Created 2024-02-25 
// Door lock servo actuator with Blynk support

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL4uXybjO3W"
#define BLYNK_TEMPLATE_NAME "gate"
#define BLYNK_AUTH_TOKEN "Gb-9OMbQ_1uCS6t0x8o59PjLgLB3E48p"

#define SERVO_OUTPUT_PIN 4 // NODEMCU D2
#define CARD_INPUT_PIN 5 // NODEMCU D1

#define DRIVE_TIME 4600 // TOTAL SERVO DRIVE TIME IN MS
#define SLOW_AT 3800 // SLOW DOWN AT THIS MS

#define SLOW_SPEED_CW 150 // SLOW SPEED TURNING CW
#define SLOW_SPEED_CCW 30 // SLOW SPEED TURNING CCW

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

Servo myservo;

volatile bool isLocked = false;
volatile bool lockRequested = false;
volatile bool unlockRequested = false;

char ssid[] = "SMITHCOM";
char pass[] = "jugbogdan63";

void lock() {
  int pos = 180;
  double startTime = millis();
  while ((millis() - startTime) <= DRIVE_TIME) {
    if ((millis() - startTime) >= SLOW_AT) pos = SLOW_SPEED_CW;
    myservo.write(pos);
  }
  myservo.write(90);
}

void unlock() {
  int pos = 0;
  double startTime = millis();
  while ((millis() - startTime) <= DRIVE_TIME) {
    if ((millis() - startTime) >= SLOW_AT) pos = SLOW_SPEED_CCW;
    myservo.write(pos);
  }
  myservo.write(90);
}

void ICACHE_RAM_ATTR handleLockUnlock() {
  if (isLocked) {
    unlockRequested = true;
    lockRequested = false;
  } else {
    unlockRequested = false;
    lockRequested = true;
  }
}

int doLockUnlock(int value) {
  int returnValue;
  if (value == 1) {
    Serial.println("Lock requested");
    lock();
    isLocked = true;
    lockRequested = false;
    returnValue = 1;
  } else if (value == 0) {
    Serial.println("Unlock requested");
    unlock();
    isLocked = false;
    unlockRequested = false;
    returnValue = 0;
  }
  return returnValue;
}

BLYNK_WRITE(V0)
{
  int value = param.asInt();
  int returnValue = 0;

  returnValue = doLockUnlock(value);

  Blynk.virtualWrite(V1, returnValue);
}

void setup() {
  Serial.begin(115200);
  isLocked = false;
  lockRequested = false;
  unlockRequested = false;

  myservo.attach(SERVO_OUTPUT_PIN);
  myservo.write(90);
  delay(10);

  pinMode(CARD_INPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CARD_INPUT_PIN), handleLockUnlock, FALLING);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  if (lockRequested) {
    doLockUnlock(1);
  }

  if (unlockRequested) {
    doLockUnlock(0);
  }
  delay(2);
}
