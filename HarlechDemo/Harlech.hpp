#ifndef _HARLECH_HPP_
#define _HARLECH_HPP_

// #include <Ticker.h>


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

#include "LumensalisSimpleLog.hpp"
#include "LumensalisSimpleConfig.hpp"
#include "HarlechPinConfig.hpp"

#define HARLECH_DEMO_CONFIG_LAYOUT( LSSA_CONFIG_VALUE )                                           \
    LSSA_CONFIG_VALUE( int, keepAliveMsecOn, 1000, 1, 10000, "on time for keep-alive pin" )       \
    LSSA_CONFIG_VALUE( int, keepAliveMsecOff, 5000, 1000, 10000, "off time for keep-alive pin" )  \
    LSSA_CONFIG_VALUE( int, adcInPin,  HPC_ADC_IN, 0, 99, "ADC Input Pin" )                       \
    
CREATE_SIMPLE_CONFIG( CID_TerrainTronics, HarlechDemoConfig, HarlechDemo, 1, 0, HARLECH_DEMO_CONFIG_LAYOUT )

#endif //  _HARLECH_HPP_
