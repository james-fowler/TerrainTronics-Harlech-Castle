#ifndef _HARLECH_OUTPUT_CONTROL_HPP_
#define _HARLECH_OUTPUT_CONTROL_HPP_

#include "Harlech.hpp"
#include "HarlechPinConfig.hpp"
#include "KeepAlive.hpp"
#include "Patterns.hpp"


class HarlechOutputControl
 {
public: 
  
  HarlechOutputControl() : nextAdvance(0) {}

  void setup() {
    //set pins to output because they are addressed in the main loop
    pinMode(HPC_LATCH, OUTPUT);
    pinMode(HPC_CLOCK, OUTPUT);
    pinMode(HPC_DATA, OUTPUT);

    addPattern( "candle", candlePattern );
    addPattern( "blink", blinkPattern );
    addPattern( "pattern3", pattern3 );
    
  }
  const HarlechCastleDemoPattern &pattern() const { return *activePattern; }
  HarlechCastleDemoPattern &pattern() { return *activePattern; }

  template <size_t N>
  HarlechOutputControl &addPattern( const char *name, int (&pattern)[N][3] ) {
    HarlechCastleDemoPattern p( name, pattern );
    auto pShared = std::make_shared<HarlechCastleDemoPattern>(p);
    _patterns.push_back(pShared);
    patternMap[pShared->name] = pShared;
    if( !activePattern ) {
      activePattern = pShared;
    }
    return *this;
  }
  size_t patterns() const { return _patterns.size(); }
  
  void setPattern( size_t index ) {
    index = std::min( index, _patterns.size()-1 );
    activePattern = _patterns[index];
  }

  void loop(MilliTime);

protected:
  MilliTime nextAdvance;
  std::vector<PatternPtr> _patterns;
  std::map<std::string, PatternPtr> patternMap;
  PatternPtr activePattern;

};


#endif // _HARLECH_OUTPUT_CONTROL_HPP_
