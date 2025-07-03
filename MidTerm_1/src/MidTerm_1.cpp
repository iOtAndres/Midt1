/* 
 * Project MidTerm_1
 * Author: Andres
 * Date: 6/30/2025
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Button.h"
#include "Neopixel.h"
#include "Colors.h"
#include "Encoder.h"
#include "hue.h"
#include "DS1307.h"

const int OLED_RESET=-1;

Adafruit_SSD1306 display(OLED_RESET);


SYSTEM_MODE(MANUAL);
const int MYWEMOLED=_2;
const int MYWEMOTV=_1;
const int WAKEUPBUTTON = D3;
bool clicked;
bool onOFF;

//DS1307 clock;  -----------

SYSTEM_MODE(AUTOMATIC);
const int GREENLED =

Button WAKEUPBUTTON (BUTTON);

int i;
const int pixelCount = 96;
const int BRI = 30;----------------------
const int PINA = D8;
const int PINB = D9;
int currentp;
int lastp;

Encoder myEnc(PINA,PINB);



SYSTEM_THREAD(ENABLED);

void setup() {
Serial.begin(9600);
    clock.begin();

    //uncomment the next 4 lines to set a new date and time, then upload
    clock.fillByYMD(2025, 7, 1); //July 1,2025 (year, month, date)
    clock.fillByHMS(13, 55, 30); //1:55 PM 30 13:55:30 (hh:mm:ss)24 hr format
    clock.fillDayOfWeek(TUE);//Tuesday
    clock.setTime();//write time to the RTC chip

    //comment the 4 lines again then upload so that the date and time will not be reset


  waitFor(Serial.isConnected,10000);

  // USE GENERATED BITMAP CODE FOR THIS

  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C); -------------

// initialize with the I2C addr 0x3D (for the 128x64)

  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer

// draw mulitple circles
  testdrawcircle();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  display.clearDisplay();

 // miniature bitmap display
  display.clearDisplay();
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();


pixel.begin();
  pixel.setBrightness(BRI);
  pixel.show();




  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("WAKE UP @ \n");
  //----------------
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.printf("GOOD MORNING \n");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("CURRENT TIME\n");-------------------------
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  Serial.printf("WAKE UP @\n", n);
  Serial.printf("GOOD MORNING\n");
  Serial.printf("CURRENT TIME\n");
  display.display();
  delay(2000);
 





 waitFor(Serial.isConnected,15000);
 WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
  
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");
//on.startTimer(2000);

}


void loop() {

 if (!alarmSet) {
    long position = myEnc.read() / 4;
    alarmHour = constrain(5 + (position / 12), 5, 8);
    alarmMinute = (position % 12) * 5;

    if (digitalRead(ENCODER_BUTTON) == LOW) {
      alarmSet = true;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.printf("Alarm Set:\n%02d:%02d AM", alarmHour, alarmMinute);
      display.display();
      delay(1000);
}
    return;


for ( i = 0; i <= 45; i++){
pixel.setPixelColor(i, red);
pixel.show();
pixel.clear();
pixel.show();
delay(ECHO);
  }
  
}
void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("SAINT CASAN engineering");
  display.display();

  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();


    clicked =digitalRead(BUTTON);
   if (ONWEMO.isClicked()){
    //boolean on off
   onOFF =!onOFF;
   }
   //so code for when on function is true to write turn on wemo
   if(onOFF==TRUE){
  wemoWrite(MYWEMO,HIGH);
   Serial.printf("Turning on Wemo# %i\n",MYWEMO);
   }
   //so code for when on function is false to write turn off wemo
 if (onOFF ==FALSE){
  wemoWrite(MYWEMO,LOW);
  Serial.printf("Turning off Wemo# %i\n",MYWEMO);
}
clock.getTime();
    Serial.print(clock.hour, DEC);
    Serial.print(":");
    Serial.print(clock.minute, DEC);
    Serial.print(":");
    Serial.print(clock.second, DEC);
    Serial.print("  ");
    Serial.print(clock.month, DEC);
    Serial.print("/");
    Serial.print(clock.dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(clock.year + 2000, DEC);
    Serial.println();
//encoder for volume controller for setting timer 
// currentp = myEnc.read();
// currentp = 6*currentp-96;
// //set encoder to a poition
// if (currentp != lastp){
//   //! not equal
// myEnc.read("%i/n", currentp);
// currentp = lastp;

}