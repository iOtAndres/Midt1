/* 
 * Project Insole
 * Author: Andres
 * Date: 8/4/2025
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

#include "Particle.h"
#include "neopixel.h"
#include "Colors.h"
#include "BME280.h"
//every 5 seonds temp and pressure data will be 
//coverted to neo pixels and displayed to an app

//class
Adafruit_BME280 bme;
//class 
IoTTimer relay;
Adafruit_BME280 bme;

/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();

void HeelTemp();
void ArchTemp();
void BallTemp();
void ToeTemp();

void HeelPress();
void ArchPress();
void BallPress();
void ToePress();
SYSTEM_MODE(SEMI_AUTOMATIC);

const int PIXELCOUNT = 4;      
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);

//neopixel settings
int bri = 200; 
int neo;
int startpixel, endpixel;
int pixelsLit;

//pressureplate
int pressurePinBigToe = A3;
float fsrValueBigToe;
int pressurePinBall = A2;
float fsrValueBall;
int pressurePinArch = A1;
float fsrValueArch;
int pressurePinHeel = A0;
float fsrValueHeel;

//BME 
int tempc;
float tempHeelf, tempArchf, tempBallf, tempToef;
// float pressPA;
// float inhg;
// float humidRH;
bool status;
const int HEXADDY = 0x76;

SYSTEM_THREAD(ENABLED);

void setup() {
  Serial.begin(9600);
  pixel.begin();               
  pixel.setBrightness(bri);    
  pixel.show(); //clear pixels
  pixel.clear(); 
  //getting data from BME280
  status = bme.begin(HEXADDY);
  if (status == false) {
    Serial.printf("BME280 at address 0x%02 X failed to start", HEXADDY);

    relay.startTimer(500);
  }
  // pinMode(A0,)
}

void loop() {

void HeelTemp();
void ArchTemp();
void BallTemp();
void ToeTemp();

void HeelPress();
void ArchPress();
void BallPress();
void ToePress();

}

void HeelTemp() {  
   //heel temp to neo pixel
   tempf= ((tempc*9.0/5)+32);//concert c to farenheight
   Serial.printf("heel tempf =%f \n",tempHeelf); 
    if (tempHeelf >= 82 && tempHeelf <= 86) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (tempHeelf >= 86 && tempHeelf <= 90) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (tempHeelf >= 90 && tempHeelf <= 94) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms


}
void ArchTemp() {
  //arch temp to neo pixel
   tempf= ((tempc*9.0/5)+32);//concert c to farenheight
   Serial.printf("arch tempf =%f \n",tempArchf); 
    if (tempArchf >= 82 && tempArchf <= 86) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (tempArchf >= 86 && tempArchf <= 90) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (tempArchf >= 90 && tempArchf <= 94) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms


}
void BallTemp() {
  //ball temp to neo pixel
   tempf= ((tempc*9.0/5)+32);//concert c to farenheight
   Serial.printf("ball tempf =%f \n",tempBallf); 
    if (tempBallf >= 82 && tempBallf <= 86) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (tempBallf >= 86 && tempBallf <= 90) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (tempBallf >= 90 && tempBallf <= 94) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms

}
void ToeTemp()  {
  //toe temp to neo pixel
   tempf= ((tempc*9.0/5)+32);//concert c to farenheight
   Serial.printf("toe tempf =%f \n",tempToef); 
    if (tempToef >= 82 && tempToef <= 86) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (tempToef >= 86 && tempToef <= 90) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (tempToef >= 90 && tempToef <= 94) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms


}

void HeelPress(){ //heel pressure to neo pixel
  fsrValueHeel = analogRead (pressurePinHeel);//read heel pressure 
  Serial.printf("heel pressure =%f \n",fsrValueHeel); 
    if (fsrValueHeel >= 0 && fsrValueHeel <= 120) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (fsrValueHeel >= 120 && fsrValueHeel <= 240) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (fsrValueHeel >= 240 && fsrValueHeel <= 360) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms
  

}
void ArchPress(){  //arch pressure to neo pixel
  fsrValueArch = analogRead (pressurePinArch);//read heel pressure 
  Serial.printf("arch pressure =%f \n",fsrValueHeel); 
    if (fsrValueArch >= 0 && fsrValueArch <= 120) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (fsrValueArch >= 120 && fsrValueArch <= 240) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (fsrValueArch >= 240 && fsrValueArch <= 360) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms


}
void BallPress(){  //ball pressure to neo pixel
  fsrValueBall = analogRead (pressurePinBall);//read heel pressure 
  Serial.printf("ball pressure =%f \n",fsrValueBall); 
    if (fsrValueBall >= 0 && fsrValueBall <= 120) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (fsrValueBall >= 120 && fsrValueBall <= 240) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (fsrValueBall >= 240 && fsrValueBall <= 360) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms

}
void ToePress(){
      //big toe pressure to neo pixel
  fsrValueBigToe = analogRead (pressurePinBigToe);//read heel pressure 
  Serial.printf("toe pressure =%f \n",fsrValueBigToe); 
    if (fsrValueBigToe >= 0 && fsrValueBigToe <= 120) {
      // Turn NeoPixel blue
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_BLUE));
    } 
    else (fsrValueBigToe >= 120 && fsrValueBigToe <= 240) {
      // Turn NeoPixel yellow
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_YELLOW));
    } 
    else (fsrValueBigToe >= 240 && fsrValueBigToe <= 360) {
      // Turn NeoPixel red
      pixels.setPixelColor(0, pixels.Color(RGB_COLOR_RED));
    }
  pixels.show();
  delay(100); // Sample every 100ms

}