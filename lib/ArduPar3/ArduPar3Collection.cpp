#include "ArduPar3Collection.h"
#include "AbstractArduPar3.h"

ArduPar3Collection* ArduPar3Collection::globalDefaultCollection=nullptr;

size_t ArduPar3Collection::registerInstance(AbstractArduPar3 *instance)
{
  // register instance in the global array
  if (numInstancesRegistered < PAR_SETTINGS_MAX_NUMBER)
  {
    knownInstances[numInstancesRegistered] = instance;
    numInstancesRegistered++;
  }
  else
  {
    Serial.print(F("Max Parsetting instances exceeded, could not register"));
  }
  return(numInstancesRegistered);
}


/// write information about all parameter instances to a stream
void ArduPar3Collection::dumpParameterInfos(Stream *outStream)
{
  for (int i = 0; i < numInstancesRegistered; i++)
  {
    knownInstances[i]->dumpParameterInfo(outStream);
  }
};
/// this function automatically distributes incoming data from a stream to the parameter setting instances
void ArduPar3Collection::updateParametersFromStream(Stream *inStream, int timeout)
{
  if (!inStream->available())
    return; // if there is no data available, dont even start timeouts
  // read data from stream
  char buf[PAR_SETTINGS_BUFFER_SIZE];
  int bufPos = 0;
  int inByte;
  long lastCharMillis = millis();
  // even if the buffer is full, we keep on reading to flush the remaining data.
  bool receivedNewLine = false;
  while ((millis() - lastCharMillis) < timeout && !receivedNewLine)
  {
    inByte = inStream->read();
    if (inByte == '\n' || inByte == '\r')
      receivedNewLine = true;
    if (inByte > 0)
    {
      lastCharMillis = millis();
      if (bufPos < PAR_SETTINGS_BUFFER_SIZE - 1)
      {
        buf[bufPos] = inByte;
        bufPos++;
      }
    }
  }

  buf[bufPos] = 0;

  // let all the parameter setting instances have a look at the data
  if (bufPos > 0)
  {
    for (int i = 0; i < numInstancesRegistered; i++)
    {
      knownInstances[i]->parseCommand(buf);
    }
  }
};

   void ArduPar3Collection::loadAll(){
    for (int i = 0; i < numInstancesRegistered; i++)
    {
      knownInstances[i]->load();
    }
   }
   void ArduPar3Collection::saveAll(){
    for (int i = 0; i < numInstancesRegistered; i++)
    {
      knownInstances[i]->save();
    }
   }