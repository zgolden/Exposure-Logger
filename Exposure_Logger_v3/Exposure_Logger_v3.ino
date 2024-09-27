//     This version of the exposure logger includes an Adafruit SCD30 and a Plantower A03001S also put together by adafruit.
// Hopefully this code will log the data into an easy to analyze txt file, as well as display the current CO2, Temp, RH and PM2.5.
// The majority of the code is copied from the examples provided by Adafruit.

//- Libraries
#include <Wire.h> //these might be unnecesary but i dont know lol
#include <SPI.h>
#include <Adafruit_SCD30.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_PM25AQI.h"

//- Variables to declare
Adafruit_SCD30  scd30;
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, 4);

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Exposure Logger v3");

  delay(1000); // Waiting for everything to boot up

  // Try to initialize the SCD30
  if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { delay(10); }
  }
  Serial.println("SCD30 Found!");

  // Plantower Setup
  // There are 3 options for connectivity!
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("Plantower Found");
  
  // OLED Start Up
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if (!scd30.setMeasurementInterval(2)){
    Serial.println("Failed to set measurement interval");
    while(1) {delay(10);}
  }
  Serial.print("Measurement Interval: "); 
  Serial.print(scd30.getMeasurementInterval()); 
  Serial.println(" seconds");
  
  
  
  // Initial display message
  display.display();
  delay(500); // Pause for half second

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("Exposure");
  display.println("Logger v3");
  display.setCursor(0,50);
  display.setTextSize(1);
  display.println("I love you Chloe :)");
  display.display(); // you need to update the display after you've made changes
  delay(3000);

  
}


void loop() {
  if (scd30.dataReady()) {
    sensors_event_t temp;
    sensors_event_t humidity;
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);

    if (!scd30.read()){
      Serial.println("Error reading sensor data");
      display.println("READ ERR");
      display.display();
      return;
    }

    Serial.print("CO2: ");
    Serial.print(scd30.CO2, 3);
    Serial.println(" ppm");

    
    // Plantower
    PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");
    
    
    
    /// Sending Data to OLED
    //- OLED CO2 
    display.print("CO2: ");
    display.print(scd30.CO2, 2);
    display.println(" ppm");
    
    //- OLED Temp
    display.print(scd30.temperature,2);
    display.println(" C");

    //- OLED Humidity
    display.print("RH: ");
    display.print(scd30.relative_humidity,2);
    display.println("%");

    //- OLED PM2.5
    display.print("PM2.5: ");
    display.print(data.pm25_standard);
    display.println(" ug/m^3");
    display.print("0.3um: ");
    display.print(data.particles_03um);
    display.println(" #/100 cm3");
    display.print("0.5um: ");
    display.print(data.particles_05um);
    display.println(" #/100 cm3");
    display.print("1um: ");
    display.print(data.particles_10um);
    display.println(" #/100 cm3");
    display.print("2.5um: ");
    display.print(data.particles_25um);
    display.println(" #/100 cm3");
    display.display(); //don't forget to update display >:C
    // displpay.print(text,variable type)

  }

  delay(100);
}
