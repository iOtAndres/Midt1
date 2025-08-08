/*
 * Project Insole
 * Author: Andres
 * Date: 8/4/2025
 */

#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "credentials.h"
#include "Particle.h"
#include "Encoder.h"
#include "Button.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// MQTT Setup
TCPClient TheClient;
Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe VIBRATION = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/foot-pressure.vibration");

// Function declarations
void MQTT_connect();
bool MQTT_ping();
void subscriptionHandler(const char *event, const char *data);
void handleEncoderClick();
void handleEncoderRotation();
void updateVibration(int mode);
void updateOLED(int mode);


// OLED Setup
const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);


// Encoder setup
const int PINA = D3;
const int PINB = D4;
const int SWPIN = D16;
Encoder myEnc(PINA, PINB);
Button encButton(SWPIN);

// Vibration motor pins
int heelMotor1 = D1;
int heelMotor2 = D2;
int archMotor1 = D5;
int archMotor2 = D6;
int ballMotor1 = D7;
int ballMotor2 = D8;
int toeMotor1  = D9;
int toeMotor2  = D10;

// PWM values per zone
int heelPWM = 180;
int archPWM = 128;
int ballPWM = 128;
int toePWM  = 100;

// State variables
int clickCount = 1;
int lastEncVal = 0;
bool mqttOverride = false;


void setup() {

Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(heelMotor1, OUTPUT);
  pinMode(heelMotor2, OUTPUT);
  pinMode(archMotor1, OUTPUT);
  pinMode(archMotor2, OUTPUT);
  pinMode(ballMotor1, OUTPUT);
  pinMode(ballMotor2, OUTPUT);
  pinMode(toeMotor1, OUTPUT);
  pinMode(toeMotor2, OUTPUT);

  mqtt.subscribe(&VIBRATION);

  // OLED Init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Smart Insole Boot");
  display.println("Initializing...");
  display.display();
  delay(2000);
  display.clearDisplay();
}


void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &VIBRATION) {
      int mqttValue = atoi((char *)VIBRATION.lastread);
      Serial.printf("MQTT Vibration Mode: %d\n", mqttValue);

      if (mqttValue == 0) {
        mqttOverride = true;
        updateVibration(0);
      } else if (mqttValue >= 1 && mqttValue <= 5) {
        mqttOverride = true;
        clickCount = mqttValue;
        updateVibration(clickCount);
      } else {
        mqttOverride = false;
      }
    }
  }

  if (!mqttOverride) {
    handleEncoderClick();
    handleEncoderRotation();
  }
}

void handleEncoderClick() {
  if (encButton.isClicked()) {
    clickCount++;
    if (clickCount > 5) clickCount = 1;
    Serial.printf("Encoder clicked %d times\n", clickCount);
    updateVibration(clickCount);
  }
}

void handleEncoderRotation() {
  int encVal = myEnc.read();
  if (encVal != lastEncVal) {
    encVal = constrain(encVal, 0, 100);

    heelPWM = map(encVal, 0, 100, 130, 255);
    archPWM = map(encVal, 0, 100, 100, 200);
    ballPWM = map(encVal, 0, 100, 100, 200);
    toePWM  = map(encVal, 0, 100, 80, 150);

    lastEncVal = encVal;

    Serial.printf("PWM Values - Heel: %d, Arch: %d, Ball: %d, Toe: %d\n", heelPWM, archPWM, ballPWM, toePWM);
    updateVibration(clickCount);
  }
}

void updateVibration(int mode) {
  analogWrite(heelMotor1, 0);
  analogWrite(heelMotor2, 0);
  analogWrite(archMotor1, 0);
  analogWrite(archMotor2, 0);
  analogWrite(ballMotor1, 0);
  analogWrite(ballMotor2, 0);
  digitalWrite(toeMotor1, LOW);
  digitalWrite(toeMotor2, LOW);

  switch (mode) {
    case 1:
      analogWrite(heelMotor1, heelPWM);
      analogWrite(heelMotor2, heelPWM);
      analogWrite(archMotor1, archPWM);
      analogWrite(archMotor2, archPWM);
      analogWrite(ballMotor1, ballPWM);
      analogWrite(ballMotor2, ballPWM);
      analogWrite(toeMotor1, toePWM);
      analogWrite(toeMotor2, toePWM);
      break;
    case 2:
      analogWrite(heelMotor1, heelPWM);
      analogWrite(heelMotor2, heelPWM);
      break;
    case 3:
      analogWrite(archMotor1, archPWM);
      analogWrite(archMotor2, archPWM);
      break;
    case 4:
      analogWrite(ballMotor1, ballPWM);
      analogWrite(ballMotor2, ballPWM);
      break;
    case 5:
      analogWrite(toeMotor1, toePWM);
      analogWrite(toeMotor2, toePWM);
      break;
    default:
      break;
  }

  updateOLED(mode);
}

void updateOLED(int mode) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.printf("Mode: %d\n", mode);
  display.printf("Heel: %d\n", heelPWM);
  display.printf("Arch: %d\n", archPWM);
  display.printf("Ball: %d\n", ballPWM);
  display.printf("Toe : %d\n", toePWM);
  display.display();
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) {
    Serial.printf("Failed, code %d. Retrying...\n", ret);
    delay(5000);
  }
  Serial.println("MQTT Connected!");
}