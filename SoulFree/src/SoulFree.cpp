/*
 * Project SoulFree
 * Description: Abuduction abolisher, gps, neo pixel/piezo sensors, smart charger box
 * Author: Andres 
 * Date: 8/8/25
 */

#include "Particle.h"
#include <Adafruit_GPS.h>
#include "Adafruit_SSD1306.h"
#include "IoTTimer.h"
#include "neopixel.h"
#include "Colors.h"

// #include "GPS_CNM"
const int PIXELCOUNT = 8;      
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
//neopixel settings
int bri = 200; 
int neo;
int startpixel, endpixel;
int pixelsLit;

int pressureHeel = A0;

unsigned int last, lastTime;


Adafruit_GPS GPS(&Wire);
const int OLED_RESET=-1;
Adafruit_SSD1306 display(OLED_RESET);
// Define User and Credentials
String password = "AA4104132968BA2224299079021594AB"; // AES128 password
String myName = "Andres";
const int RADIOADDRESS = 0xC4; // DONT CHANGE Get address from Instructor, it will be a value between 0xC1 - 0xCF
const int TIMEZONE = -6;
const unsigned int UPDATE = 30000;
int last;

IoTTimer LORATIMER;

// Define Constants
const int RADIONETWORK = 7;    // range of 0-16
const int SENDADDRESS = 302;   // address of radio to be sent to DONT CHANGE
// Declare Variables

const int MESSAGE = D7;

// Declare Functions
void getGPS(float *latitude, float *longitude, float *altitude, int *satellites);
void sendData(String name, float latitude, float longitude, int satelittes);
void reyaxSetup(String password);

float latitude, longitude, altitude;
int satellites;

// Declare Variables 
float lat, lon, alt;
int sat;//satellites
unsigned int lastGPS;//helps with timer moving along 

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);
  pixel.begin();
  pixel.setBrightness(bri);
  pixel.clear();
  pixel.show();


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  display.display(); // show splashscreen
  delay(500);
  display.clearDisplay();   // clears the screen and buffer
  display.display();

  Serial1.begin(115200);// SERIAL 1 IS another serial bus coming from lora, based on the component data sheet 
  reyaxSetup(password);

  //Initialize GPS
  GPS.begin(0x10);  // The I2C address to use is 0x10, can use i2 scanner, use our address
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);//sets what data we will be sneding to component
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); //sets rate we send data
  GPS.sendCommand(PGCMD_ANTENNA);// which antenna
  delay(1000);//pause
  GPS.println(PMTK_Q_RELEASE);//gps print data
  pinMode(MESSAGE,OUTPUT);

LORATIMER.startTimer(100);
}

void loop() {
  // listen for incoming lora messages and then send GPS back
  if (Serial1.available())  { // full incoming buffer: +RCV=203,50,35.08,9,-36,41 , IF DATA AVAIL THEN FORMAT RECIEVE = RCV
    String parse0 = Serial1.readStringUntil('=');  //+RCV WILL START AFTER = SIGN 
    String parse1 = Serial1.readStringUntil(',');  // address received from = HOW MUCH DATA IN BYTES 
    String parse2 = Serial1.readStringUntil(',');  // buffer length
    String parse3 = Serial1.readStringUntil(',');  // fuseSound GROUND SENSOR 
    String parse4 = Serial1.readStringUntil(',');  // fuseDust
    String parse5 = Serial1.readStringUntil(',');  // rssi SIGNAL STRETCH 
    String parse6 = Serial1.readStringUntil('\n'); // snr SIGNAL NOISE RADIO
    String parse7 = Serial1.readString();          // extra DATA STORED HERE

    Serial.printf("parse0: %s\nparse1: %s\nparse2: %s\nparse3: %s\nparse4: %s\nparse5: %s\nparse6: %s\nparse7: %s\n", parse0.c_str(), parse1.c_str(), parse2.c_str(), parse3.c_str(), parse4.c_str(), parse5.c_str(), parse6.c_str(), parse7.c_str());
    
    delay(100);//BIG S STRING SO WE WANT TO COVNERT TO AN ARRAY 
    getGPS(&latitude, &longitude, &altitude, &satellites);
    sendData(myName,lat, lon, sat);// REPLACE WITH THE GET DATA MODIFY 

    digitalWrite(D7,HIGH);
    LORATIMER.startTimer(5000);
   

  }
  if (LORATIMER.isTimerReady()){
     digitalWrite(D7,LOW);
  }
}

