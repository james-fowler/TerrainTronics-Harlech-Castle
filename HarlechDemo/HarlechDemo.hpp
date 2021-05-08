#ifndef _HARLECH_DEMO_HPP_
#define _HARLECH_DEMO_HPP_

#include "Harlech.hpp"
#include "HarlechPinConfig.hpp"
#include "KeepAlive.hpp"
#include "Patterns.hpp"
#include "HarlechOutputControl.hpp"

class HarlechCastleDemo {
public: 
  HarlechOutputControl &harlech;
  ESP_NVM_Access nvm;
  
  HarlechCastleDemo(HarlechOutputControl &harlech ) : harlech(harlech) {}

  void setup() {
    nvm.setup();
    showHelp();
  }
  void checkAnalogIn() {
  
    static int maxAnalogIn = 1;
    static int priorAnalogIn = 1;
    static int priorPattern = -1;
    int analogIn = analogRead( HPC_ADC_IN );
    
    if( analogIn != priorAnalogIn ) {
      priorAnalogIn  = analogIn;
      // HC_LOG2( "analogIn ", analogIn );
      if(analogIn > maxAnalogIn) {
        maxAnalogIn = analogIn;
      }
      
      size_t pattern = (analogIn * (harlech.patterns()+1)) / maxAnalogIn;
      
      if( pattern != priorPattern ) {
        priorPattern = pattern;
        HC_LOG2( "Changed Pattern ", pattern );
        harlech.setPattern(pattern);
      }
    }
  }

  void showHelp() {
    static const char *helpLines[] =  {
        "HELP",
        " h/? : show this help",
        "  d  : toggle debug print",
        
    };

    for( auto line : helpLines ) {
      Serial.println( line );
    }
  }
  
  void handleKey( int c ) {
    switch(c) {
      case 'h':
      case '?':
        showHelp();
        break;
      case 'd':
        spw.enabled = !spw.enabled;
        spw.printsln( "Debug Logging ", spw.enabled ? "enabled" : "disabled" );
        break;
      case 'f':
      case 'g':
      case 'i':
      case 'j':
        nvm.storeByteToEeprom(42,c);
        spw.printsln( "stored", (int)c, " in [42]" );
        break;
      case 'r': {
        int v = nvm.readByteFromEeprom(42);
        spw.printsln( "read ", (int)v, " from [42]" );
        break;
      }

      default:
        Serial.print( (char)c );
    }
  }

  void loop(MilliTime now);

protected:

};



#endif // _HARLECH_DEMO_HPP_
