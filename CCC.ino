/*
   weight sensor
   adxl345
   temp
   BLE

*/
#include <OneWire.h> 
#include <Wire.h>
#include <ADXL345.h>
#include "HX711.h"
#include <SPI.h>
#include <EEPROM.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#define DOUT  5
#define CLK  8
HX711 scale(DOUT, CLK);
int DS18S20_Pin = 9; //DS18S20 Signal pin on digital 9
OneWire ds(DS18S20_Pin);  // on digital pin 9

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup

int acc = 2;

void setup() {
  ble_begin();
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(25); //62.5mg per increment
  adxl.setInactivityThreshold(25); //62.5mg per increment
  adxl.setTimeInactivity(5); // how many seconds of no activity is inactive?

  //look of activity movement on this axes - 1 == on; 0 == off
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);

  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);

  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);

  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625μs per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment

  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment

  //setting all interupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,  ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,  ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,  ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,  ADXL345_INT1_PIN );

  //register interupt actions - 1 == on; 0 == off
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

  Serial.begin(9600);
  // scale

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

unsigned char buf[16] = {0};
unsigned char len = 0;

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor


  int x, y, z;
  adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables x,y,z

  // temp code


float temperature = getTemp(); //will take about 750ms to run

  //int temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset

  // ADXL

  byte interrupts = adxl.getInterruptSource();

  //inactivity
  if (adxl.triggered(interrupts, ADXL345_INACTIVITY)) {
       Serial.println("inactivity");
    acc = 2;
  }

  //activity
  if (adxl.triggered(interrupts, ADXL345_ACTIVITY)) {
     Serial.println("activity");
    acc = 1;
  }

//  Serial.print("Reading: ");
//  Serial.print(scale.get_units(), 1);
//  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
//  Serial.print(" calibration_factor: ");
//  Serial.print(calibration_factor);
//  Serial.println();

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }

  float weight = scale.get_units();
  //  scale.power_down();             // put the ADC in sleep mode
  //  delay(5000);
  //  scale.power_up();

  //
  //  temperatureC = 10;
  //  weight = 52;
  //  acc = 13;

  // Serial.print(acc);
//    Serial.print(",");
//    Serial.println(temperature);
   // Serial.print(",");
//  Serial.println(weight);

  char str0[10];
  itoa(acc, str0, 10);
  int i0 = 0;
  while (str0[i0] != '\0') {
    ble_write(str0[i0++]);
  }

  ble_write(',');

  char str[10];
  itoa(temperature, str, 10);
  int i = 0;
  while (str[i] != '\0') {
    ble_write(str[i++]);

  }

//  Serial.println(temperature);
  ble_write(',');

  char str2[10];
  itoa(weight, str2, 10);
  int i2 = 0;
  while (str2[i2] != '\0') {
    ble_write(str2[i2++]);

//     Serial.println(str2[i2++]);
//
//     Serial.println(weight);
  }


  ble_do_events();
  /////////////////////////// Using the data /////////////////////////////

}

////

float getVoltage(int pin)
{
  return (analogRead(pin) * 0.004882814);
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  
  delay(750); // Wait for temperature conversion to complete

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
