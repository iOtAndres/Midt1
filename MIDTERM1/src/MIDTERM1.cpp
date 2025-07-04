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
    RTC_DS1307 rtc; 

    // Alarm state
    bool alarmSet = false;
    bool alarmTriggered = false;
    bool mode2Active = false;
    int alarmHour;
    int alarmMinute;

    DateTime now;

    char timeInADay[12][24] = {"1am", "2am", "3am", "4am", "5am", "6am", "7am,", "8am", "9am", "10am", "11am,", "12am"};

    float dayTimer;
    float lightTimer;
    int startHour = 5, endHour = 6, startMin = 45, endMin = 0;

                // Buttons   
                Button AWAKE_BUTTON (S1);
                const int Button =S1;
                const int REDLED = (D6);
                // hue lights = 5
                // led lights 5 total  Sunrise animation
                int sunriseStep = 0;
                const int lastSunriseUpdate = 0;
                int ambientPulse;

                int brightness;
                bool lightPower, lightNoPower;

                int HUEY;

                //have hue 1-3 power on at same time neeed to fix or add to make sur eit works
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

bool enterMode2;
int wemoStart;
int wemoActive;
int wemo1Active;
int wemo1Start;
int nowMillis;
int wemo2Start;
int wemo2Active;


void testdrawcircle(void);
void startscrollright(void);



// Vibration motor
const int VIBRATION_PIN = D7;

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected,15000);
  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
  //when wifi is connected or boot up screen leds turn on so = (led,high)
  WiFi.connect();
  while(WiFi.connecting()) {
  digitalWrite(REDLED, HIGH);
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

  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  pinMode(Button, INPUT);
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);

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
          }
}


 void loop() {
//program alarm
  // Function declaration
void checkAlarm(RTC_DS1307 rtc, int alarmHour, int alarmMinute, bool &alarmTriggered);

  if (alarmSet) {
   checkAlarm;
 }
  }


void setAlarm() {
int position = 0;
int lastPosition = -1;
 while (true) {
 position = myEnc.read() / 4; // Adjust for encoder resolution
 if (position != lastPosition) {
 lastPosition = position;
 display.clearDisplay(); 
 display.setCursor(0, 0);
 display.print("Set Alarm: ");
 display.print(position % 12 + 1); // 1-12 hours
 display.print(":30"); // Fixed minutes for simplicity
 display.display();
 }
 if (digitalRead(ENCODER_BUTTON) == LOW) {
 alarmHour = position % 12 + 1;
 alarmMinute = 30;
 alarmSet = true;
 display.clearDisplay();
 display.setCursor(0, 0);
 display.print("Alarm set for ");
 display.print(alarmHour);
 display.print(":");
 display.print(alarmMinute);
 display.display();
 delay(2000);
 display.clearDisplay();
 break;
 }
 }
}

void checkAlarm() {
 now = rtc.now();
 if (now.hour() == alarmHour && now.minute() == alarmMinute) {
 if (!alarmTriggered) {
 alarmTriggered = true;
 display.clearDisplay();
 display.setCursor(0, 0);
 display.print("Wake up! It's ");
 display.print(alarmHour);
 display.print(":");
 display.print(alarmMinute);
 display.display();
 digitalWrite(VIBRATION_PIN, HIGH); // Turn on the vibration motor
 }
 } else {
 alarmTriggered = false;
 }

 if (alarmTriggered && digitalRead(Button) == LOW) {
 alarmTriggered = false;
 display.clearDisplay();
 display.setCursor(0, 0);
 display.print("GOOD MORNING");
 display.display();
 delay(2000);
 display.clearDisplay();
 digitalWrite(VIBRATION_PIN, LOW); // Turn off the vibration motor }
}

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

  void WOKEUPMode2(){
  //currentTime = millis();
  if (( millis() - dayTimer ) > 1000) {
    dayTimer = millis();
    Serial.printf("WOKEUPMode2\n");
    showInfo();
  }
}

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