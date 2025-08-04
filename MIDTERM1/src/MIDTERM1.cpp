/* 
 * Project MidTerm_1
 * Author: Andres
 * Date: 6/30/2025
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */





#include "Particle.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Button.h"
#include "Neopixel.h"
#include "Colors.h"
#include "Encoder.h"
#include "hue.h"
#include "RTClib.h"
#include "FreeRTOS.h"
#include "IoTtimer.h"
#include "math.h"

// Function declarations
void WOKEUPMode2();
void SETTIMEMode1();
void testdrawcircle(void);
void startscrollright(void);
void setClock();

SYSTEM_MODE(MANUAL);

// OLED
const int OLED_RESET = -1;
const int HEXADDY = 0x3C;
Adafruit_SSD1306 display(OLED_RESET);

// RTC
RTC_DS1307 rtc;

// Alarm state
DateTime now;
int alarmHour;
int alarmMinute;
bool alarmSet;
bool alarmTriggered = false;
bool mode2Active = false;
char timeInADay[12][24] = {"1am", "2am", "3am", "4am", "5am", "6am", "7am,", "8am", "9am", "10am", "11am,", "12am"};
float dayTimer;
float lightTimer;
unsigned int lastDisplay;
int i;
// Encoder
const int ENCODER_A = D8;
const int ENCODER_B = D9;
Button ENCODER_BUTTON(D4);
Encoder myEnc(ENCODER_A, ENCODER_B);
int position;
int lposition;
float y;

// Buttons
const int buttonPin = S1;
Button wakeUpbutton(buttonPin);
const int REDLED = D6;

// Vibration motor
const int VIBRATION_PIN = D7;

// NeoPixel strip light
const int STRIP_LIGHT_PIN = D2;
const int stripLightCount = 8;
Adafruit_NeoPixel stripLights(stripLightCount, SPI1, WS2812B);
//const int fadeInterval = 1000;
int neoStripLight;
int lightIndex;

// Time position for encoder to match time
int hour;
int minute;
int lastHour;
int lastMinute;
int count;

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected, 15000);
    WiFi.on();
    WiFi.clearCredentials();
    WiFi.setCredentials("IoTNetwork");
    WiFi.connect();
    while (WiFi.connecting()) {
        digitalWrite(REDLED, HIGH);
        Serial.printf(".");
    }
    Serial.printf("\n\n");
    DateTime now = rtc.now();
    Wire.begin();
    rtc.begin();
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }
    if (rtc.begin()) {
        Serial.println("RTC lost power, let's set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    display.begin(SSD1306_SWITCHCAPVCC, HEXADDY);
    // draw mulitple circles
  testdrawcircle();
  display.display();
  delay(2000);
  display.clearDisplay();
   

    
       //DateTime now = rtc.now();


   Serial.printf("%i:%i\n", now.hour(), now.minute());
    alarmSet = true;
 

    Serial.print(now.year());
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    alarmSet = true; 

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.printf("Set NoTo NIKOLA ALARM\n");
    Serial.printf("Set NoTO NIKOLA ALARM\n");
    display.display();
    display.clearDisplay();
    display.clearDisplay();
   
        while (count < 2) {
            if (ENCODER_BUTTON.isClicked()) {
                count++;
            }
            if (count == 0) {
                hour = myEnc.read() / 4; // Adjust for encoder resolution
                if (hour != lastHour) {
                    lastHour = hour;
                    hour = ((hour % 12) + 1);
                    if (hour <= 1) {
                        hour = 1;
                    }

                    display.setTextSize(1);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 0);                    
                    display.clearDisplay();
                }
            }

            if (count == 1) {
                minute = myEnc.read() / 4; // Adjust for encoder resolution
                if (minute != lastMinute) {
                    lastMinute = minute;
                    minute = (minute % 60);
                    if (minute <= 1) {
                        minute = 1;
                    }
                    display.setTextSize(1);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 0);
                    display.printf("minute = %i\n", minute);
                    Serial.printf("minute = %i\n", minute);
                    display.display();
                    display.clearDisplay();
                }
            }
            if (count == 2) {
                alarmSet = false;
                count = 0;
                pinMode(VIBRATION_PIN, OUTPUT);
                digitalWrite(VIBRATION_PIN, HIGH);
            }
        }


    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.printf("ALARM SET FOR %i : %i\n", hour, minute);
    Serial.printf("ALARM SET FOR %i : %i\n", hour, minute);
    display.display();
    delay(10000);
    display.clearDisplay();

  digitalWrite(VIBRATION_PIN, LOW);
    stripLights.begin();
    stripLights.setBrightness(0);
    stripLights.show();
    }
// void loop() {
//     DateTime now = rtc.now(); // Update the current time
//     Serial.printf("Current Time: %02d:%02d\n", now.hour(), now.minute());
//     Serial.printf("Alarm Time: %02d:%02d\n", hour, minute);
//     if (now.hour() == hour && now.minute() == minute && !alarmTriggered) {
//         alarmTriggered = true; // Set the alarm triggered
//         digitalWrite(VIBRATION_PIN, HIGH); // Trigger the vibration motor
//         //setNeoPixelBrightness(now.hour()); // Set NeoPixel brightness based on time
//         display.setTextSize(1);
//         display.setTextColor(WHITE);
//         display.setCursor(0, 0);
//         display.printf("Wake up, it's %02d:%02d\n", hour, minute);
//         display.display();
//     }

