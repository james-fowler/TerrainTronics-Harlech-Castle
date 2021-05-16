
#ifndef _KEEPALIVE_HPP_
#define _KEEPALIVE_HPP_

#include "Harlech.hpp"

// This sets up the USBBank Keep Alive Functionality. By default, this is set to 100mA of additional current, for 1S every 10.


class KeepAlivePinger {
public:

  int USBBankState;
  MilliTime nextChangeTime;
  KeepAlivePinger() : USBBankState(0), nextChangeTime(0), kaTimeOn(minTimeOn), kaTimeOff(minTimeOff) {}

  void scheduleNext( int stateTime = 1 ) {
    nextChangeTime = millis() + stateTime;
  }

  MilliTime getTimeOn() const {
    return kaTimeOn;
  }

  MilliTime getTimeOff() const {
    return kaTimeOff;
  }
  
  void setTimeOn( MilliTime duration ) {
    kaTimeOn = std::max( minTimeOn, std::min(maxTimeOn, duration) );
  }
  void setTimeOff( MilliTime duration ) {
    kaTimeOff = std::max( minTimeOff, std::min(maxTimeOff, duration) );
  }

  void setup() {
    
    pinMode(HPC_KEEP_ALIVE, OUTPUT);     // KeepAlive pin needs pulling low, otherwise it'll float up
    digitalWrite(HPC_KEEP_ALIVE, LOW);   // and switch on the transistor. - THIS IS IMPORTANT, OTHERWISE IT FLOATS UP AND SWITCHES THE TRANSISTOR ON!
    #ifdef HPC_KEEP_ALIVE2
    pinMode(HPC_KEEP_ALIVE2, OUTPUT);     // KeepAlive pin needs pulling low, otherwise it'll float up
    digitalWrite(HPC_KEEP_ALIVE2, LOW);   // and switch on the transistor. - THIS IS IMPORTANT, OTHERWISE IT FLOATS UP AND SWITCHES THE TRANSISTOR ON!
    #endif

    scheduleNext();
  }
  void loop( int now ) {
    if( now < nextChangeTime )  {
      return;
    }

    keepAliveChangeState();
  }
      
  void keepAliveChangeState()
  {
    //=======================================================================
    // This is a subroutine that switches the keepAlive pin on/off to switch on a transistor that pulls current to ground, through a 47Ohm resistor
    // PLEASE NOTE: You cannot leave this on for a long long time, as the resistor will begin to heat up beyond it's rating!
    int pinState = USBBankState ? HIGH : LOW;
    int stateTime = USBBankState ? kaTimeOff : kaTimeOn;
    USBBankState = !USBBankState;

    LSSA_LOG( "keep alive :", pinState, stateTime );
    digitalWrite(HPC_KEEP_ALIVE, pinState);
    #ifdef HPC_KEEP_ALIVE2
    digitalWrite(HPC_KEEP_ALIVE2, pinState);
    #endif
    scheduleNext(stateTime);
  }

private:
  const MilliTime minTimeOn = 100;
  const MilliTime maxTimeOn = 1000;
  const MilliTime minTimeOff = 5000;
  const MilliTime maxTimeOff = 60000;
  MilliTime kaTimeOn = 1000; // Time to switch the keep alive circuit on.
  MilliTime kaTimeOff = 8000; // Time to switch the keep alive circuit off for.

};

#endif //  _KEEPALIVE_HPP_
