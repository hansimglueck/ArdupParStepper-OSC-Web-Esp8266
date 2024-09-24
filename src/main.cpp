#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ArduPar3.h>
#include <ArduPar3OscMessageInterface.h>

#include <AccelStepper.h>

///////// Everything that has to do with the steppers TODO: set pins
AccelStepper stepper1(AccelStepper::DRIVER, D4, D2);
AccelStepper stepper2(AccelStepper::DRIVER, D5, D6);
float stepperSpeed1 = 100;
float stepperSpeed2 = 100;

/////////Everthing that has to do with the remote settable parameters
ArduPar3Collection parameterCollection; // create a collection for parameters so we can conveniently do things with all of them at once

SingleFloatArduPar3 stepperSpeedPar1; // a setting for a float number
SingleFloatArduPar3 stepperSpeedPar2; //

void dumpFunction()
{
  parameterCollection.dumpParameterInfos(&Serial);
}
CallbackArduPar3 dumpCallbackParameter;

// A function that can be triggered remotely to load parameter states
void loadFunction()
{
  parameterCollection.loadAll();
}
CallbackArduPar3 loadCallbackParameter;

// A function that can be triggered remotely to save parameter states
void saveFunction()
{
  parameterCollection.saveAll();
}
CallbackArduPar3 saveCallbackParameter;

//////////////////////////Everything WiFi/Osc
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP wifiUdp; // will be used to send OSC status reports
void setupWifi(WiFiUDP &Udp, const char *ssid, const char *password, uint16_t localPort);
void parseOscMessages();
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up...");
  delay(500);

  /// try to get connected to a WiFi
  setupWifi(wifiUdp, "Wokule Intern", "kontrollverlust", 8000);

  // steppers

   stepper1.setMaxSpeed(200.0);
   stepper1.setAcceleration(100.0);
   stepper2.setMaxSpeed(200.0);
   stepper2.setAcceleration(100.0);

  // remote control
  ArduPar3Collection::globalDefaultCollection = &parameterCollection; // use our parameter collection as a default
  stepperSpeedPar1.setup(("/step1Speed"), ("Stepper 1 Speed"), 0, 10000, 500, &stepperSpeed1);
  stepperSpeedPar2.setup(("/step2Speed"), ("Stepper 2 Speed"), 0, 10000, 500, &stepperSpeed2);

  dumpCallbackParameter.setup(("/dump"), ("Print Parameter Summary"), &dumpFunction);
  loadCallbackParameter.setup(("/load"), ("Load Settings."), &loadFunction);
  saveCallbackParameter.setup(("/save"), ("Save Settings."), &saveFunction);

  delay(500);
  parameterCollection.loadAll(); // load the parameters from the EEPROM
  parameterCollection.dumpParameterInfos(&Serial);
}

void loop()
{

  parameterCollection.updateParametersFromStream(&Serial, 1000);
  parseOscMessages();
  bool saveFlag = false;
  if (stepperSpeedPar1.getAndClearValueChangedFlag())
  {
    stepper1.setSpeed(stepperSpeed1);
    saveFlag = true;
  }
  if (stepperSpeedPar2.getAndClearValueChangedFlag())
  {
    stepper2.setSpeed(stepperSpeed2);
    saveFlag = true;
  }
  //autosave when a parameter has changed
  if (saveFlag)
    parameterCollection.saveAll();

  stepper1.runSpeed();
  stepper2.runSpeed();
}

void setupWifi(WiFiUDP &Udp, const char *ssid, const char *password, uint16_t localPort)
{

  Serial.print("Connecting to wifi network \"");
  Serial.print(ssid);
  Serial.print("\" .");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  long start = millis();
  // try for ten seconds to connect every 500 ms (i.e. make 10000/500 = 20 attempts)
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
  {
    Serial.print(".");
    delay(500);
  }

  // print result of connection attempt(s) on serial console
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(" failed with status ");
    Serial.println(WiFi.status());
  }
  else
  {
    Serial.println(" succeeded");
    Serial.print("local IP address is ");
    Serial.println(WiFi.localIP());
    // esp_wifi_set_ps( WIFI_PS_NONE); // disable power saving
    // esp_wifi_set_max_tx_power(84); // this seems to make things worse!
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