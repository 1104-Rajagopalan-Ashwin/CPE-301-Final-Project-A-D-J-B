/*
Names: Ashwin Rajagopalan, Dede Mendoza, Jordan Nattenberg, Bryan Escatel-Jimenez
CPE 301 Final Project
*/

#include <LiquidCrystal.h>
#include <Stepper.h>
#include <DHT.h>
#include <RTClib.h>

#define RDA 0x80
#define TBE 0x20

// Temperature setup
DHT dht(1, 1);
#define DHT11_PIN 3
#define TEMP_THRESHOLD 24

// Start/stop and reset button pins
#define START_STOP_PIN 19
#define RESET_PIN 18

// Water threshold
#define WATER_THRESHOLD 200

//RTC module
RTC_DS1307 myrtc;

// LCD setup
const int RS=53, EN=51, D4=49, D5=47, D6=45, D7=43;
 LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
//LiquidCrystal lcd(1, 2, 3, 4, 5, 6);

// Stepper motor setup
const int IN1=23, IN2=25, IN3=27, IN4=29;
const int stepsPerRevolution = 2038;
Stepper myStepper = Stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

//UART pointers:
volatile unsigned char *myUCSR0A  = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B  = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C  = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0   = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0    = (unsigned char *)0x00C6;

//pin pointers:

//A0 = button (22)
volatile unsigned char *portA     = (unsigned char *) 0x22;
volatile unsigned char *portDDRA  = (unsigned char *) 0x21;
volatile unsigned char *pinA      = (unsigned char *) 0x20;

//B4-7 = Status LEDs (10-13)
volatile unsigned char *portB     = (unsigned char *) 0x25;
volatile unsigned char *portDDRB  = (unsigned char *) 0x24;

// C3 = vent up C7 = vent down
volatile unsigned char *portC     = (unsigned char *) 0x28;
volatile unsigned char *portDDRC  = (unsigned char *) 0x27;
volatile unsigned char *pinC      = (unsigned char *) 0x26;

//D2 = start button (19)
volatile unsigned char *portD     = (unsigned char *) 0x2B;
volatile unsigned char *portDDRD  = (unsigned char *) 0x2A;
volatile unsigned char *pinD      = (unsigned char *) 0x29;

//E3 = LCD Register Select (5), E5 = Temperature/Humididty (3)
volatile unsigned char *portE     = (unsigned char *) 0x2E;
volatile unsigned char *portDDRE  = (unsigned char *) 0x2D;
volatile unsigned char *pinE      = (unsigned char *) 0x2C;

//F0 = Water Sensor (A0)
volatile unsigned char *portF     = (unsigned char *) 0x31;
volatile unsigned char *portDDRF  = (unsigned char *) 0x30;
volatile unsigned char *pinF      = (unsigned char *) 0x2F;

//G5 = LCD Enable (4)
volatile unsigned char *portG     = (unsigned char *) 0x34;
volatile unsigned char *portDDRG  = (unsigned char *) 0x33;
volatile unsigned char *pinG      = (unsigned char *) 0x32;

//H3-6 = LCD Data Pins (6-9)
volatile unsigned char *portH     = (unsigned char *) 0x102;
volatile unsigned char *portDDRH  = (unsigned char *) 0x101;
volatile unsigned char *pinH      = (unsigned char *) 0x100;

//ADC pointers
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

//timer pointers
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;

/* States:
0: Disabled
1: Idle
2: Running
3: Error */
int state, nextState;
char stateNames[4][9] = {"Disabled", "Idle", "Running", "Error"};
int waterLevel;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
