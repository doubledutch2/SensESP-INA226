#ifndef _i2c_tools_H_
#define _i2c_tools_H_

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>


void scan_i2c() {

  Wire.begin();

  // Serial.begin(115200); //  LdB this is different to main.cpp
  Serial.println("I2C Scanner");

  // LdB Set up SCA and SCL lines
  //  int SDA = 4;	// Wemos D1 Mini Pro
  //  int SCL = 5;  // Wemos D1 Mini Pro
  // Wire.begin(SDA, SCL);	// 	Connect the scanner to the connect GPIO pins. 

  // LdB not sure what this does except for just scan 5 times?
  // for(uint8_t n = 0; n < 5; n++) {

    uint8_t error, address;
    int nDevices = 0;

    Serial.println("Scanning...");

    for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of the Write.endTransmisstion
    //  to see if a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address<16) 
          Serial.print("0");
        Serial.println(address,HEX);
        nDevices++;
      }
      else if (error==4) {
        Serial.print("Unknow error at address 0x");
        if (address<16) 
          Serial.print("0");
        Serial.println(address,HEX);
      }    
  }
  
    Serial.println("Finished scanning. ");
    if (nDevices == 0)
      Serial.println("No I2C devices found");

    delay(2000); // wait 2 seconds for next scan
    // LdB See above for loop - not sued I think }
}

#endif
