
/* 
-- P. A. M. -- 

(P)articulates in (A)ir (M)onitor

*/

// Required Libraries
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_PM25AQI.h>

//_________________________________________________________________________________________________________________________________

// -- SGP30 Set Up -- \\


// -- Library needed for SGP30

#include <Wire.h>
#include "Adafruit_SGP30.h"      //  Library to support use of sensor

Adafruit_SGP30 sgp;              //  Create an Adafruit_SGP30 object called "sgp"

/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [°C]
* @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}



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
  // -- SGP30 Set-up -- \\


  // -- Serial set up:
  Serial.begin(9600);
  delay(2000);         //   Insert a delay to let the Serial object start

  // -- Initialize sensor

  // -- If statement that checks if sensor is found, if not, it will send a message to the serial monitor.
  if (! sgp.begin()) {
    Serial.println("SGP30 Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

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

 // SGP30 Data Reading
 if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

   if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }
  Serial.print("Raw H2 "); Serial.print(sgp.rawH2); Serial.print(" \t");
  Serial.print("Raw Ethanol "); Serial.print(sgp.rawEthanol); Serial.println("");
 

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
dataString += String(sgp.TVOC); // Column 13
dataString += ",";
dataString += String(sgp.eCO2); // Column 14
dataString += ",";
dataString += String(sgp.rawH2); // Column 15
dataString += ",";
dataString += String(sgp.rawEthanol); // Column 16
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

  counter++;
  if (counter == 30) {
    counter = 0;

    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.println("Failed to get baseline readings");
      return;
    }
    Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
  }
  delay(60000);

}
