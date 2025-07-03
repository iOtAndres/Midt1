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
#include "DS1307.h"
#include "FreeRTOS.h"
#include "IoTtimer.h"
#include "math.h"

 //void goManual();
 //void goAuto();

 //void showInfo();
 //void dayMode();
void WOKEUPMode2();
void SETTIMEMode1();

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

// OLED
const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);
//bool status;
const int HEXADDY = 0x3C;


// Encoder
const int ENCODER_A = D8;
const int ENCODER_B = D9;
Encoder myEnc(ENCODER_A, ENCODER_B);
const int ENCODER_BUTTON = D4;
float y;
int freq;
int position;
int lposition;


// RTC
RTC_DS1307 rtc; // FIXME

// Alarm state
bool alarmSet = false;
bool alarmTriggered = false;
bool mode2Active = false;
int alarmHour;
int alarmMinute;

DateTime now;
//DateTime daysOfTheWeek("Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday");
//code from ddavis
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

char timeInADay[12][24] = {"1am", "2am", "3am", "4am", "5am", "6am", "7am,", "8am", "9am", "10am", "11am,", "12am"};


//float lastSecond, lastMinute, lastDaySecond, lastNightSecond; 

float dayTimer;
float lightTimer;
int startHour = 5, endHour = 6, startMin = 45, endMin = 0;


// Buttons   
Button AWAKE_BUTTON (S1);
const int Button =S1;

// hue lights = 5
// led lights 5 total  Sunrise animation
int sunriseStep = 0;
 const int lastSunriseUpdate = 0;
int ambientPulse;

int brightness;
bool lightPower, lightNoPower;
//=1; 
int HUEY;
//= 2;
//have hue 1-3 power on at same time
int color=0;
const int ambientPulseStart = 0;
bool pulseIncreasing = true;
int ambientBrightness = 0;
bool orchestrateSunrise;

// NeoPixel strip light
const int STRIP_LIGHT_PIN = D2;
const int stripLightCount = 8;

int neoStripLight;

int lightIndex;

const int r = 255, g = 183, b = 76;
const int fadeInterval = 1000;



// const int HEXCOLOR = #FFB764; need to fix
//const int HEXDISPLAY = 0X3C;   need to fix
Adafruit_NeoPixel stripLights(stripLightCount,SPI1,WS2812B);

// have neo strip mimick the hues

// plug ins wemo/usbs

const int MYWEMO= 1;
const int MYWEMO1 = 2;
const int MYWEMO2 = 3;

bool ONOFF = false;
bool buttonClick;
bool manual = false;  

const int mode2Start = 0;
const int ONWEMO3 = 0;
unsigned int currentTime = millis();
unsigned int lastTime= 0;

const int EBIKE= 3;
const int TV =2;
const int MUSIC = 1;

int wemoStart;
int wemoActive;
int wemo1Active;
int wemo1Start;

int nowMillis;

bool enterMode2;

int wemo2Start;
int wemo2Active;


//int now;


//timesInADAY

void testdrawcircle(void);
void startscrollright(void);



void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected,15000);
  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
  //when wifi is connected or boot up screen leds turn on so = (led,high)
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

  
  //boot up screen
display.begin(SSD1306_SWITCHCAPVCC, HEXADDY);
display.display(); // show splashscreen
display.clearDisplay();
 if ((currentTime-lastTime)<2000){
    currentTime = lastTime;
    display.clearDisplay();
 }
//display.clearDisplay();   // clears the screen and buffer

// draw mulitple circles
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
  display.println("SAINT CASAN >>>> engineering");
  display.display();
  display.clearDisplay();

 display.clearDisplay();
  testdrawcircle();
  display.display();

  if ((currentTime-lastTime)<2000){
    currentTime = lastTime;
  display.clearDisplay();
  display.clearDisplay();
  }
 

