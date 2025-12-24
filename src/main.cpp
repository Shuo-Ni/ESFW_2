#include <Arduino.h>
/****************************************************************************
 Module
     main.c
 Description
     starter main() function for Events and Services Framework applications
 Notes

*****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "FrameworkHeaders/ES_Configure.h"
#include "FrameworkHeaders/ES_Framework.h"

#include "ProjectHeaders/PinDefs.h"

void setup() 
{
  Serial.begin(9600); // enable usb debugging
  pinMode(entButtonPin, INPUT_PULLUP); // interal pull-up for both buttons
  ES_Return_t ErrorType = Success;

  // Your hardware initialization function calls go here


  // now initialize the Events and Services Framework and start it running
  ErrorType = ES_Initialize();
  if (ErrorType == Success)
  {
    ErrorType = ES_Run();
    Serial.println("running");
  }
  //if we got to here, there was an error
  switch (ErrorType)
  {
    case FailedPost:
    {
      ErrorType = Success; // a hack to get a readable assert message
      assert( FailedPost == ErrorType );
      Serial.println("Failed on attempt to Post");
    }
    break;
    case FailedPointer:
    {
      ErrorType = Success; // a hack to get a readable assert message
      assert( FailedPointer  == ErrorType );
      Serial.println("Failed on NULL pointer");
    }
    break;
    case FailedInit:
    {
      ErrorType = Success; // a hack to get a readable assert message
      assert( FailedInit  == ErrorType );
      Serial.println("Failed Initialization");
    }
    break;
    default:
    {
      ErrorType = Success; // a hack to get a readable assert message
      assert( FailedOther  == ErrorType );
      Serial.println("Other Failure");
    }
    break;
  }
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
