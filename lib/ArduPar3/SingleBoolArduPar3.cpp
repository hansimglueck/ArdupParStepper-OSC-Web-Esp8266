#include "SingleBoolArduPar3.h"


void SingleBoolArduPar3::setup(
     ARDUPAR_CONST_CHAR *address,
     ARDUPAR_CONST_CHAR *description,
	   bool startValue,																		///< value will be initialized with this

    bool *valuePointer,  ///< the setting can modify an arbitrary location im memory if you give it here.
    bool isPersistent , ///< should it be possible to save this setting to NVS?
    ArduPar3Collection* collection ,/// will register here and get a unique id if provided
    bool useAutomaticNvsAddress,
    NvsAddress manualNvsAddress  ///< if you want a specific fixed adress, specify it here
)
{
    this->addressString = address;
    this->descriptionString = description;
    addressLength = strlen_P((const char PROGMEM *)addressString);
    if (valuePointer == 0)
        valuePointer = &this->value;
    this->valuePointer = valuePointer;

    this->isPersistent=isPersistent;
    if(collection!=nullptr){
        uniqueId = collection->registerInstance(this);
    }

    if (isPersistent)
    {
        if (useAutomaticNvsAddress)
        {
            TRACE((F("Getting EEPROM. asking for auto address: ")));
            manualNvsAddress = NvsManager::getAddressFor(addressString,sizeof(bool));
        };
        this->nvsAddress = manualNvsAddress;
    }

	 setValue(startValue);
};

// set the value and rpint some debug info
void SingleBoolArduPar3::setValue(bool newValue)
{
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
bool SingleBoolArduPar3::getAndClearValueChangedFlag(){
	bool returnValue=valueHasChanged;
	valueHasChanged=false;
	return returnValue;
}
void SingleBoolArduPar3::save(){
    // save the new value
    if (isPersistent)
    {
        TRACE((F("Writing ")));
        TRACE(*valuePointer);
        TRACE(F(" to "));
        TRACELN(getAddress());
        NvsManager::write_bytes(valuePointer, nvsAddress, sizeof(bool));
    }
};

void SingleBoolArduPar3::load(){
    if (isPersistent)
    {
        TRACE((F("Loading from EEPROM. Address: ")));
        TRACE((nvsAddress.crcOfKeyAsHex));
        bool tempVal = 0;
        NvsManager::read_bytes(&tempVal, nvsAddress, sizeof(bool));
			*valuePointer = tempVal;
        TRACE((F(" value:")));
        TRACELN((*valuePointer));
    }
}

void SingleBoolArduPar3::dumpParameterInfo(Stream *out)const{
    out->print(F("bool\t"));
    out->print(this->addressString);
    out->print(F("\t"));
    out->print(this->descriptionString);
    out->print(F("\t"));
    out->print(*valuePointer);
    out->print(F("\n"));
}

void SingleBoolArduPar3::parseCommand(char *data){
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
void SingleBoolArduPar3::setValueFromText(const char *data)
{
    setValue(atol(data));
}

size_t SingleBoolArduPar3::getValueAsText(char *buffer, size_t buflength)const{
//  write value as a human readable text, with at most buflength characters
return snprintf(buffer,buflength,"%ld",(long)getValue());
}; 
size_t SingleBoolArduPar3::getValueTextLength()const{
    return snprintf(NULL,0,"%ld",(long)getValue());
}                        

AbstractArduPar3::ArduPar3Type SingleBoolArduPar3::getType(size_t position )const{
return position==0?AbstractArduPar3::ArduPar3TypeBool:ArduPar3TypeNone;
};                   
void SingleBoolArduPar3::setValueFromFloat(float value, size_t position )
{
    if(position==0)    setValue(value>0.5f);
}
void SingleBoolArduPar3::setValueFromInt32(int32_t value, size_t position ){
        if(position==0)    setValue(value>0);
}
void SingleBoolArduPar3::setValueFromBool(bool value, size_t position ){
        if(position==0)    setValue(value);
}
void SingleBoolArduPar3::setValueFromDouble(double value, size_t position ){
    if(position==0)    setValue(value>0.5f);
}

// same here: these methods are primarily inteded to make interfaceing simple, implement them as you think they make sense
float SingleBoolArduPar3::getValueAsFloat(size_t position )const{
    return (position==0?getValue():0);
}
int32_t SingleBoolArduPar3::getValueAsInt32(size_t position )const{
    return (position==0?getValue():0);
}
bool SingleBoolArduPar3::getValueAsBool(size_t position )const{
    return (position==0?getValue():0);
}
double SingleBoolArduPar3::getValueAsDouble(size_t position )const{
    return (position==0?getValue():0);
}
