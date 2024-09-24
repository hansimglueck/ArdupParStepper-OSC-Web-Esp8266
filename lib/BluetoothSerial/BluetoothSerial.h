// replacement for the original BluetoothSerial.h not present in Esp8266 and needed by the OSC library
// remove if your platform comes with a "real" BluetoothSerial.h
#pragma once    
#include <Arduino.h>

class BluetoothSerial: public Stream
{
public: 
void begin(){};
void begin(String){}
};
