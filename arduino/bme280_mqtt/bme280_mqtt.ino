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


  testet mot WEMOS LOLIN32
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define tries 2

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
//char* mqtt_server = "skule.sormo.no";
char* mqtt_server = "192.168.10.16";

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;
String filnavn, ssid1, password1 ;
char charBuf[50];
char charBuf1[50];
int teller = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial.begin(115200);

  delay(10);
  Serial.flush();
  Serial.println("\n********************************");
 String filnavn = String(__FILE__);

  Serial.println("Fil: " + String(__FILE__));
  Serial.print("Kompilert: "); Serial.println(__TIMESTAMP__);

String  ssid1 = "2.4G";
String   password1 = "12345abc";
  Serial.print("SSID: "); Serial.print(ssid1); Serial.print(" PSWD: "); Serial.println(password1);
  Serial.print("Prøver " + ssid1 + "..");
  WiFi.mode(WIFI_STA);
    WiFi.begin(charBuf, charBuf1);
  ssid1.toCharArray(charBuf, 20);
  password1.toCharArray(charBuf1, 20);
  
  Serial.println("charBuf:\t" + String(charBuf) + "\tSSID:\t" + String(charBuf1));
 WiFi.begin("2.4G", "12345abc");

  while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (teller++ < tries))
  {
    Serial.print(".");
    delay(10);
  };


  client.setServer(mqtt_server, 1883); // 1883 er default lister port.
  //  client.setCallback(callback);
  Serial.print("Prover MQTT server:");
  teller = 0;
  boolean connected1 = false;

  while (!client.connected() && (teller++ < 4) && (!connected1)) {
    Serial.print(".");
    // Create a random client ID
    String clientId = "1234";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    Serial.println("clientid: " + clientId);
    if (client.connect(clientId.c_str()))
    {
      connected1 = true;
      Serial.println(" connected");
      // Once connected, publish an announcement...
      client.publish("metrologi/mqtt1", mqtt_server);
      String(__FILE__).toCharArray(charBuf, 50);
      client.publish("metrologi/file", charBuf);
      ssid1.toCharArray(charBuf, 20);
      client.publish("meteorologi/wifi1", charBuf);
      client.publish("meteorologi", "status1");
      client.publish("meteorologi", "status2");
    }
 
  }



  if (! bme.begin(0x76, &Wire)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  Serial.println("normal mode, 16x oversampling for all, filter off,");
  Serial.println("0.5ms standby period");
  delayTime = 5000;


  // For more details on the following scenarious, see chapter
  // 3.5 "Recommended modes of operation" in the datasheet

  /*
      // weather monitoring
      Serial.println("-- Weather Station Scenario --");
      Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
      Serial.println("filter off");
      bme.setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1, // temperature
                      Adafruit_BME280::SAMPLING_X1, // pressure
                      Adafruit_BME280::SAMPLING_X1, // humidity
                      Adafruit_BME280::FILTER_OFF   );

      // suggested rate is 1/60Hz (1m)
      delayTime = 60000; // in milliseconds
  */

  /*
      // humidity sensing
      Serial.println("-- Humidity Sensing Scenario --");
      Serial.println("forced mode, 1x temperature / 1x humidity / 0x pressure oversampling");
      Serial.println("= pressure off, filter off");
      bme.setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1,   // temperature
                      Adafruit_BME280::SAMPLING_NONE, // pressure
                      Adafruit_BME280::SAMPLING_X1,   // humidity
                      Adafruit_BME280::FILTER_OFF );

      // suggested rate is 1Hz (1s)
      delayTime = 1000;  // in milliseconds
  */

  /*
      // indoor navigation
      Serial.println("-- Indoor Navigation Scenario --");
      Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
      Serial.println("0.5ms standby period, filter 16x");
      bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                      Adafruit_BME280::SAMPLING_X2,  // temperature
                      Adafruit_BME280::SAMPLING_X16, // pressure
                      Adafruit_BME280::SAMPLING_X1,  // humidity
                      Adafruit_BME280::FILTER_X16,
                      Adafruit_BME280::STANDBY_MS_0_5 );

      // suggested rate is 25Hz
      // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
      // T_ovs = 2
      // P_ovs = 16
      // H_ovs = 1
      // = 40ms (25Hz)
      // with standby time that should really be 24.16913... Hz
      delayTime = 41;
  */

  /*
    // gaming
    Serial.println("-- Gaming Scenario --");
    Serial.println("normal mode, 4x pressure / 1x temperature / 0x humidity oversampling,");
    Serial.println("= humidity off, 0.5ms standby period, filter 16x");
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X1,   // temperature
                  Adafruit_BME280::SAMPLING_X4,   // pressure
                  Adafruit_BME280::SAMPLING_NONE, // humidity
                  Adafruit_BME280::FILTER_X16,
                  Adafruit_BME280::STANDBY_MS_0_5 );

    // Suggested rate is 83Hz
    // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5)
    // T_ovs = 1
    // P_ovs = 4
    // = 11.5ms + 0.5ms standby
    delayTime = 12;
  */

  Serial.println();
}


void loop() {
//  }void l(){
  // Only needed in forced mode! In normal mode, you can remove the next line.
  bme.takeForcedMeasurement(); // has no effect in normal mode

  printValues();
  delay(delayTime);
}


void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  String(bme.readTemperature()).toCharArray(charBuf, 50);
  client.publish("meteorologi/Temp", charBuf);

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  String(bme.readPressure()/100).toCharArray(charBuf, 50);
   Serial.print("charbuf  "); Serial.println(charBuf);
 client.publish("meteorologi/Pres", charBuf);

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  String(bme.readHumidity()).toCharArray(charBuf, 50);
  client.publish("meteorologi/Hum", charBuf);

  Serial.println();
}
