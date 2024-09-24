#include "SingleInt32ArduPar3.h"

void SingleInt32ArduPar3::setup(
     ARDUPAR_CONST_CHAR *address,
     ARDUPAR_CONST_CHAR *description,
    int32_t minValue,
    int32_t maxValue,
    int32_t startValue,
    int32_t *valuePointer,  ///< the setting can modify an arbitrary location im memory if you give it here.
    boolean isPersistent , ///< should it be possible to save this setting to NVS?
    ArduPar3Collection* collection ,/// will register here and get a unique id if provided
    bool useAutomaticNvsAddress,
    NvsAddress manualNvsAddress  ///< if you want a specific fixed adress, specify it here
)
{
    this->addressString = address;
    this->descriptionString = description;
    addressLength = strlen_P((const char PROGMEM *)addressString);
    if (valuePointer == 0){
        valuePointer = &this->value;
    }
    this->valuePointer = valuePointer;
    this->minValue = minValue;
    this->maxValue = maxValue;

    this->isPersistent=isPersistent;
    if(collection!=nullptr){
        uniqueId = collection->registerInstance(this);
    }

    if (isPersistent)
    {
        if (useAutomaticNvsAddress)
        {
            TRACE((F("Getting EEPROM. asking for auto address: ")));
            manualNvsAddress = NvsManager::getAddressFor(addressString,sizeof(int32_t));
        };
        this->nvsAddress = manualNvsAddress;
    }
    setValue(startValue);

};

// set the value and rpint some debug info
void SingleInt32ArduPar3::setValue(int32_t newValue)
{
    newValue = constrain(newValue, minValue, maxValue);
    TRACE((F("Setting ")));
    TRACE((this->addressString));
    TRACE((F(" to ")));
    TRACE((newValue));
    TRACE((F("\n")));
        if(newValue!=*valuePointer){
    *valuePointer = newValue;
        valueHasChanged= true; // flag: I got new data!
        }
};

// returns if the value has changed since the last call of this function
bool SingleInt32ArduPar3::getAndClearValueChangedFlag(){
bool returnValue=valueHasChanged;
valueHasChanged=false;
return returnValue;
}
void SingleInt32ArduPar3::save(){
    // save the new value
    if (isPersistent)
    {
        TRACE((F("Writing ")));
        TRACE(*valuePointer);
        TRACE(F(" to "));
        TRACELN(getAddress());
        NvsManager::write_bytes(valuePointer, nvsAddress, sizeof(int32_t));
    }
};

void SingleInt32ArduPar3::load(){
    if (isPersistent)
    {
        TRACE((F("Loading from EEPROM. Address: ")));
        //TRACE((int)(nvsAddress));
        int32_t tempInt32 = 0;
        bool success=NvsManager::read_bytes(&tempInt32, nvsAddress, sizeof(int32_t));
        if (success)
        {
            *valuePointer = tempInt32;
        }
        TRACE((F(" value:")));
        TRACELN((*valuePointer));
    }
}

void SingleInt32ArduPar3::dumpParameterInfo(Stream *out)const{
    out->print(F("int32_t\t"));
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

void SingleInt32ArduPar3::parseCommand(char *data){
    TRACE((F("Matching serial cmd")));
    TRACE((addressString));
    TRACE((F("to")));
    TRACELN((data));
    int foundDiff = strncmp_P(data, (const char PROGMEM *)addressString,addressLength);
    size_t dataLen=strlen(data);
    // check if the command continues beyond the address
    if(dataLen>addressLength){
        if(data[addressLength]!=' '&&data[addressLength]!='\n'&&data[addressLength]!='\r'&&data[addressLength]!='\t')foundDiff=1;
    }
    if (foundDiff==0&&dataLen>=addressLength){
        TRACE(F("matched:"));
        TRACELN((addressString));
        char* parameterData=data + strlen_P((const char PROGMEM *)addressString);
        setValueFromText(parameterData);
    }
}
// digest incoming string that only contains the value to be set (should be able to parse output of "getValueAsText")
void SingleInt32ArduPar3::setValueFromText(const char *data)
{
    setValue(atol(data));
}

size_t SingleInt32ArduPar3::getValueAsText(char *buffer, size_t buflength)const{
//  write value as a human readable text, with at most buflength characters
return snprintf(buffer,buflength,"%ld",(long)getValue());
}; 
size_t SingleInt32ArduPar3::getValueTextLength()const{
    return snprintf(NULL,0,"%ld",(long)getValue());
}                        

AbstractArduPar3::ArduPar3Type SingleInt32ArduPar3::getType(size_t position )const{
return position==0?AbstractArduPar3::ArduPar3TypeInt32:ArduPar3TypeNone;
};                   
void SingleInt32ArduPar3::setValueFromFloat(float value, size_t position )
{
    if(position==0)    setValue(value);
}
void SingleInt32ArduPar3::setValueFromInt32(int32_t value, size_t position ){
        if(position==0)    setValue(value);
}
void SingleInt32ArduPar3::setValueFromBool(bool value, size_t position ){
        if(position==0)    setValue(value);
}
void SingleInt32ArduPar3::setValueFromDouble(double value, size_t position ){
    if(position==0)    setValue(value);
}

// same here: these methods are primarily inteded to make interfaceing simple, imeplement them as you think they make sense
float SingleInt32ArduPar3::getValueAsFloat(size_t position )const{
    return (position==0?getValue():0);
}
int32_t SingleInt32ArduPar3::getValueAsInt32(size_t position )const{
    return (position==0?getValue():0);
}
bool SingleInt32ArduPar3::getValueAsBool(size_t position )const{
    return (position==0?getValue():0);
}
double SingleInt32ArduPar3::getValueAsDouble(size_t position )const{
    return (position==0?getValue():0);
}
