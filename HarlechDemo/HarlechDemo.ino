//**************************************************************//
//  Name    : Harlech Castle PG1.0 Simple Patterns              //
//  Author  : Dafydd Roche                                      //
//  Date    : 2/8/2021                                         //
//  Version : 1.1                                               //
//  Notes   : This code runs on the Wemos D1 Mini.              //
//            This code does not use any of the Wifi            //
//              Capabilities                                    //
//****************************************************************

/* Simple Mods possible on Harlech Castle board.
 *  Connect Pin2 to pin3 of Pot R2 to make sure the potentiometer changes the constant current.
 *  PWM'ing the OE pin can do amplitude control
 *  That circuit is normally used to keep a USB Powerbank alive by pulling a load once in a while.
 *
 * 
 * Things you need to do when you get your hands on this code.
 * - Configure the Keepalive settings. See the lines "const int kaTimeOn" and "kaTimeOff".
 * This circuits pulls a burst of power for 1 second every 8 seconds by default. This needs to be tuned 
 * for the USB battery pack you are using. Some might only need it for 0.5 seconds every 30 seconds 
 * to make sure it stays on. Others, need it WAY more often. 1 second on every 8 seconds was selected with
 * the crappy battery pack I was using
 * 
 * Select which pattern you want. There are three patterns in this code, the line of code starts with 
 * int ledPattern[][3]. The default one is all on, all off for 3 seconds each. 2 states and it just goes 'round and round.
 * To select the one you want, comment out the test one (by using // in front of the lines) and uncomment the pattern you want.
 * (Simply delete the // in front of the lines you want)
 * 
 * Any questions, come on over to github or post on the facebook group!
 * 
 * Cheers
 * 
 * TerrainTronics
 * 
 * 
 */

/* chosing pin configurations 
 *    D1 chip  
 *      ESP8266 
 *      ESP32
 *    Harlech Castle board version
 *        PG10 
 *        PG11 
 *        DUAL (support both, if you don't need to use the extra pins)
 *        
 * choices are  
 *        HPC_ESP8266_DUAL / HPC_ESP8266_PG10 / HPC_ESP8266_PG11 
 *        HPC_ESP326_DUAL / HPC_ESP32_PG10 / HPC_ESP32_PG11 
 * 
 */


#define HARLECH_PIN_CONFIG HPC_ESP8266_PG10

// this is used to switch debug comments on and off. Comment it out with // to optimize the code.
#define HC_ENABLE_DEBUG_LOG 1 

#include "HarlechDemo.hpp"

SerialPrintWrap spw;


KeepAlivePinger keepAlive;
HarlechOutputControl harlech;
HarlechCastleDemo demo(harlech);

void setup() {
  
#if !defined(ARDUINO_ARCH_ESP32)
 // WiFi.disconnect();     // We're not using Wifi, so lets disable it to save power.
 // WiFi.forceSleepBegin();
 // delay(1); //For some reason the modem won't go to sleep unless you do a delay
#endif
  
  // initialize serial communications at 9600 bps: this is only used for communication with a PC.
  Serial.begin(115200);

  keepAlive.setup();
  harlech.setup();
  demo.setup();
}

void HarlechOutputControl::loop( MilliTime now ) {
  if( now < nextAdvance ) return;
  
  auto const &patternStep = pattern().currentStep();
  auto dutyCycle = patternStep.brightness;
  
  HC_LOG4( "Pattern", patternStep.ledPattern, dutyCycle, patternStep.duration );
  
  HC_LOG1( "WRITE TO SHIFTER" );
  digitalWrite(HPC_LATCH, LOW); // Tells the LED driver IC to listen
  shiftOut(HPC_DATA, HPC_CLOCK, LSBFIRST, patternStep.ledPattern);
  digitalWrite(HPC_LATCH, HIGH); // Tells the LED driver 
  HC_LOG1(  "DONE WRITING");
  
  analogWrite(HPC_OE, dutyCycle);
  #ifdef HPC_OE2
  analogWrite(HPC_OE2, dutyCycle);
  #endif
  
  HC_LOG2( "DELAY", patternStep.duration );
  nextAdvance = now + patternStep.duration;

  pattern().advance();
}

void HarlechCastleDemo::loop( MilliTime now ) {
 //'while( Serial.available() ) {
    char buf[2];
    auto lengthRead = Serial.readBytes(buf, 1 );
    if(lengthRead) {
      buf[1] = 0;
      HC_LOG( "Read key input", buf );
      handleKey(buf[0]);
    }  
    //else {
    //  break;
   // }
  }    
//}

void loop() {
  MilliTime now = millis();
  harlech.loop(now);
  demo.loop(now);
  keepAlive.loop(now);
  

//  delay(10);
}
