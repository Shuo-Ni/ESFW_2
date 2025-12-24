/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1

 Description
   This is the sample for writing event checkers along with the event
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.

 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 13:36 jec     initial version
****************************************************************************/
#include "../ProjectHeaders/PinDefs.h"

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "../FrameworkHeaders/ES_Configure.h"
// This gets us the prototype for ES_PostAll
#include "../FrameworkHeaders/ES_Framework.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "../FrameworkHeaders/ES_Events.h"
// if you want to use distribution lists then you need those function
// definitions too.
#include "../FrameworkHeaders/ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "../FrameworkHeaders/ES_ServiceHeaders.h"
// include our own prototypes to insure consistency between header &
// actual functionsdefinition
#include "../ProjectHeaders/EventCheckers.h"

#include "../FrameworkHeaders/ES_CustomTimer.h"

// create your custom timers here, then add them to other cpp files using extern
CustomTimer TestTimer;

// This is the event checking function sample. It is not intended to be
// included in the module. It is only here as a sample to guide you in writing
// your own event checkers

// hardware libraries
#include <Bounce.h>

// initialize button object
Bounce entButton = Bounce(entButtonPin, 100);

bool Check4Keystroke(void)
{
  if (Serial.available())   // new key waiting?
  {
    ES_Event_t ThisEvent;
    ThisEvent.EventType   = ES_NEW_KEY;
    ThisEvent.EventParam  = Serial.read();
    ES_PostAll(ThisEvent);
    return true;
  }
  return false;
}

bool CheckEntButton(void)
{
  if (entButton.update())
  {
    if (entButton.fallingEdge())
    {
      ES_Event_t ThisEvent;
      ThisEvent.EventType = ES_EntButton;
      PostTestHarnessService0(ThisEvent);
      Serial.println("Event Checker: enter button");
      return true;
    }
  }
  return false;
}

bool CheckTestTimer(void)
{
  if (TestTimer.check())
  {
    ES_Event_t ThisEvent;
    ThisEvent.EventType = ES_TestTimer;
    PostTestHarnessService0(ThisEvent);
    Serial.println("Event Checker: timer expire");
    return true;
  }
  return false;
}