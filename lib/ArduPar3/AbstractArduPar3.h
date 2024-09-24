#pragma once
#include "Arduino.h"
#include "ArduPar3Helpers.h"
#include "NvsManager.h"

/// A common interface for all kinds of parameter settings. 
/// modelled after OSC messages: they can a number of have different values with different types. 
//  That can be useful i.e. for providing access to an array, or a mix of settings that all need to be updated at once
/// All the concrete implementations provided in this library give access only to a single value.
class AbstractArduPar3
{
public:
  virtual ARDUPAR_CONST_CHAR *getAddress()const = 0;                    ///< get an address that will be used to pass information to this Parameter. Doesn't have to be unique. For osc compatibility and ease of building interfaces, a valid OSC address format like "/machine/part/setting" is recommended
  virtual ARDUPAR_CONST_CHAR *getDescription() const= 0;                ///< a human readable description of the parameter
  virtual int getId()const = 0;                                         ///< a uinique identifier
  virtual void save() = 0;                                         ///< save content to NVM
  virtual void load() = 0;                                         ///< save content from NVM
  virtual void dumpParameterInfo(Stream *out) const = 0;                 ///< derived classed can give some information about semselves this way. preferably in a machine-readable way.
  virtual void parseCommand(char *data) = 0;                       ///< digest incoming string data that potentially contains a "set" command
  virtual size_t getValueAsText(char *buffer, size_t buflength)const  = 0; ///<  write value as a human readable text, with at most buflength characters
  virtual size_t getValueTextLength() const= 0;                         ///< get size of the buffer needed for getValueAsText()

  // we allow setting the parameter from a wide range fo data types, so interfaces can be built for a lot of different use cases
  // just implement them as you see fit, they might also just do nothing at all...
  virtual void trigger(){};
  virtual void setValueFromText(const char *data) = 0;                          ///< digest incoming string that only contains the value to be set (should be able to parse output of "getValueAsText")
  virtual void setValueFromFloat(float value, size_t position = 0) = 0;   ///< set Value using a floating point Number (and possibly a some single component of a vector)
  virtual void setValueFromInt32(int32_t value, size_t position = 0) = 0; ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)
  virtual void setValueFromBool(bool value, size_t position = 0) = 0;     ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)
  virtual void setValueFromDouble(double value, size_t position = 0) = 0; ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)

  // same here: these methods are primarily inteded to make interfaceing simple, implement them as you think they make sense
  virtual float getValueAsFloat(size_t position = 0) const = 0;
  virtual int32_t getValueAsInt32(size_t position = 0) const = 0;
  virtual bool getValueAsBool(size_t position = 0) const = 0;
  virtual double getValueAsDouble(size_t position = 0)const  = 0;
	/// each value has a type that can bve queried, this is primarily useful for visitor classes that build an interface
  enum ArduPar3Type
  {
    ArduPar3TypeNone,
    ArduPar3TypeOther,
    ArduPar3TypeTrigger,
    ArduPar3TypeFloat,
    ArduPar3TypeInt32,
    ArduPar3TypeBool,
    ArduPar3TypeString
  };
  virtual ArduPar3Type getType(size_t position = 0)const = 0; ///< to aid building of user interfaces at runtime
};
