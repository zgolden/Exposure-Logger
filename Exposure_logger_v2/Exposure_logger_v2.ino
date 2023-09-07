
/* 
-- P. A. M. -- 

(P)articulates in (A)ir (M)onitor

*/
/:) 
// Required Libraries
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_PM25AQI.h>
#include "DHT.h"

//_________________________________________________________________________________________________________________________________

// -- DHT22 AM2302 Set-Up

#include "DHT.h"

#define DHTPIN 5     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);



//_________________________________________________________________________________________________________________________________

// -- PM2.5 Set Up -- \\

#include "Adafruit_PM25AQI.h"

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

//_________________________________________________________________________________________________________________________________

// -- SD Card Set up -- \\

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

void setup() {
  
  Serial.begin(9600);
  
 // -- DHT 22 Void Set Up -- \\

  dht.begin();
  
 //______________________________________________________________________________________________________________________________

 //-- PLANTOWER PM2.5 Sensor set up --\\
 
  // There are 3 options for connectivity!
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
}

//______________________________________________________________________________________________________________________________

//-- SD Card Set Up --\\
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

}
int counter = 0;

void loop() {


  String dataString = "";



  PM25_AQI_Data data;

  
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(100);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");



 // DHT22 Data reading
 
 // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
 

// Creating the string to write to the file
  
dataString += String(data.pm10_standard); // Column 1 (this label is to make my life easier writing matlab code)
dataString += ",";
dataString += String(data.pm25_standard); // Column 2
dataString += ",";
dataString += String(data.pm100_standard); // Column 3
dataString += ",";
dataString += String(data.pm10_env); // Column 4
dataString += ",";
dataString += String(data.pm25_env); // Column 5
dataString += ",";
dataString += String(data.pm100_env); // Column 6
dataString += ",";
dataString += String(data.particles_03um); // Column 7
dataString += ",";
dataString += String(data.particles_05um); // Column 8
dataString += ",";
dataString += String(data.particles_10um); // Column 9
dataString += ",";
dataString += String(data.particles_25um); // Column 10
dataString += ",";
dataString += String(data.particles_50um); // Column 11   
dataString += ",";
dataString += String(data.particles_100um); // Column 12
dataString += ",";
dataString += String(h); // Column 13 humidity
dataString += ",";
dataString += String(t); // Column 14 temperature celsius
dataString += ",";
dataString += String(f); // Column 15 temperature farenheit
dataString += ",";
dataString += String(hic); // Column 16 heat index celsius
dataString += ",";
dataString += String(hif); // Column 17 heat index farenheit
dataString += ",";
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
    delay(1000);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  delay(60000);

}