// text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("SET NoTo NIKOLA ALARM\n");
  Serial.printf("SET NoTO NIKOLA ALARM\n");
  display.display();
  delay(2000);
  display.clearDisplay();

  //manual menu

  // if (ENCODER_BUTTON.isClicked){
  //    switch (timesInADAY);
  //    0=ENCODER_BUTTON;
  
  // TIME()
  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setCursor(0,0);
  // display.printf("THRESHOLD VALUES\n");



  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  pinMode(Button, INPUT);

   //pinMode(ONWEMO1,ONWEMO2,ONWEMO3, OUTPUT);
  // digitalWrite(ONWEMO1,ONWEMO2,ONWEMO3,LOW);
  //     digitalWrite(unsigned int, LOW);
  // // pinMode(HUEY1,HUEY2,HUEY3, HUEY4,HUEY5, OUTPUT);
  //   digitalWrite(HUEY1,HUEY2,HUEY3, HUEY4,HUEY5,LOW);


  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  stripLights.begin();
  stripLights.setBrightness(0);
  stripLights.show();


      //DISPLAY TIME SELECT
       Wire.begin();
       rtc.begin();
        if (!rtc.isrunning()) {
        Serial.println("RTC not running, setting to compile time.");
        rtc.adjust(DateTime(2025,7,2,5,59,59));// year, month, day, hour, min, sec
        }

          if (! rtc.begin()) {
          Serial.println("Couldn't find RTC");
          while (1);
          }

           if (rtc.begin()) {
          Serial.println("RTC lost power, lets set the time!");
           // following line sets the RTC to the date & time this sketch was compiled
          rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
           // This line sets the RTC with an explicit date & time, for example to set
          // January 21, 2014 at 3am you would call:
          // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
          }
        //  if (buttonClick = false){
        // mywemoWrite(MUSIC, LOW);
        // mywemoWrite(TV, LOW);
        // mywemoWrite(EBIKE, LOW);
          
}

 void loop() {
//program alarm

now = rtc.now();

freq = myEnc.read()/4;
if (freq <1){
freq= 1;
}
if (freq >12){
freq= 12;
}

position = myEnc.read();


//set encoder to a poition
if (position != lposition){
  //! not equal
//Serial.printf("%i/n", position);
position = lposition;
}

// t = millis() /1000.0;

// y=127.5*sin(2*M_PI*freq*t)+127.5;
// // so we sub t in the formula and have frequency in 
// analogWrite(BLUELED, y, 50000.0);



        // if (HUEY <1){
        // HUEY= 1;
        // }
        // if (HUEY >6){
        // HUEY = 6;
        // }


        // if (MYWEMO <1){
        // MYWEMO= 1;
        // }
        // if (MYWEMO >3){
        // MYWEMO = 3;
        // }

        // if (stripLightCount >8){
        // stripLightCount = 8;
        // }


// pixel.setPixelColor(i, rcRed, rcGreen, rcBlue);
// pixel.show();
// pixel.clear();


//   if (ENCODER_BUTTON.isClicked()){
//     //set time
//     if(now.minute() >= startMin){
//       if((now.hour() >= startHour  && now.hour() <= endHour)){
//         SETTIMEMode1();
//     }
    

 // Set alarm time with encoder
      if (!alarmSet) {
        const int position = myEnc.read() / 4;
        alarmHour = constrain(5 + (position / 12), 5, 8);
        alarmMinute = (position % 12) * 5;

        if (digitalRead(ENCODER_BUTTON) == LOW) {
          alarmSet = true;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.setTextSize(1);
          display.printf("Alarm Set:\n%02d, AM", alarmHour, alarmMinute);
          display.display();
          delay(1000);
        }
        return;
      }
 
// Trigger alarm
        if (!alarmTriggered){
            now.hour() == alarmHour;
            now.minute() == alarmMinute;
        }
        else { 
          alarmTriggered = true;
          // wemo1Start = millis;
          wemo1Active = true;
          digitalWrite(MYWEMO1, HIGH);

          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(5,0);
        //display message
          display.println("WAKE UP @ %i!");
          display.display();
          display.clearDisplay();


        }
             // Enter Mode 2/wake up mode
        //!mode2Active 
        if (alarmTriggered && digitalRead(Button) == HIGH) {
          digitalWrite(neoStripLight, HIGH);

        }
        
        // Sunrise and ambient effects
        if (mode2Active and nowMillis - mode2Start <= 300000) {
          digitalWrite((nowMillis), HIGH);
          digitalWrite((nowMillis), HIGH);
        }

        // Stop Wemo1 after 5 seconds
        if (wemo1Active and nowMillis - wemo1Start >= 5000) {
          digitalWrite(MYWEMO, LOW);
          wemo1Active = false;
        }
         else {(digitalWrite(MYWEMO,HIGH));
        }
        // Turn off Wemo2 after 5 minutes
        if (mode2Active and nowMillis - mode2Start > 300000) {
          digitalWrite(MYWEMO1, LOW);
        }
         else {(digitalWrite(MYWEMO1,HIGH));
        }
        // Turn off Wemo3 after 1 hour
        if (mode2Active and nowMillis - wemo2Start > 300000) {
          digitalWrite(MYWEMO2, LOW);
        }
          else {(digitalWrite(MYWEMO2,HIGH));
        }
        if (neoStripLight < 5) {
            int color = stripLights.Color(r * brightness / 100, g * brightness / 100, b * brightness / 100);
            stripLights.setBrightness(lightIndex);
            stripLights.show();

            
            // syncRoomLight(lightIndex, brightness);
          }
    
          int color = stripLights.Color(r * ambientBrightness / 100, g * ambientBrightness / 100, b * ambientBrightness / 100);
          for (int i = 5; i < 8; i++) {
            stripLights.setPixelColor(i, color);
          }
          stripLights.show();

          for (int i = 1; i < 6; i++) {
            setHue(HUEY,ONOFF,color,position,255);
          }
          Serial.printf(" HUEY %i, color %i, brightness %i, saturation %i\n",HUEY,color,position,255);
        }
      

      //void enterMode2(unsigned long nowMillis, DateTime now)
      // void enterMode2 (VOID){
      //   mode2Active = true;
      //   wemoStart = nowMillis;
      //   wemo1Start = nowMillis;
      //   wemo2Start = nowMillis;
      //   display.clearDisplay();
      //   display.setCursor(0, 0);
      //   display.setTextSize(2);
      //   display.printf("Time:\n%02d:%02d", now.hour(), now.minute());
      //   display.display();

        // digitalWrite(MYWEMO1, HIGH);
        // digitalWrite(MYWEMO2, HIGH);
        // digitalWrite(MYWEMO, HIGH);
        // }
        //   lastSunriseUpdate = now;
      
    
              //   if(!buttonClick){
              //     startHour = 5;
              //     startMin = 45;
              //     endHour = 6;
              //     endMin = 00; 
              //     manual = false;
              //   }

              // if (BUTTON.isClicked()){

                  
              //     (HUEY, HIGH);
              //     pixel.show();
              //     pixel.clear();
              //     (strip)
              //     (MYWEMO, HIGH);
              //     MYWEMO++;
              //   //buttonClick = !buttonClick;
              //   Serial.printf("%i\n",ENCODER_BUTTON);
              //   }

              // if(now.minute() >= startMin){
              //       if((now.hour() >= startHour  && now.hour() <= endHour)){
              //         SETTIMEMode1();
              //     }
              //     if((now.hour() < startHour) || (now.hour() > endHour)){
              //       WOKEUPMode2();
              //     }
              //   }

              // if (buttonClick){
              //     manual = true;
              //     //count2 = 0;
              //     //currentTime = millis();
              //     // while(count2 < 1){
              //     //   Serial.printf("in the while loop");
              //     //   goManual();
              //     //   count2++;
              //     // }
              //     if(now.minute() >= startMin){
              //       if((now.hour() >= startHour  && now.hour() <= endHour)){
              //         SETTIMEMode1();
              //     }
              //     if((now.hour() < startHour) || (now.hour() > endHour)){
              //       WOKEUPMode2();
              //     }
                  
              //   }
              // }

              //   //Serial.printf("Start hour: %i\n", startHour);

              // void goManual(){
              //     //int myenc[500,800];
              //       int twice = 0;
              //     while(twice < 2){
              //       //count = 0;
              //       display.clearDisplay();
              //       display.setCursor(0,0);
              //       display.setTextSize(2);
              //       display.printf("  MANUAL");
              //       display.setTextSize(1);
              //       if(twice == 0){
              //         display.printf("\n\nEnter the ON time!\nON: ");
              //       }
              //       if(twice == 1){
              //         display.printf("\n\nEnter the OFF time!\nOFF: ");
              //       }
              //       display.display();
                  
              //     }
                
              //     display.printf("%i:%i",startHour, startMin);
              //     display.display();  
              //     }

              // void (HUEY, )(){
              //   if(!lightPower){
              //     //Serial.printf("Turning Light ON");
              //     //currentTime = millis();
              //     if((millis() - lightTimer)> 1000)
              //     { 
              //       lightTimer = millis();
              //       Serial.printf("Light ON\n");
              //       for (int i = 0; i < 6; i++){
              //         setHue(lightsOn, true, HueOrange, brightness, 255);
              //       }
              //         brightness++;
              //       if (brightness >= 255){
              //         brightness = 0;
              //         lightPower = true;
              //       }
              //     }
              //   }
              // }


              // void lightOff(){
              //   if(!lightNoPower){
                  
              //     Serial.printf("Turning light off\n");
                  
              //     //currentTime = millis();
              //     if((millis() - dayTimer)> 1000){
                    
              //       dayTimer = millis();
              //       Serial.printf("Dimming\n");
              //       for(int i = 0; i < 6; i++){
              //         setHue(lightsOn, true, HueViolet, brightness, 255);
              //       }
              //         brightness--;
              //       if(brightness <= 0){
              //         brightness = 255;
              //         lightNoPower = true;
              //       }
              //     }
              //   }
              // }


