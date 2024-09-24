#include "ArduPar3.h"



ArduPar3Collection parameterCollection; // create a collection for parameters so we can conveniently do things with all of them at once

float someFloat=0; // we will controll the value of this number
SingleFloatArduPar3 someFloatParameter;

int32_t someInt=0;// we will controll the value of this number too
SingleInt32ArduPar3 someInt32Parameter;

// A function that can be triggered remotely to print all parameter states
void dumpFunction(){
    parameterCollection.dumpParameterInfos(&Serial);
}
CallbackArduPar3 dumpCallbackParameter;
// A function that can be triggered remotely to load parameter states
void loadFunction(){
    parameterCollection.loadAll();
}
CallbackArduPar3 loadCallbackParameter;

// A function that can be triggered remotely to save parameter states
void saveFunction(){
    parameterCollection.saveAll();
}
CallbackArduPar3 saveCallbackParameter;
//////////////////////////Everything WiFi/Osc
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
WiFiUDP wifiUdp;                        // will be used to send OSC status reports
void setupWifi(WiFiUDP &Udp,const char* ssid,const  char* password, uint16_t localPort);
void parseOscMessages()
{
  OSCMessage inmsg;
  int size = wifiUdp.parsePacket();
  if (size > 0)
  {
    Serial.println("got an UDP packet!");
    while (size--)
    {
      inmsg.fill(wifiUdp.read());
    }
    if (!inmsg.hasError())
    {
       Serial.println("applying message to pars..");
      ArduPar3OscMessageInterface::applyMessageToPars(inmsg);
    }
    else
    {
      auto error = inmsg.getError();
      Serial.print("OSC_Parse Error:");
      Serial.print(error);
    }
  }
}

void setup(){
        Serial.begin(115200);
/// try to get connected to a WiFi
    setupWifi(wifiUdp,"syntheticwire","doesnotmatter",8000);


    ArduPar3Collection::globalDefaultCollection=&parameterCollection; // use our parameter collection as a default
    someFloatParameter.setup(F("/someFloat"),F("An exquisite float"),0,10,&someFloat);
    someInt32Parameter.setup(F("/someInt"),F("An exquisite integer"),0,10000,&someInt);
    dumpCallbackParameter.setup(F("/dump"),F("Print Parameter Summary"),&dumpFunction);
    loadCallbackParameter.setup(F("/load"),F("Load Settings."),&loadFunction);
    saveCallbackParameter.setup(F("/save"),F("Save Settings."),&saveFunction);

    delay(500);
    parameterCollection.dumpParameterInfos(&Serial);

    
}

void loop(){
    parameterCollection.updateParametersFromStream(&Serial,1000);
    parseOscMessages();
}



void setupWifi(WiFiUDP &Udp,const char* ssid,const  char* password, uint16_t localPort)
{

    Serial.print("Connecting to wifi network \"");
    Serial.print(ssid);
    Serial.print("\" .");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    long start = millis();
    // try for ten seconds to connect every 500 ms (i.e. make 10000/500 = 20 attempts)
    while (WiFi.status()  != WL_CONNECTED && millis() - start < 10000)
    {
        Serial.print(".");
        delay(500);
    }

    // print result of connection attempt(s) on serial console
    if (WiFi.status()  != WL_CONNECTED)
    {
        Serial.print(" failed with status ");
        Serial.println(WiFi.status());
    }
    else
    {
        Serial.println(" succeeded");
        Serial.print("local IP address is ");
        Serial.println(WiFi.localIP());
        esp_wifi_set_ps( WIFI_PS_NONE);
        //esp_wifi_set_max_tx_power(84); // this seems to make things worse!
        Serial.print("Starting UDP connection to local port ");
        Serial.print(localPort);
        if (0 == Udp.begin(localPort))
        {
            // no socket available for use
            Serial.println(" ... failed");
        }
        Serial.println(" ... succeeded");
            
    }

}