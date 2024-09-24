#pragma once
#include "ArduPar3Helpers.h"
#ifdef __XTENSA__
#include <Preferences.h>
#endif
#include "Arduino.h"
/// encapsulates a device independent storage location for non volatile storage
class NvsAddress{
	public:
#if defined(__AVR__)
	size_t addressOffset=0; ///< eeprom start byte
#endif

#ifdef __XTENSA__
ARDUPAR_CONST_CHAR * key;
char crcOfKeyAsHex[9];
#endif

};
/// an abstraction layer for device independent for non-volatile storage
class NvsManager{
	public:
	static NvsAddress getAddressFor(ARDUPAR_CONST_CHAR * key,size_t bytes);	///< returns an adress and reserves the requested number of bytes. the "key" is currently only used on ESP32 platforms, and ignored everywhere else
	static bool read_bytes(void* dst, NvsAddress address, int nBytes); ///< read raw data from permanent storage
	static bool write_bytes(void* dst, NvsAddress address, int nBytes); ///< write raw data to permanent storage
	
	protected:
	#if defined(__AVR__)
	static int nextFreeAdress;	///< points to the next free byte
#endif
#ifdef __XTENSA__
	static bool isInitialized;
	static Preferences preferences;
	#endif
};


