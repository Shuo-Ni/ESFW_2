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
#include "C:\Users\15263\Documents\GitHub\ESFW_2\src\ProjectHeaders\RadioConTrolService.h"

// Button Pin Definition
#define BUTTON1_PIN 2    // Mode switch button
#define BUTTON2_PIN 3    // Send Button
Bounce button1 = Bounce(BUTTON1_PIN, 10);
Bounce button2 = Bounce(BUTTON2_PIN, 10);


bool CheckModeShiftButton(void) 
{
    // Update Button Status
    button1.update();
    // Detection button pressed (falling edge)
    if (button1.fallingEdge()) 
    {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_MODE_SHIFT;
        ThisEvent.EventParam = 0;
        // Publish to Wireless Remote Control Service
        PostRadioControlService(ThisEvent);
        return true;
    }
    
    return false;
}


bool CheckSendButton(void)
 {
    
    // Update button status (using the Arduino library's Bounce)
    button2.update();
    
    // Detect button press falling edge
    if (button2.fallingEdge()) 
    {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_SEND;
        ThisEvent.EventParam = 0;
        // Use the Post function to publish to the wireless remote control service
        PostRadioControlService(ThisEvent);
        return true;
    }
    
    return false;
}