void getGPS(float *latitude, float *longitude, float *altitude, int *satellites) {
  int theHour;

  theHour = GPS.hour + TIMEZONE;
  if (theHour < 0) {
    theHour = theHour + 24;
  }

  Serial.printf("Time: %02i:%02i:%02i:%03i\n", theHour, GPS.minute, GPS.seconds, GPS.milliseconds);
  Serial.printf("Dates: %02i-%02i-%02i\n", GPS.month, GPS.day, GPS.year);
  Serial.printf("Fix: %i, Quality: %i", (int)GPS.fix, (int)GPS.fixquality);
     // text display
    display.clearDisplay(); 
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);

    display.printf("Time: %02i:%02i:%02i:%03i\n", theHour, GPS.minute, GPS.seconds, GPS.milliseconds);
    display.printf("Dates: %02i-%02i-%02i\n", GPS.month, GPS.day, GPS.year);
    display.printf("Fix: %i, Quality: %i", (int)GPS.fix, (int)GPS.fixquality);
    display.display();

  if (GPS.fix) {
    *latitude = GPS.latitudeDegrees;
    *longitude = GPS.longitudeDegrees;
    *altitude = GPS.altitude;
    *satellites = (int)GPS.satellites;
  }
}

void sendData(String name, float latitude, float longitude, int satelittes) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i,60,%f,%f,%i,%s\r\n", SENDADDRESS, latitude, longitude, satelittes, name.c_str());
  Serial1.printf("%s",buffer);//PRINT BUFFER 
  //Serial1.println(buffer); 
  delay(1000);
  if (Serial1.available() > 0)
  {
    Serial.printf("Awaiting Reply from send\n");
    String reply = Serial1.readStringUntil('\n');
    Serial.printf("Send reply: %s\n", reply.c_str());
  }
}

void reyaxSetup(String password) {
  // following four paramaters have most significant effect on range
  // recommended within 3 km: 10,7,1,7 = AROUND 
  // recommended more than 3 km: 12,4,1,7
  const int SPREADINGFACTOR = 10;
  const int BANDWIDTH = 7;
  const int CODINGRATE = 1;
  const int PREAMBLE = 7;
  String reply; // string to store replies from module

  Serial1.printf("AT+ADDRESS=%i\r\n", RADIOADDRESS); // set the radio address, R/ RETURN 
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply from address\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Reply address: %s\n", reply.c_str());
    display.clearDisplay(); 
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.printf("AT+ADDRESS=%i\r\n", RADIOADDRESS);
    display.display();
  }

  Serial1.printf("AT+NETWORKID=%i\r\n", RADIONETWORK); // set the radio network
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply from networkid\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Reply network: %s\n", reply.c_str());
  }

  Serial1.printf("AT+CPIN=%s\r\n", password.c_str());// GO THRU PASSWORD TO CONNECT 2 DEVICES
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply from password\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Reply: %s\n", reply.c_str());
  }

  Serial1.printf("AT+PARAMETER=%i,%i,%i,%i\r\n", SPREADINGFACTOR, BANDWIDTH, CODINGRATE, PREAMBLE);
  delay(200);
  if (Serial1.available() > 0) {
    reply = Serial1.readStringUntil('\n');
    Serial.printf("reply: %s\n", reply.c_str());
  }

  Serial1.printf("AT+ADDRESS?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Radio Address: %s\n", reply.c_str());
  }

  Serial1.printf("AT+NETWORKID?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Radio Network: %s\n", reply.c_str());
  }

  Serial1.printf("AT+PARAMETER?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("RadioParameters: %s\n", reply.c_str());
  }

  Serial1.printf("AT+CPIN?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Radio Password: %s\n", reply.c_str());
  }
}


//RUN CODE ON SERIAL MONITOR NO RX TO RX THEN GO TEST OUTSIDE  