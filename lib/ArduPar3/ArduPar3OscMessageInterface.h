#pragma once
#include <OSCMessage.h>
#include "AbstractArduPar3.h"
#include "ArduPar3Collection.h"

class ArduPar3OscMessageInterface{
    public:
    static void applyMessageToPars(OSCMessage& message, ArduPar3Collection* parameterCollection=ArduPar3Collection::globalDefaultCollection);
};