//     if (wakeUpbutton.isClicked()) {
//         digitalWrite(VIBRATION_PIN, LOW); // Turn off the vibration motor
//         stripLights.setBrightness(0); // Turn off the NeoPixel strip light
//         stripLights.show();
//         alarmTriggered = false; // Reset the alarm triggered flag
//         display.setTextSize(1);
//         display.setTextColor(WHITE);
//         display.setCursor(0, 0);
//         display.printf("Good morning\n");
//         DateTime now = rtc.now(); // Get the current time
//         display.printf("Current Time: %02d:%02d\n", now.hour(), now.minute());
//         display.display();
//         delay(5000); // Display the message for 5 seconds
//         display.clearDisplay();
//         display.setTextSize(1);
//         display.setTextColor(WHITE);
//         display.setCursor(0, 0);
//         display.printf("Set NoTo NIKOLA ALARM\n");
//         Serial.printf("Set NoTO NIKOLA ALARM\n");
//         display.display();
//         alarmSet = true; // Reset the alarm set flag
//     }
// }

// void setNeoPixelBrightness(int hour) {
//     int brightness;
//     uint32_t color;

//     if (hour >= 1 && hour < 4) {
//         // Early morning colors
//         if (hour == 1) {
//             color = stripLights.Color(0, 0, 128); // Deep blue
//         } else if (hour == 2) {
//             color = stripLights.Color(0, 0, 255); // Lighter blue
//         } else if (hour == 3) {
//             color = stripLights.Color(75, 0, 130); // Hint of purple
//         }
//         brightness = 64; // Lower brightness for early morning
//     } else if (hour >= 4 && hour < 7) {
//         // Dawn to early morning colors
//         if (hour == 4) {
//             color = stripLights.Color(255, 140, 0); // Soft orange
//         } else if (hour == 5) {
//             color = stripLights.Color(255, 215, 0); // Warm yellow
//         } else if (hour == 6) {
//             color = stripLights.Color(255, 255, 0); // Bright yellow
//         }
//         brightness = map(hour, 4, 6, 128, 192); // Adjust brightness based on time
//     } else if (hour >= 7 && hour < 10) {
//         // Morning colors
//         if (hour == 7) {
//             color = stripLights.Color(255, 255, 0); // Bright yellow
//         } else if (hour == 8) {
//            // color = neoStripLight


//         }
//     }
// }



void testdrawcircle(void){
  
  for (int16_t i=0; i<display.height(); i+=1) {
   display.drawCircle(display.width()/1, display.height()/1, i, WHITE);
   display.display();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(5,0);
   display.println("SAINT CASAN >>>>\n, engineering");
   display.display();
   display.clearDisplay();
}
display.clearDisplay();
}
// void setNeoPixelBrightness(int hour) {
//     int brightness;
//     uint32_t color;

//     if (hour >= 1 && hour < 4) {
//         // Early morning colors
//         if (hour == 1) {
//             color = stripLights.Color(0, 0, 128); // Deep blue
//         } else if (hour == 2) {
//             color = stripLights.Color(0, 0, 255); // Lighter blue
//         } else if (hour == 3) {
//             color = stripLights.Color(75, 0, 130); // Hint of purple
//         }
//         brightness = 64; // Lower brightness for early morning
//     } 
//     else if (hour >= 4 && hour < 7){
//         // Dawn to early morning colors
//         if (hour == 4) {
//             color = stripLights.Color(255, 140, 0); // Soft orange
//         } else if (hour == 5) {
//             //color = strip
//   }
// }


    

// wemo code 
//   clicked =digitalRead(BUTTON);
//    if (ONWEMO.isClicked()){
//     //boolean on off
//    onOFF =!onOFF;
//    }
//    //so code for when on function is true to write turn on wemo
//    if(onOFF==TRUE){
//   wemoWrite(MYWEMO,HIGH);
//    Serial.printf("Turning on Wemo# %i\n",MYWEMO);
//    }
//    //so code for when on function is false to write turn off wemo
//  if (onOFF ==FALSE){
//   wemoWrite(MYWEMO,LOW);
//   Serial.printf("Turning off Wemo# %i\n",MYWEMO);


//this code loops ring
// for (int16_t i=0; i<display.height(); i+=1) {
//     display.drawCircle(display.width()/1, display.height()/1, i, WHITE);
//     display.display();
//       display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(5,0);
//   display.println("SAINT CASAN >>>> engineering");
//   display.display();
//   display.clearDisplay();
//   for (int16_t i=0; i<display.height(); i+=1) {
//     display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
//     display.display();
//     display.clearDisplay();
 // }



















//   //tposition! = tposition
//   display.clearDisplay(); 
//   display.setCursor(0, 0);
//   display.printf("Set Alarm: ");
//   display.print(position % 12 + 1); // 1-12 hours
//   display.print(":30"); // Fixed minutes for simplicity
//   display.display();
  
  
  


//   // if (digitalRead(ENCODER_BUTTON) == HIGH) {
//   // alarmHour = position % 12 + 1;
//   // alarmMinute = 30;
//   // alarmSet = true;
//   display.setCursor(0, 0);
//   Serial.printf("Alarm set for ");
//   display.print(alarmHour);
//   display.printf(":");
//   display.print(alarmMinute);
//   display.display();
//   delay(2000);
//   display.clearDisplay();
//  // break;


// void setAlarm() {
  // int position = 0;
  //int lastPosition = -1;
  // while (true) {
  // set.hour = myEnc.read() / 4; // Adjust for encoder resolution
  // if (ENCODER_BUTTON.isCLicked) 
  // (count = 1; count <= 2, count ++);
  // }