void showInfo(){
    //lastSecond = millis();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.printf("  ");
    //display.print(daysOfTheWeek[now.daysOfTheWeek()]); NEED TO FIX
    display.printf("\n %i:%i:%i", now.hour(), now.minute(), now.second());
    Serial.printf("\n %i:%i:%i", now.hour(), now.minute(), now.second());
    display.printf("\n%.0f%c || %.0f%c");
    if(!manual){
      display.setTextSize(1);
      display.printf("\n AUTO");
    }
    else{
      display.setTextSize(1);
      display.printf("\n MANUAL");
    }
    }

  
    
// void SETTIMEMode1 (){
//   //currentTime = millis();
//   if (( millis() - dayTimer ) > 1000) {
//     dayTimer = millis();
//     Serial.printf("SETTIMEMode1\n");
//     showInfo();
//   }
//   if(Button==HIGH){
//    lightPower = true;
//       wemoWrite(EBIKE, HIGH);
//       wemoWrite(TV, HIGH);
//       wemoWrite(MUSIC, HIGH);
//     }
//      else{
//       lightPower = false;
//       wemoWrite(EBIKE, LOW);
//       wemoWrite(TV, LOW);
//       wemoWrite(MUSIC, LOW)
//      }
    // Serial.printf("Heater ON\nFan OFF\n");  
    // count3++;
    // }
  

  void WOKEUPMode2(){
  //currentTime = millis();
  if (( millis() - dayTimer ) > 1000) {
    dayTimer = millis();
    Serial.printf("WOKEUPMode2\n");
    showInfo();
  }
}


  // const float nowMillis = millis();


  // calculate a date which is 1 day into the future       do I need this?
  // DateTime future(now.millis() + 7 * 86400L);

  // Serial.print(" now + 1d: ");      
  // Serial.print(future.year(), DEC);
  // Serial.print('/');
  // Serial.print(future.month(), DEC);
  // Serial.print('/');
  // Serial.print(future.day(), DEC);
  // Serial.print(' ');
  // Serial.print(future.hour(), DEC);
  // Serial.print(':');
  // Serial.print(future.minute(), DEC);
  // Serial.print(':');
  // Serial.print(future.second(), DEC);
  // Serial.println();

  // Serial.println();
  // delay(3000);


 
  
// void displayMessage(String msg) {
//   display.clearDisplay();
//   display.setCursor(0, 0);
//   display.setTextSize(1);
//   display.println(msg);
//   display.display();
 
 
 void testdrawcircle(void){
  
  for (int16_t i=0; i<display.height(); i+=1) {
    display.drawCircle(display.width()/1, display.height()/1, i, WHITE);
    display.display();
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
  display.println("SAINT CASAN >>>> engineering");
  display.display();
  display.clearDisplay();
}
display.clearDisplay();
}
 
    

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