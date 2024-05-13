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
void displayTempHumidity(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(DHT.readHumidity);
  lcd.print("%");
}

void adc_init(){
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num){
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

void my_delay(unsigned int freq)
{
  // calc period
  double period = 1.0/double(freq);
  // 50% duty cycle
  double half_period = period/ 2.0f;
  // clock period def
  double clk_period = 0.0000000625;
  // calc ticks
  unsigned int ticks = half_period / clk_period;
  // stop the timer
  *myTCCR1B &= 0xF8;
  // set the counts
  *myTCNT1 = (unsigned int) (65536 - ticks);
  // start the timer
  * myTCCR1A = 0x0;
  * myTCCR1B |= 0b00000001;
  // wait for overflow
  while((*myTIFR1 & 0x01)==0); // 0b 0000 0000
  // stop the timer
  *myTCCR1B &= 0xF8;   // 0b 0000 0000
  // reset TOV           
  *myTIFR1 |= 0x01;
}
