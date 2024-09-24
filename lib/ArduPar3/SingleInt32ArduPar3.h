#pragma once
#include "Arduino.h"
#include "ArduPar3Helpers.h"
#include "ArduPar3Collection.h"
#include "NvsManager.h"
#include "AbstractArduPar3.h"

/// A common interface for all kinds of parameter settings. modelled after OSC messages: they can have different
class SingleInt32ArduPar3 : public AbstractArduPar3
{
public:
  SingleInt32ArduPar3() : value(0),
                          valuePointer(&this->value)
  {
  }
  /// set up the setting. has to be called to make it functional
  void setup(
      ARDUPAR_CONST_CHAR *address,
      ARDUPAR_CONST_CHAR *description,
      int32_t minValue,
      int32_t maxValue,
		int32_t startValue =0,																			///< value will be initialized with this
      int32_t *valuePointer = 0,                                                    ///< the setting can modify an arbitrary location im memory if you give it here.
      boolean isPersistent = true,                                                  ///< should it be possible to save this setting to NVS?
      ArduPar3Collection *collection = ArduPar3Collection::globalDefaultCollection, /// will register here and get a unique id if provided
      bool useAutomaticNvsAddress = true,                                           ///< if you want a specific fixed NVS storage location, specify it here, otherwise it will be assigned automatically
      NvsAddress manualNvsAddress = NvsAddress()                                    ///< if you want a specific fixed NVS storage location, specify it here

  );

  ARDUPAR_CONST_CHAR *getAddress() const{ return addressString; };         ///< get an address that will be used to pass information to this Parameter. Doesn't have to be unique. For osc compatibility and ease of building interfaces, a format like "/machine/part/setting" is recommended
  ARDUPAR_CONST_CHAR *getDescription() const{ return descriptionString; }; ///< a human readable description of the parameter

  void setValue(int32_t newValue); ///< set the value and rpint some debug info
  int32_t getValue() const{ return *valuePointer; }
  bool getAndClearValueChangedFlag(); ///< returns if the value has changed since the last call of this function

  int getId() const{ return uniqueId; }; ///< a uinique identifier

  void save();                                           ///< save content to NVM
  void load();                                           ///< save content from NVM
  void dumpParameterInfo(Stream *out)const;                   ///< derived classed can give some information about semselves this way. preferably in a machine-readable way.
  void parseCommand(char *data);                         ///< digest incoming serial data that potentially contains a "set" command
  size_t getValueAsText(char *buffer, size_t buflength)const; ///<  write value as a human readable text, with at most buflength characters
  size_t getValueTextLength()const;                           ///< get size of the buffer needed for getValueAsText()

  ArduPar3Type getType(size_t position = 0)const; ///< to aid building of user interfaces at runtime

  void setValueFromText(const char *data);                          ///< digest incoming string that only contains the value to be set (should be able to parse output of "getValueAsText")
  void setValueFromFloat(float value, size_t position = 0);   ///< set Value using a int32_ting point Number (and possibly a some single component of a vector)
  void setValueFromInt32(int32_t value, size_t position = 0); ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)
  void setValueFromBool(bool value, size_t position = 0);     ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)
  void setValueFromDouble(double value, size_t position = 0); ///< set Value using a integer point Number. Bools can be handles by 0/1 (and possibly a some single component of a vector)

  // same here: these methods are primarily inteded to make interfaceing simple, imeplement them as you think they make sense
  float getValueAsFloat(size_t position = 0)const;
  int32_t getValueAsInt32(size_t position = 0)const;
  bool getValueAsBool(size_t position = 0)const;
  double getValueAsDouble(size_t position = 0)const;

protected:
  size_t uniqueId;
  bool isPersistent;     ///< do we support EEPROM persistency?
  NvsAddress nvsAddress; ///< an adress in eeprom memory for permanent storage of the parameter value. When <0 no storage will happen
  int32_t minValue;      ///< lower Bound of the Parameter. Received values are constrained to be in the Range of [minValue,maxValue]
  int32_t maxValue;      ///< upper Bound of the Parameter. Received values are constrained to be in the Range of [minValue,maxValue]

  int32_t value;
  int32_t *valuePointer = nullptr;       ///< points to the parameter value to be set
  bool valueHasChanged = false;          ///< flag to signal if value has been changed through a "setValue" Call since the last time it was checked
  ARDUPAR_CONST_CHAR *addressString;     ///< serial input is parsed for this command string, anything that follows is
  ARDUPAR_CONST_CHAR *descriptionString; ///< serial input is parsed for this command string, anything that follows is
  size_t addressLength;                  ///< to facilitate comparisons
};
