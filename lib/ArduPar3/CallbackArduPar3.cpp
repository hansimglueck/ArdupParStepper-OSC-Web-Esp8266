#include "CallbackArduPar3.h"

void CallbackArduPar3::setup(
    ARDUPAR_CONST_CHAR *address,
    ARDUPAR_CONST_CHAR *description,
    CallbackParameter, ///< A pointer to the function that will be triggered.
    ArduPar3Collection *collection  /// will register here and get a unique id if provided
)
{
    this->addressString = address;
    this->descriptionString = description;
    addressLength = strlen_P((const char PROGMEM *)addressString);
    this->callbackFunction = callbackFunction;
    if (collection != nullptr)
    {
        uniqueId = collection->registerInstance(this);
    }
}

/// set the attached integer parameter from a string that was received
void CallbackArduPar3::parseCommand(char *data)
{
    TRACE((F("Matching serial cmd")));
    TRACE((addressString));
    TRACE((F("to")));
    TRACELN((data));
    int foundDiff = strncmp_P(data, (const char PROGMEM *)addressString, addressLength);
    size_t dataLen = strlen(data);
    // check if the command continues beyond the address
    if (dataLen > addressLength)
    {
        if (data[addressLength] != ' ' && data[addressLength] != '\n' && data[addressLength] != '\r' && data[addressLength] != '\t')
            foundDiff = 1;
    }
    if (foundDiff == 0 && dataLen >= addressLength)
    {
        TRACE((F("Calling ")));
        TRACE((addressString));
        TRACE((F("\n")));

        callbackFunction();
    }
};

void CallbackArduPar3::dumpParameterInfo(Stream *out)const 
{

    out->print(F("trigger\t"));
    out->print(this->addressString);
    out->print(F("\t"));
    out->print(this->descriptionString);
    out->print(F("\n"));
}
