#pragma once
#include "AbstractArduPar3.h"
#include "ArduPar3Helpers.h"
class ArduPar3Collection
{
public:
  static ArduPar3Collection* globalDefaultCollection; ///< can be used by any ardupar as a default
  // some static members for keeping track of all the instances
   AbstractArduPar3 *knownInstances[PAR_SETTINGS_MAX_NUMBER];
   size_t numInstancesRegistered=0;
   size_t registerInstance(AbstractArduPar3 *instance);       ///returns a unique ID 
   void dumpParameterInfos(Stream *outStream);               ///< write information about all parameter instances to a stream
   void updateParametersFromStream(Stream *inStream, int timeout); ///< distributes incoming data from a stream to the 
   void loadAll();
   void saveAll();
};
