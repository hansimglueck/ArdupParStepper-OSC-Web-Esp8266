#pragma once

#if defined(__AVR__)
#define ARDUPAR_CONST_CHAR __FlashStringHelper
#endif
#if defined(__XTENSA__)
#define ARDUPAR_CONST_CHAR const char
#endif



#define DEBUG 1
#define TRACE(x)       \
  do                   \
  {                    \
    if (DEBUG)         \
      Serial.print(x); \
  } while (0)
#define TRACELN(x)       \
  do                     \
  {                      \
    if (DEBUG)           \
      Serial.println(x); \
  } while (0)

// the instances of classes derived from AbstractArduPar3 maintain a global list. You can change the maximum number of settings here.
#define PAR_SETTINGS_MAX_NUMBER 32

// this determines the maximum parameter command length
#define PAR_SETTINGS_BUFFER_SIZE 32

