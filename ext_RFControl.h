/*
  RFControl.h - 
  https://github.com/pimatic/RFControl
  GPL v3
*/
#ifndef ArduinoRf_h
#define ArduinoRf_h

class RFControl
{
  public:
    static unsigned int getPulseLengthDivider();
    static void startReceiving(int interruptPin);
    static void stopReceiving();
    static bool hasData();
    static void getRaw(unsigned int **timings, unsigned int* timings_size);
    static void continueReceiving();
    static bool compressTimings(unsigned int buckets[8], unsigned int *timings, unsigned int timings_size);
    static bool compressTimingsAndSortBuckets(unsigned int buckets[8], unsigned int *timings, unsigned int timings_size);
    static void sendByTimings(int transmitterPin, unsigned int *timings, unsigned int timings_size, unsigned int repeats = 3);
    static void sendByCompressedTimings(int transmitterPin, unsigned long* buckets, char* compressTimings, unsigned int repeats = 3); 
    static unsigned int getLastDuration();
    static bool existNewDuration();
  private:
    RFControl();
};

// content of arduino_functions.h

#include "Arduino.h"

#if (defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)) && !defined(MAX_RECORDINGS)
  #define MAX_RECORDINGS 400   //In combination with Homeduino maximum 490*2Byte are possible. Higher values block the arduino
#endif
#if (defined(__AVR_ATmega32U4__) || defined(TEENSY20) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) || defined(ESP8266)) && !defined(MAX_RECORDINGS)
  #define MAX_RECORDINGS 512   //on the bigger arduino we have enough SRAM
#endif
#if defined(ESP8266)
  #define MAX_RECORDINGS 512  // ESP8266 has far enough ram
#endif
#if !defined(MAX_RECORDINGS)
  #define MAX_RECORDINGS 255   // fallback for undefined Processor.
#endif

static inline void hw_attachInterrupt(int pin, void (*callback)(void)) {
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), callback, CHANGE);
}

static inline void hw_detachInterrupt(int pin) {
  detachInterrupt(digitalPinToInterrupt(pin));
}

static inline void hw_delayMicroseconds(uint32_t time_to_wait) {
  //  delayMicroseconds() only works up to 16383 micros
  // https://github.com/pimatic/rfcontroljs/issues/29#issuecomment-85460916
  while(time_to_wait > 16000) {
    #if defined(ESP8266)
      delay(16);
    #else
      delayMicroseconds(16000);
    #endif
    time_to_wait -= 16000;
  }
  delayMicroseconds(time_to_wait);
}

static inline void hw_pinMode(int pin, int mode) {
  pinMode(pin, mode);
}

static inline void hw_digitalWrite(int pin, int value) {
  digitalWrite(pin, value);
}

static inline uint32_t hw_micros() {
  return micros();
}

#endif
