#ifndef _LUMENSALIS_SIMPLE_LOG_HPP_
#define _LUMENSALIS_SIMPLE_LOG_HPP_

/* Copyright 2021, James Fowler */

namespace Lumensalis {
  
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

#ifdef LSSA_ENABLE_DEBUG_LOG

#define LSSA_LOG( ... ) Luemnsalis::spw.dsprintln( __VA_ARGS__ )

#else

#define LSSA_LOG( ... ) 

#endif


} // namespace Lumensalis

#endif // _LUMENSALIS_SIMPLE_LOG_HPP_
