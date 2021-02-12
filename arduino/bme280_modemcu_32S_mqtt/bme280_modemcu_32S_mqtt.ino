/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#define SEALEVELPRESSURE_HPA (1013.25)


#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA 1013.25


#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "2.4G";
const char pass[] = "12345abc";

WiFiClient net;
MQTTClient client;

const char* mqtt_server = "192.168.10.16";

//WiFiClient espClient;
//MQTTClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
char charBuf[50];
char charBuf1[50];
int teller = 0;
String filnavn;

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

float hum, temp, trykk, hoyde,spenning;


void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("192.168.10.16", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}















void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("192.168.10.16", net);
  client.onMessage(messageReceived);

  connect();


  // Wire.begin(5, 4);
  delay(10);
  Serial.flush();
  Serial.println("\n********************************");
  filnavn = String(__FILE__);

  Serial.println("Fil: " + String(__FILE__));
  Serial.print("Kompilert: "); Serial.println(__TIMESTAMP__);

  while (!Serial);   // time to get serial running
  Serial.println(F("BME280 test"));

  //  Wire.begin(5, 4);
  Wire.begin();
  unsigned status;

  // default settings
  status = bme.begin(0x76, &Wire);
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  WiFi.begin("2.4G", "12345abc");
  // connect();

  Serial.println();


  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  ArduinoOTA.handle();
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }


  filnavn.toCharArray(charBuf, 50);
  client.publish("meteorologi1/fil", charBuf);
  client.publish("meteorologi1/versjon", "1.0.0");
  printValues();
  delay(1000);
}


void printValues() {

  temp = bme.readTemperature();
  trykk = bme.readPressure() / 100.0;
  hoyde = bme.readAltitude(SEALEVELPRESSURE_HPA);
  hum = bme.readHumidity();
  spenning=(5*analogRead(A0)*2/5120)*4.57/4.0;

  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(trykk);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(hoyde);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(hum);
  Serial.println(" %");

  
  Serial.print("Spenning = ");
  Serial.print(spenning);
  Serial.println(" V");

  String(temp).toCharArray(charBuf, 50);
  client.publish("meteorologi1/Temp", charBuf);
  String(trykk).toCharArray(charBuf, 50);
  client.publish("meteorologi1/Pres", charBuf);
  String(hum).toCharArray(charBuf, 50);
  client.publish("meteorologi1/Hum", charBuf);
  String(hoyde).toCharArray(charBuf, 50);
  client.publish("meteorologi1/hoyde", charBuf);
   String(spenning).toCharArray(charBuf, 50);
  client.publish("meteorologi1/spenning", charBuf);
  Serial.println();
}
