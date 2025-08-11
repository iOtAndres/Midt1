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
#include "JsonParserGeneratorRK.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"
#include "GPS_CNM.h"

Adafruit_GPS GPS(&Wire);
struct GeoLocation {
  float lat;
  float lon;

};

GeoLocation myLoc;

const int PIXELCOUNT = 16;      
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
//neopixel settings
int bri = 200; 
int neo;
int startpixel, endpixel;
int pixelsLit;
int color;
int pressureHeel = A0;
int pixelON;

float subValue,pubValue;

unsigned int last, lastTime ,currentTime;




const int OLED_RESET=-1;
Adafruit_SSD1306 display(OLED_RESET);
// Define User and Credentials
// String password = "AA4104132968BA2224299079021594AB"; // AES128 password
// String myName = "Andres";

const int RADIOADDRESS = 0xC4; // DONT CHANGE Get address from Instructor, it will be a value between 0xC1 - 0xCF
const int TIMEZONE = -6;
const unsigned int UPDATE = 30000;
// int last;


// Define Constants
const int RADIONETWORK = 7;    // range of 0-16
const int SENDADDRESS = 302;   // address of radio to be sent to DONT CHANGE
// Declare Variables

const int MESSAGE = D7;

// Declare Functions
// void setPixels();

float latitude, longitude, altitude;

// Declare Variables 
float lat, lon, alt;
unsigned int lastGPS;//helps with timer moving along


void pixelFill(int start, int end,int Color);
void createEventPayLoad (GeoLocation myGPS);
void MQTT_connect();
bool MQTT_ping();
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);
/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 
// Run the application and system concurrently in separate threads

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe subFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/subfeed"); 
//Adafruit_MQTT_Publish pubFeed1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/randomNumber");
Adafruit_MQTT_Publish pubFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gps");


SYSTEM_THREAD(ENABLED);




void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);


      // Connect to Internet but not Particle Cloud
    WiFi.on();
    WiFi.connect();
    while(WiFi.connecting()) {
    Serial.printf(".");
    }
  Serial.printf("\n\n");
  myLoc.lat = 35.08449;
  myLoc.lon = -106.65114;

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
}

void loop() {
   pixelFill(0,neo,red);
  currentTime = millis () ;
if( currentTime - lastTime > 10000 ) {
	lastTime = millis();
  float lon;
  float lat;
   Serial.printf("random coordinates %f : %f \n",lon,lat); 
      } 

  MQTT_connect();
  MQTT_ping();


  // Adafruit_MQTT_Subscribe *subscription;
  // while ((subscription = mqtt.readSubscription(100))) {
  //   if (subscription == &subFeed) {
  //   }
  // }

  if((millis()-lastTime > 10000)) {
    if(mqtt.Update()) {
      myLoc.lon = random(-10665114,-10610000)/100000.0;
      myLoc.lat = random(3508449,3205421)/100000.0;
      lastTime = millis();
      Serial.printf("random Location : lon %f, lat %f\n", myLoc.lon, myLoc.lat); 
     createEventPayLoad (myLoc);
      } 
}
    // text display
    display.clearDisplay(); 
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.printf("Lat: %0.6f, Lon: %0.6f/n",lat, lon);
    display.display();

}

void pixelFill(int start, int end, int color){
  int i;
 for (i = start; i <= end; i++){
    pixel.clear();
    pixel.setPixelColor(i,color);
    pixel.show();
  }
}
void createEventPayLoad (GeoLocation myLoc){
//356 bytes
  JsonWriterStatic <256> jw;
  {
  JsonWriterAutoObject obj (& jw );
  jw.insertKeyValue ("lon", lon);
  jw.insertKeyValue ("lat", lat);
  
  }
pubFeed.publish (jw.getBuffer());
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
//onDataReceived is used to receive data from Bluefruit Connect App
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  uint8_t i;
  Serial.printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1],peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
  Serial.printf("Bytes:");
  for (i=0; i< len; i++){
  Serial.printf("%02X ", data[i]);
  }
  Serial.printf("\n");
  Serial.printf("Message: %s\n", (char*)data);

  //Serial.printf("encoder pos: %i\n", currentp);
}
//RUN CODE ON SERIAL MONITOR NO RX TO RX THEN GO TEST OUTSIDE  