/* 
 * Project Press Insole
 * Author: Andres
 * Date: 8/4/2025
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
#include "Particle.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"
#include "neopixel.h"
#include "Colors.h"
#include "Encoder.h"
#include "Button.h"

SYSTEM_MODE(AUTOMATIC);
//SYSTEM_THREAD(ENABLED);
/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe VIBRATION = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/foot-pressure.vibration"); 
//Adafruit_MQTT_Publish footTemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/footTemp");
// Adafruit_MQTT_Publish FOOTpressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/FOOTpressure");
Adafruit_MQTT_Publish HeelPressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/foot-pressure.heelpressure");
Adafruit_MQTT_Publish ArchPressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/foot-pressure.archpressure");
// Adafruit_MQTT_Publish BallPressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/foot-pressure.archpressure");
Adafruit_MQTT_Publish ToePressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/foot-pressure.toepressure");
//Adafruit_MQTT_Publish fallAlert = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fallAlert");


//Functions
void MQTT_connect();
bool MQTT_ping();
//void subscriptionHandler(const char *event, const char *data);
void updatePressurePixels();
void updateTempPixels();
uint32_t pressureToColor(int pressure);

// NeoPixels
const int PIXELCOUNT = 4;
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
int bri = 200;

// Pressure sensors
int pressurePinBigToe = A5;
int pressurePinBall = A2;
int pressurePinArch = A1;
int pressurePinHeel = A0;



// Vibration motor
int Vibe = D15;
float frequency;
int vibrationPWM = 180; // ~28Hz


// // PWM pins for frequency-controlled motors
// int heelMotor1 = D1;
// int heelMotor2 = D2;
// int archMotor1 = D3;
// int archMotor2 = D4;
// int ballMotor1 = D5;
// int ballMotor2 = D6;

// // Digital pins for simple ON/OFF motors
// int bigToeMotor = D7;
// int toeMotor1 = D8;
// int toeMotor2 = D9;





// Encoder
const int PINA = D3;
const int PINB = D4;
const int SWPIN = D16;
Encoder myEnc(PINA, PINB);
Button encButton(SWPIN);
bool VibeOn = false;
int lastEncVal = 0;

// Timers
unsigned long lastPressureTime = 0;
unsigned long lastTempTime = 0;
const unsigned long pressureInterval = 5000;
// const unsigned long tempInterval = 7000;


unsigned int last, lastTime;
float subValue,pubHEELValue ,pubARCHValue, pubBALLValue ,pubTOEValue;
// float yellow;
int VibebuttonState;
bool onOFF;

void setup() {
  Serial.begin(9600);
  pixel.begin();
  pixel.setBrightness(bri);
  pixel.clear();
  pixel.show();

  pinMode(Vibe, OUTPUT);

  
// // Set PWM pins
//   pinMode(heelMotor1, OUTPUT);
//   pinMode(heelMotor2, OUTPUT);
//   pinMode(archMotor1, OUTPUT);
//   pinMode(archMotor2, OUTPUT);
//   pinMode(ballMotor1, OUTPUT);
//   pinMode(ballMotor2, OUTPUT);

//   // Set digital pins
//   pinMode(bigToeMotor, OUTPUT);
//   pinMode(toeMotor1, OUTPUT);
//   pinMode(toeMotor2, OUTPUT);

  // pinMode(SWPIN, INPUT_PULLUP);
//  pinMode(Vibe, );
// pinMode(Vibe, OUTPUT);             // Vibration Sensor
  //pinSetDriveStrength(D15, DriveStrength::HIGH);
   // Toggle vibration on button click
  if (encButton.isClicked()) {
    analogWrite(VibeOn, 180);
  }

 // if (!bme.begin(HEXADDY)) {
   // Serial.println("BME280 failed to start");
  //}

  // myEnc.write(0);
    // Setup MQTT subscription
  mqtt.subscribe(&VIBRATION);
    waitFor(Serial.isConnected,15000);

  // String subscriptionName = String::format("%s/%s/", System.deviceID().c_str(), EVENT_NAME);
  // Particle.subscribe(subscriptionName, subscriptionHandler, MY_DEVICES);
  // Serial.printf("Subscribing to %s\n", subscriptionName.c_str());
}

void loop() {
MQTT_connect();
MQTT_ping();

// this is our 'wait for incoming subscription packets' busy subloop 
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &VIBRATION) {
    VibebuttonState = atof((char *)VIBRATION.lastread);
     
      if (VibebuttonState == 1){

      digitalWrite(Vibe,HIGH);
      
      }
      if (VibebuttonState ==0){
       digitalWrite(Vibe,LOW);
      
      }
    }
  }
if((millis()-lastTime > 10000)) {
    if(mqtt.Update()) {

      pubHEELValue = analogRead(pressurePinHeel);
      HeelPressure.publish(pubHEELValue);
      Serial.printf("Publishing HEEL%f \n",pubHEELValue); 
      pubARCHValue = analogRead(pressurePinArch);
      ArchPressure.publish(pubHEELValue);
      Serial.printf("Publishing ARCH %f \n",pubARCHValue); 
      pubTOEValue = analogRead(pressurePinBigToe);
      ToePressure.publish(pubTOEValue);
      Serial.printf("Publishing TOE %f \n",pubTOEValue); 
      } 
    lastTime = millis();
  }

// if((millis()-lastTime > 6000)) {
//     if(mqtt.Update()) {

//       pubARCHValue = analogRead(pressurePinArch);
//       ArchPressure.publish(pubHEELValue);
//       Serial.printf("Publishing ARCH %f \n",pubARCHValue); 
//       } 
//     lastTime = millis();
//   }


// if((millis()-lastTime > 6000)) {
//     if(mqtt.Update()) {

//       pubBALLValue = pressurePinBall;
//       pubBALLValue.publish(pubHEELValue);
//       Serial.printf("Publishing BALL %f \n",pubBALLValue); 
//       } 
//     lastTime = millis();
//   }

// if((millis()-lastTime > 6000)) {
//     if(mqtt.Update()) {

//       pubTOEValue = analogRead(pressurePinBigToe);
//       ToePressure.publish(pubTOEValue);
//       Serial.printf("Publishing TOE %f \n",pubTOEValue); 
//       } 
//     lastTime = millis();
//   }
//   }


  unsigned long currentTime = millis();

  // Pressure to NeoPixels every 5 seconds
  if (currentTime - lastPressureTime >= pressureInterval) {
    lastPressureTime = currentTime;
    updatePressurePixels();
  }

  // Temperature to NeoPixels every 7 seconds
  // if (currentTime - lastTempTime >= tempInterval) {
  //   lastTempTime = currentTime;
  //   updateTempPixels();
  // }

  // Toggle vibration on button click
  if (encButton.isClicked()) {
    VibeOn = !VibeOn;
    Serial.printf("Vibration %s\n", VibeOn ? "ON" : "OFF");
  }

  // Encoder adjusts vibration frequency
  int encVal = myEnc.read();
  if (encVal != lastEncVal) {
    if (encVal < 0) encVal = 0;
    if (encVal > 100) encVal = 100;

    int rpm = map(encVal, 0, 100, 1200, 2100);
    vibrationPWM = map(rpm, 1200, 2100, 130, 255);
    lastEncVal = encVal;

    Serial.printf("Encoder RPM: %i, PWM: %i\n", rpm, vibrationPWM);
  }

  // Apply vibration
  analogWrite(Vibe, VibeOn ? vibrationPWM : 0);
  // analogWrite(Vibe, true);

}



// Pressure sensor to NeoPixel mapping
void updatePressurePixels() {
  int heel = analogRead(pressurePinHeel);
  int arch = analogRead(pressurePinArch);
  int ball = analogRead(pressurePinBall);
  int toe = analogRead(pressurePinBigToe);

  pixel.setPixelColor(0, pressureToColor(heel));
  pixel.setPixelColor(1, pressureToColor(arch));
  pixel.setPixelColor(2, pressureToColor(ball));
  pixel.setPixelColor(3, pressureToColor(toe));
  pixel.show();

  Serial.printf("Heel: %i, Arch: %i, Ball: %i, Toe: %i\n", heel, arch, ball, toe);
}

// // BME280 temperature to NeoPixel alert
// void updateTempPixels() {
//   float tempC = bme.readTemperature();
//   tempf = (tempC * 9.0 / 5.0) + 32.0;
//   Serial.printf("Temp (F): %.2f\n", tempf);

//   uint32_t color = 0;
//   if (tempf >= 82 && tempf < 86) color = pixel.Color(0, 0, 255);       // Blue
//   else if (tempf >= 86 && tempf < 90) color = pixel.Color(255, 255, 0); // Yellow
//   else if (tempf >= 90 && tempf < 94) color = pixel.Color(255, 0, 0);   // Red

//   if (color != 0) {
//     for (int i = 0; i < PIXELCOUNT; i++) {
//       pixel.setPixelColor(i, color);
//     }
//     pixel.show();
//   }
// }

// Pressure value to color
uint32_t pressureToColor(int pressure) {
  if (pressure < 120) return pixel.Color(0, 0, 255);         // Blue
  else if (pressure < 240) return pixel.Color(255, 255, 0);  // Yellow
  else return pixel.Color(255, 0, 0);                        // Red
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}
bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;
  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}
