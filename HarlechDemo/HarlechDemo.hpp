#ifndef _HARLECH_DEMO_HPP_
#define _HARLECH_DEMO_HPP_

#include "Harlech.hpp"
#include "HarlechPinConfig.hpp"
#include "KeepAlive.hpp"
#include "Patterns.hpp"
#include "HarlechOutputControl.hpp"

using Lumensalis::spw;

class HarlechCastleDemo {
public: 
  HarlechOutputControl &harlech;
  //ESP_NVM_Access nvm;
  HarlechDemoConfig demoConfig;
  
  HarlechCastleDemo(HarlechOutputControl &harlech ) : harlech(harlech) {}

  void setup() {
    //nvm.setup();
    HarlechDemoConfigWrapper cfg( demoConfig );

    cfg.dump();
    
    showHelp();
  }
  void checkAnalogIn() {
  
    static int maxAnalogIn = 1;
    static int priorAnalogIn = 1;
    static int priorPattern = -1;
    int analogIn = analogRead( demoConfig.adcInPin );
    
    if( analogIn != priorAnalogIn ) {
      priorAnalogIn  = analogIn;
      // LSSA_LOG( "analogIn ", analogIn );
      if(analogIn > maxAnalogIn) {
        maxAnalogIn = analogIn;
      }
      
      size_t pattern = (analogIn * (harlech.patterns()+1)) / maxAnalogIn;
      
      if( pattern != priorPattern ) {
        priorPattern = pattern;
        LSSA_LOG( "Changed Pattern ", pattern );
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
        // nvm.storeByteToEeprom(42,c);
        spw.printsln( "stored", (int)c, " in [42]" );
        break;
      case 'r': {
        /// int v = nvm.readByteFromEeprom(42);
        //spw.printsln( "read ", (int)v, " from [42]" );
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
