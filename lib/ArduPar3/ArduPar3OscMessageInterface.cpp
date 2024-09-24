#include "ArduPar3OscMessageInterface.h"

void ArduPar3OscMessageInterface::applyMessageToPars(OSCMessage &message, ArduPar3Collection *parameterCollection)
{
    TRACE(F("message address:"));
    TRACELN(message.getAddress());
    for (int parIdx = 0; parIdx < parameterCollection->numInstancesRegistered; parIdx++)
    {
        TRACE(F("par no"));
        TRACE(parIdx);
        AbstractArduPar3 *curPar = parameterCollection->knownInstances[parIdx];
        TRACE("-");
        TRACE(curPar->getAddress());
        TRACELN((strcmp_P(message.getAddress(), (const char PROGMEM *)curPar->getAddress())));
        if ((strcmp_P(message.getAddress(), (const char PROGMEM *)curPar->getAddress()) == 0))
        {
            TRACE("No pars:");
        TRACELN(message.size());
            for (int argIdx = 0; argIdx < message.size(); argIdx++)
            {
                TRACE(message.getType(argIdx));
                switch (message.getType(argIdx))
                {
                case 'i':
                    curPar->setValueFromInt32(message.getInt(argIdx));
                    break;
                case 'f':
                    curPar->setValueFromFloat(message.getFloat(argIdx));
                    break;
                case 'd':
                    curPar->setValueFromDouble(message.getDouble(argIdx));
                    break;
                case 'T':
                    curPar->setValueFromBool(message.getBoolean(argIdx));
                    break;
                case 'F':
                    curPar->setValueFromBool(message.getBoolean(argIdx));
                    break;
                default:
                    curPar->trigger();
                }
            }
        }
    }
}
