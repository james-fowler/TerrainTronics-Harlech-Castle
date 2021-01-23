//**************************************************************//
//  Name    : Harlech Castle PG1.0 Simple Patterns              //
//  Author  : Dafydd Roche                                      //
//  Date    : 1/22/2021                                         //
//  Version : 1.0                                               //
//  Notes   : This code runs on the Wemos D1 Mini.              //
//            This code does not use any of the Wifi            //
//              Capabilities                                    //
//****************************************************************

/* Simple Mods possible on Harlech Castle board.
 *  Connect Pin2 to pin3 of Pot R2 to make sure the potentiometer changes the constant current.
 *  PWM'ing the OE pin can do amplitude control
 *  That circuit is normally used to keep a USB Powerbank alive by pulling a load once in a while.
 * 
 */


// #define DEBUG 0 // this is used to switch debug comments on and off. Comment it out with // to optimize the code.

 
// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to.



//These definitions configure the pins for the SPI interface to talk to the LED driver
int latchPin = D8;
int clockPin = D5;
int dataPin = D6;
// Set the output enable pin. This can be rapidly PWM'd.
int OE = D4;
// The Keepalive pin for USB Power Banks is D3.
int keepAlive = D3;


int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int dutyCycle = 1023;          // set starting - PWM Duty Cycle. Data is inverted so 1023 is off.
int flashdelay = 500;
int breathDir = 0;  //Breathing Direction - 0 is counting up, 1 is counting down

/*
 * The following array defines patterns. 
 * The first column sets which LED's are on or off. 
 * The second column defines the brightness. 0 is full on, 1023 is full off.
 * The third column defines how many miliseconds they should be on for.
 * The array can have many many steps, with short timing between them. Worst case, you'll run out of memory on the chip (160KBytes worth!).
 */


// LED Test Pattern. 
int ledPattern[][3] = {   {0b11111111,  0,  500 } ,
                          {0b10101010,  0,  500 } ,
                          {0b10000000,  0,  50 } ,
                          {0b01000000,  0,  50 } ,
                          {0b00100000,  0,  50 } ,
                          {0b00010000,  0,  50 } ,
                          {0b00001000,  0,  50 } ,
                          {0b00000100,  0,  50 } ,
                          {0b00000010,  0,  50 } ,
                          {0b00000001,  0,  50 } ,
                          {0b01010101,  0,  500 } };

//Candle Flicker (on one of the LED's)
//int ledPattern[][3] = {   {0b00000101,  600,  100 } ,
//                          {0b00000101,  300,  150 } ,
//                          {0b00000101,  100,  200 } ,
//                          {0b00000101,  0,  100 } ,
//                          {0b00000101,  400,  100 } ,
//                          {0b00000101,  800,  50 } ,
//                          {0b00000101,  1000,  200 } ,
//                          {0b00000101,  1020,  50 } ,
//                          {0b00000101,  850,  150 } ,
//                          {0b00000101,  900,  100 } ,
//                          {0b00000101,  500,  150 } ,
//                          {0b00000101,  100,  200 } ,
//                          {0b00000101,  0,  100 } ,
//                          {0b00000101,  600,  100 } ,
//                          {0b00000101,  800,  50 } ,
//                          {0b00000101,  100,  200 } ,
//                          {0b00000101,  1020,  50 } ,
//                          {0b00000101,  850,  150 } ,
//                          {0b00000101,  900,  100 } ,
//                          {0b00000101,  400,  200 } };


// Set Up and initialize the pointers used for patterns                          
int patternIndex = 0;
int patternData = 0;
int patternDelay = 0;
int numrows = 0;

int numRows = sizeof(ledPattern)/sizeof(ledPattern[0]);


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
    //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  //pinMode(OE, OUTPUT);



#ifdef DEBUG
  Serial.print("numberofrows:");
  Serial.println(numRows);
#endif
}

void loop() {


// Defines the three 
  patternData = ledPattern[patternIndex][0];
  dutyCycle = ledPattern[patternIndex][1];
  patternDelay = ledPattern[patternIndex][2];


/*
 * This little bit allows you to print debuggin data back up the serial port to the PC. If DEBUG is not defined above, or not commented out, then it'll print.
 */
#ifdef DEBUG  
  Serial.println("Pattern");
  Serial.println(patternIndex);
  Serial.println(patternData);
  Serial.println(dutyCycle);
  Serial.println(patternDelay);
#endif
  
   // Serial.println("WRITE TO SHIFTER");
    digitalWrite(latchPin, LOW); // Tells the LED driver IC to listen
    shiftOut(dataPin, clockPin, LSBFIRST, patternData);
    digitalWrite(latchPin, HIGH); // Tells the LED driver 
   // Serial.println("DONE WRITING");
   analogWrite(OE, dutyCycle);
   delay(patternDelay);

  patternIndex++;
  if (patternIndex == numRows) { 
    patternIndex = 0;
  }


}
