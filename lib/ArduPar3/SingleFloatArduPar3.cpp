#include "SingleFloatArduPar3.h"

void SingleFloatArduPar3::setup(
    ARDUPAR_CONST_CHAR *address,
    ARDUPAR_CONST_CHAR *description,
    float minValue,
    float maxValue,
	 float startValue ,																			///< value will be initialized with this
    float *valuePointer,            ///< the setting can modify an arbitrary location im memory if you give it here.
    boolean isPersistent,           ///< should it be possible to save this setting to NVS?
    ArduPar3Collection *collection, /// will register here and get a unique id if provided
    bool useAutomaticNvsAddress,
    NvsAddress manualNvsAddress ///< if you want a specific fixed adress, specify it here
)
{
    this->addressString = address;
    this->descriptionString = description;
    addressLength = strlen_P((const char PROGMEM *)addressString);
    if (valuePointer == 0)
        valuePointer = &this->value;
    this->valuePointer = valuePointer;
    this->minValue = minValue;
    this->maxValue = maxValue;


    this->isPersistent = isPersistent;
    if (collection != nullptr)
    {
        uniqueId = collection->registerInstance(this);
    }

    if (isPersistent)
    {
        if (useAutomaticNvsAddress)
        {
            TRACE((F("Getting EEPROM. asking for auto address: ")));
            manualNvsAddress = NvsManager::getAddressFor(addressString, sizeof(float));
        };
        this->nvsAddress = manualNvsAddress;
    }

    setValue(startValue);
};

// set the value and rpint some debug info
void SingleFloatArduPar3::setValue(float newValue)
{
    newValue = constrain(newValue, minValue, maxValue);
    TRACE((F("Setting ")));
    TRACE((this->addressString));
    TRACE((F(" to ")));
    TRACE((newValue));
    TRACE((F("\n")));
    if (newValue != *valuePointer)
    {
    TRACE((F("is new!")));
        *valuePointer = newValue;
        valueHasChanged = true; // flag: I got new data!
    }
};

// returns if the value has changed since the last call of this function
bool SingleFloatArduPar3::getAndClearValueChangedFlag()
{
    bool returnValue = valueHasChanged;
    valueHasChanged = false;
    return returnValue;
}
void SingleFloatArduPar3::save()
{
    // save the new value
    if (isPersistent)
    {
        TRACE((F("Writing ")));
        TRACE(*valuePointer);
        TRACE(F(" to "));
        TRACELN(getAddress());
        NvsManager::write_bytes(valuePointer, nvsAddress, sizeof(float));
    }
};

void SingleFloatArduPar3::load()
{
    if (isPersistent)
    {
        TRACE((F("Loading from EEPROM. Address: ")));
        TRACE((int)(nvsAddress.crcOfKeyAsHex));
        float tempFloat = 0;
        bool success=NvsManager::read_bytes(&tempFloat, nvsAddress, sizeof(float));
        if (success)
        {
            *valuePointer = tempFloat;
        }
        TRACE((F(" value:")));
        TRACELN((*valuePointer));
    }
}

void SingleFloatArduPar3::dumpParameterInfo(Stream *out) const
{
    out->print(F("float\t"));
    out->print(this->addressString);
    out->print(F("\t"));
    out->print(this->descriptionString);
    out->print(F("\t"));
    out->print(*valuePointer);
    out->print(F("\t"));
    out->print(minValue);
    out->print(F("\t"));
    out->print(maxValue);
    out->print(F("\n"));
}

void SingleFloatArduPar3::parseCommand(char *data)
{
    int foundDiff = strncmp_P(data, (const char PROGMEM *)addressString, addressLength);
    size_t dataLen = strlen(data);
    TRACE((F("Matching serial cmd ")));
    TRACE((addressString));
    TRACE((F(" to ")));
    TRACE((data));
    TRACE((F(" addlen: ")));
    TRACE(addressLength);
    TRACE((F(" datalen: ")));
    TRACELN(dataLen);



    // check if the command continues beyond the address
    if (dataLen > addressLength)
    {
        if (data[addressLength] != ' ' && data[addressLength] != '\n' && data[addressLength] != '\r' && data[addressLength] != '\t')
            foundDiff = 1;
    }
    if (foundDiff == 0 && dataLen >= addressLength)
    {
        TRACE(F("matched:"));
        TRACELN((addressString));
        char *parameterData = data + strlen_P((const char PROGMEM *)addressString);
        setValueFromText(parameterData);
    }
}
// digest incoming string that only contains the value to be set (should be able to parse output of "getValueAsText")
void SingleFloatArduPar3::setValueFromText(const char *data)
{
    setValue(atof(data));
}

size_t SingleFloatArduPar3::getValueAsText(char *buffer, size_t buflength) const
{
    //  write value as a human readable text, with at most buflength characters
    return snprintf(buffer, buflength, "%f", getValue());
};
size_t SingleFloatArduPar3::getValueTextLength()const
{
    return snprintf(NULL, 0, "%f", getValue());
}

AbstractArduPar3::ArduPar3Type SingleFloatArduPar3::getType(size_t position)const
{
    return position == 0 ? AbstractArduPar3::ArduPar3TypeFloat : ArduPar3TypeNone;
};
void SingleFloatArduPar3::setValueFromFloat(float value, size_t position)
{
    if (position == 0)
        setValue(value);
}
void SingleFloatArduPar3::setValueFromInt32(int32_t value, size_t position)
{
    if (position == 0)
        setValue(value);
}
void SingleFloatArduPar3::setValueFromBool(bool value, size_t position)
{
    if (position == 0)
        setValue(value);
}
void SingleFloatArduPar3::setValueFromDouble(double value, size_t position)
{
    if (position == 0)
        setValue(value);
}

// same here: these methods are primarily inteded to make interfaceing simple, imeplement them as you think they make sense
float SingleFloatArduPar3::getValueAsFloat(size_t position) const
{
    return (position == 0 ? getValue() : 0);
}
int32_t SingleFloatArduPar3::getValueAsInt32(size_t position)const
{
    return (position == 0 ? getValue() : 0);
}
bool SingleFloatArduPar3::getValueAsBool(size_t position)const
{
    return (position == 0 ? getValue() : 0);
}
double SingleFloatArduPar3::getValueAsDouble(size_t position)const
{
    return (position == 0 ? getValue() : 0);
}
