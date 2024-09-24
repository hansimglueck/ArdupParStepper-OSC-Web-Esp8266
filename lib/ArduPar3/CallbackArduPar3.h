#pragma once
#include "Arduino.h"
#include "ArduPar3Helpers.h"
#include "ArduPar3Collection.h"
#include "NvsManager.h"
#include "AbstractArduPar3.h"

// The AVR implementation doesn't have STL support, so we use pointers instead of std::function

#define CallbackParameter void (*callbackFunction)(void) 

////////////////////
///this one will just trigger a function whenever any set operation is performed on it (useful for buttons, etc)
class CallbackArduPar3 : public AbstractArduPar3
{
public:


  /// set up the setting. has to be called to make it functional
  void setup(
       ARDUPAR_CONST_CHAR *address,
       ARDUPAR_CONST_CHAR *description,
      CallbackParameter,        ///< A function that will be triggered.
    ArduPar3Collection* collection=ArduPar3Collection::globalDefaultCollection /// will register here and get a unique id if provided 
  ); 
  void trigger(){callbackFunction();};    ///< invoke the callback

  void save(){}; ///< save content to NVM
  void load(){}; ///< save content from NVM

  size_t getValueAsText(char* buffer, size_t buflength)const{return 0;}; ///<  write value as a human readable text, with at most buflength characters
   size_t  getValueTextLength()const{return 0;}; ///< get size of the buffer needed for getValueAsText()

  // we allow setting the parameter from a wide range fo data types, so interfaces can be built for a lot of different use cases
  // just implement them as you see fit, they might also just do nothing at all...
  void setValueFromText(const char *data){trigger();};
   void setValueFromFloat(float value, size_t position=0){trigger();}; ///< set Value using a floating point Number (and possibly a some single component of a vector)
   void setValueFromInt32(int32_t value, size_t position=0){trigger();}; ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)
   void setValueFromBool(bool value, size_t position=0){trigger();}; ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)
   void setValueFromDouble(double value, size_t position=0){trigger();}; ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)

  // same here: these methods are primarily inteded to make interfaceing simple, imeplement them as you think they make sense
   float getValueAsFloat(size_t position=0)const{return 0;};
   int32_t getValueAsInt32(size_t position=0)const{return 0;};
   bool getValueAsBool(size_t position=0)const{return 0;};
   double getValueAsDouble(size_t position=0)const{return 0;};


  void dumpParameterInfo(Stream *out) const;   ///< derived classed can give some information about semselves this way. preferably in a machine-readable way.
  void parseCommand(char *data);          ///< digest incoming serial data that potentially contains a "set" command
  int getId() const{ return uniqueId; }; ///< a uinique identifier
  ARDUPAR_CONST_CHAR * getAddress()const{return addressString;}; ///< get an address that will be used to pass information to this Parameter. Doesn't have to be unique. For osc compatibility and ease of building interfaces, a format like "/machine/part/setting" is recommended
  ARDUPAR_CONST_CHAR * getDescription()const{return descriptionString;}; ///< a human readable description of the parameter
   ArduPar3Type getType(size_t position=0)const{return position==0? AbstractArduPar3::ArduPar3TypeTrigger:AbstractArduPar3::ArduPar3TypeNone;}; ///< to aid building of user interfaces at runtime
protected:
  size_t uniqueId;
   ARDUPAR_CONST_CHAR *addressString;     ///< serial input is parsed for this command string, anything that follows is
   ARDUPAR_CONST_CHAR *descriptionString; ///< serial input is parsed for this command string, anything that follows is
  size_t addressLength; ///< to facilitate comparisons
  CallbackParameter; ///< points to the function to be called
};
