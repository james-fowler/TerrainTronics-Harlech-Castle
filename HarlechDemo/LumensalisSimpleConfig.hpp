#ifndef _LUMENSALIS_SIMPLE_CONFIG_HPP_
#define _LUMENSALIS_SIMPLE_CONFIG_HPP_

#include <EEPROM.h>

#include "LumensalisSimpleLog.hpp"

namespace Lumensalis {
  
static std::string dquote() {
  return "\"";
}

static std::string jNameRawValue( const std::string &name, const std::string &value ) {
  std::string dq(dquote());
  return dq + name + dq + ":" + value;
  
}
static std::string jNameValue( const std::string &name, const std::string &value ) {
  std::string dq(dquote());
  return jNameRawValue( name, dq+value+dq );
}
static std::string jNameValue( const std::string &name, int value ) {
  char buf[40];
  sprintf(buf, "%d", value );
  return jNameRawValue( name, buf );
}
#define _ADD_COMMA_JV( tag )  rv += ","; rv += jNameValue(#tag, tag );

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

struct SimpleConfigHeader {
  enum CreatorId {
      CID_UNKNOWN,
      CID_Lumensalis = 777,
      CID_TerrainTronics = 7,
      CID_LastReserved = 9999,
      CID_FreeirstLastReserved 
      
  } ;

  enum ESPBoardId {
    EBID_UNKNOWN,
    EBID_ESP8266_A = 100,
      EBID_ESP8266 = EBID_ESP8266_A,
      EBID_ESP8266_WEMOS_D1_MINI,
    EBID_ESP8266_Z = EBID_ESP8266_WEMOS_D1_MINI,

    EBID_ESP32_A = 1000,
      EBID_ESP32 = EBID_ESP32_A,
      EBID_ESP32_WEMOS_D1_MINI,
    EBID_ESP32_Z = EBID_ESP8266_WEMOS_D1_MINI,

  } ;

  SimpleConfigHeader() :
      LSSA_SIGNATURE(7420099),LSSA_CONFIG_VERSION(100),creatorId(CID_UNKNOWN), vMajor(0),vMinor(0) 
      {
           memset(configName,sizeof(configName),0);
      }
  
  int LSSA_SIGNATURE;
  int LSSA_CONFIG_VERSION;
  CreatorId  creatorId;
  
  char configName[40];
  int vMajor;
  int vMinor;

  std::string jsonParts() const {
    std::string rv = jNameValue( "lcv", LSSA_CONFIG_VERSION );
     _ADD_COMMA_JV( creatorId );
     _ADD_COMMA_JV( configName );
     _ADD_COMMA_JV( vMajor );
     _ADD_COMMA_JV( vMinor );
    return rv;
  };       
  
  static constexpr int ConfigHeaderSize = 512;
};


struct ConfigItem {
  std::string typeName;
  std::string name;
  int defaultValue;
  int minValue;
  int maxValue;
  std::string description;
  size_t offset;

  std::string jsonValue(int currentValue) const {
    std::string rv  = "{ ";
    rv += jNameValue("currentValue", currentValue ); 
    

    _ADD_COMMA_JV( typeName );
    _ADD_COMMA_JV( name );
    _ADD_COMMA_JV( defaultValue );
    _ADD_COMMA_JV( minValue );
    _ADD_COMMA_JV( maxValue );
    _ADD_COMMA_JV( description );
    rv += "}";
    return rv;
  }
};

class ConfigWrapperBase {
 protected:
 SimpleConfigHeader &baseConfig;
 SimpleConfigHeader::CreatorId  creatorId;
 SimpleConfigHeader::ESPBoardId espBoardId;
 char configName[40];

  std::vector<ConfigItem> values;
  size_t offset = SimpleConfigHeader::ConfigHeaderSize;
public:  
  ConfigWrapperBase( SimpleConfigHeader &config, SimpleConfigHeader::CreatorId cId, const char * configName, SimpleConfigHeader::ESPBoardId ebId = SimpleConfigHeader::EBID_UNKNOWN ) : 
          baseConfig(config),creatorId(cId), espBoardId(ebId)
  {}

  void addItem_int( const std::string &name, int defaultValue, int minValue, int maxValue, const std::string & description ) 
  {
    ConfigItem item { "int", name, defaultValue, minValue, maxValue, description }; 
    item.offset = offset;
    offset += sizeof(int);
    values.push_back(item);
  }

  int getValue( const ConfigItem &item ) {
    const char *start = (const char *)&baseConfig;
    const int *vp = (int*)(start + item.offset);
    return *vp;
  }

  void dump() {
    spw.println();
    spw.println("{");
    spw.println(baseConfig.jsonParts().c_str() );
    
    spw.println("\"values\":[");
    for( auto value : values ) {
      spw.println( value.jsonValue( getValue(value) ).c_str()  );
    }
    spw.println("]");
    spw.println("}");
  }
};


// LSSA_CONFIG_VALUE( type, name, defaultValue, minValue, maxValue, description ) 


#define _LSSA_CV_DECLARE( type, name, defaultValue, minValue, maxValue, description ) type name;

#define _LSSA_CV_INIT( type, name, defaultValue, minValue, maxValue, description ) , name(defaultValue)


#define _LSSA_ADD_ITEM( type, name, defaultValue, minValue, maxValue, description ) \
        addItem_ ## type( #name,defaultValue, minValue, maxValue, description );

#define CREATE_SIMPLE_CONFIG( CREATOR_ID, CONFIG_CLASS_NAME, CONFIG_NAME,     \
                              VMAJOR, VMINOR, CONFIG_VALUE_DEFINITIONS )      \
struct CONFIG_CLASS_NAME : public Lumensalis::SimpleConfigHeader {            \
  char headerReserved[ConfigHeaderSize - sizeof(SimpleConfigHeader)];         \
                                                                              \
  CONFIG_VALUE_DEFINITIONS( _LSSA_CV_DECLARE )                                \
  CONFIG_CLASS_NAME() :   Lumensalis::SimpleConfigHeader()                    \
                CONFIG_VALUE_DEFINITIONS( _LSSA_CV_INIT )                     \
            {}                                                                \
};                                                                            \
                                                                              \
class CONFIG_CLASS_NAME ## Wrapper : public Lumensalis::ConfigWrapperBase {   \
  CONFIG_CLASS_NAME &config ;                                                 \
public:                                                                       \
  CONFIG_CLASS_NAME ## Wrapper( CONFIG_CLASS_NAME &config ) :                 \
   Lumensalis::ConfigWrapperBase( config,                                     \
              Lumensalis::SimpleConfigHeader::CREATOR_ID,                     \
                                      #CONFIG_NAME ), config(config )         \
  {                                                                           \
    CONFIG_VALUE_DEFINITIONS( _LSSA_ADD_ITEM )                                \
  }                                                                           \
};                                                                            \


} // namespace Lumensalis
#endif // _LUMENSALIS_SIMPLE_CONFIG_HPP_
