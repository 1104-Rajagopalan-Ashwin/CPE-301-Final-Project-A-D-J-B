//LCD library
#include <LiquidCrystal.h>
//Stepper Motor Library
#include <Stepper.h>
//temp-humidity sensor library
#include <DHT.h>
//Real Time Clock Library
#include <RTClib.h>
#include <Adafruit_Sensor.h>

#define RDA 0x80
#define TBE 0x20
// UART Pointers
volatile unsigned char *myUCSR0A  = (unsigned char *) 0xC0;
volatile unsigned char *myUCSR0B  = (unsigned char *) 0xC1;
volatile unsigned char *myUCSR0C  = (unsigned char *) 0xC2;
volatile unsigned int  *myUBRR0   = (unsigned int *) 0xC4;
volatile unsigned char *myUDR0    = (unsigned char *) 0xC6;




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
