#ifndef _HARLECH_HPP_
#define _HARLECH_HPP_

// #include <Ticker.h>
#include <EEPROM.h>

typedef int MilliTime;

#if defined(ARDUINO_ARCH_ESP32)
#include "analogWrite.h"
//#include "ESP32PWM.h"

#else

#include <ESP8266WiFi.h> // This allows the modem to be powered off, saving 40mA!

#endif

#include <map>
#include <vector>
#include <memory>
#include <functional>


class SerialPrintWrap {
public:

    bool enabled;
    
   SerialPrintWrap() : enabled(true) {}
   void println() {
      Serial.println();
   }
   template < typename Type, typename... Args>
   void println( Type arg1, Args...args ) {
        Serial.print(arg1);
        this->println( args... );
     };
     


    void printsln() {
      Serial.println();
   }
   template < typename Type, typename... Args>
   void printsln( Type arg1, Args...args ) {
        Serial.print(arg1); Serial.print(' ');
        println( args... );
     };

   template < typename... Args>
   void dprintln( Args...args ) {
       if(!enabled) return;
       this->println(args...);
   };
   template < typename... Args>
   void dsprintln( Args...args ) {
       if(!enabled) return;
       this->printsln(args...);
   };
};

extern SerialPrintWrap spw;

#ifdef HC_ENABLE_DEBUG_LOG

#define HC_LOG( ... ) spw.dsprintln( __VA_ARGS__ )
#define HC_LOG1( A ) spw.dprintln( A )
#define HC_LOG2( B, A ) spw.dsprintln( B, A )
#define HC_LOG3( C, B, A ) spw.dsprintln( C, B, A )
#define HC_LOG4( D, C, B, A ) spw.dsprintln( D, C, B, A )
#define HC_LOG5( E, D, C, B, A ) spw.dsprintln( E, D, C, B, A )


/*
#define HC_LOG1( A ) Serial.println( A )
#define HC_LOG2( B, A ) Serial.print( B ); Serial.print( ' ' ); HC_LOG1( A )
#define HC_LOG3( C, B, A ) Serial.print( C ); Serial.print( ' ' ); HC_LOG2( B, A )
#define HC_LOG4( D, C, B, A ) Serial.print( D ); Serial.print( ' ' ); HC_LOG3( C, B, A )
#define HC_LOG5( E, D, C, B, A ) Serial.print( E ); Serial.print( ' ' ); HC_LOG4( D, C, B, A )
*/

#else

#define HC_LOG( ... ) 
#define HC_LOG1( A )
#define HC_LOG2( B, A )
#define HC_LOG3( C, B, A )
#define HC_LOG4( D, C, B, A )
#define HC_LOG5( E, D, C, B, A )

#endif

class ESP_NVM_Access {
public:  
  static const size_t NVMBlockSize = 512;
  void setup() {
    EEPROM.begin(NVMBlockSize); 
  }

  void storeByteToEeprom(int start, uint8_t val) { //Address in EEPROM, value
    EEPROM.write(start, val);
    EEPROM.commit(); //Needed to execute the write
  }
  
  uint8_t readByteFromEeprom(int start) {
    return EEPROM.read(start);
  }

};


#endif //  _HARLECH_HPP_
