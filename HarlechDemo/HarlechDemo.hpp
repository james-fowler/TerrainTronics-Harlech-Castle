#ifndef _HARLECH_DEMO_HPP_
#define _HARLECH_DEMO_HPP_

#include "Harlech.hpp"
#include "HarlechPinConfig.hpp"
#include "KeepAlive.hpp"
#include "Patterns.hpp"

struct PatternStep {
  int ledPattern;
  int brightness;
  int duration;
};
class HarlechCastleDemoPattern {
public:
  std::vector<PatternStep> steps;

  // Set Up and initialize the pointers used for patterns                          
  size_t currentIndex = 0;

  template <size_t N>
  HarlechCastleDemoPattern( int (&pattern)[N][3] ) {
    for( size_t x = 0; x < N; x++ ) {
      steps.push_back( PatternStep { pattern[x][0], pattern[x][1], pattern[x][2] } );
    }
  }

  const PatternStep &currentStep() const { return steps[currentIndex]; }
  void advance() { 
    if( ++currentIndex >= steps.size() ) {
      currentIndex  = 0; }
    }
  
};



class HarlechCastleDemo {
public: 
  int sensorValue = 0;        // value read from the pot
  int outputValue = 0;        // value output to the PWM (analog out)
  int dutyCycle = 1023;          // set starting - PWM Duty Cycle. Data is inverted so 1023 is off.
  int flashdelay = 500;
  int breathDir = 0;  //Breathing Direction - 0 is counting up, 1 is counting down

  
  
  HarlechCastleDemo() :
      sensorValue(0), outputValue(0), dutyCycle(1023), flashdelay(500), breathDir(0) ,
      blink(blinkPattern), candle(candlePattern), activePattern(&blink)
      {}

  const HarlechCastleDemoPattern &pattern() const { return *activePattern; }
  HarlechCastleDemoPattern &pattern() { return *activePattern; }


  HarlechCastleDemoPattern blink;
  HarlechCastleDemoPattern candle;

protected:
  HarlechCastleDemoPattern *activePattern;

};


#endif // _HARLECH_DEMO_HPP_
