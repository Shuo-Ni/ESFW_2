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
#include "C:\Users\15263\Documents\GitHub\ESFW_2\src\ProjectHeaders\RadioConTrolService.h"

// 按钮引脚定义 - 根据您的实际硬件修改
#define BUTTON1_PIN 2    // 模式切换按钮
#define BUTTON2_PIN 3    // 发送按钮

// 按钮对象
Bounce button1 = Bounce(BUTTON1_PIN, 10);
Bounce button2 = Bounce(BUTTON2_PIN, 10);

/****************************************************************************
 Function
    CheckModeShiftButton
****************************************************************************/
bool CheckModeShiftButton(void) {
    static bool initialized = false;
    
    // 首次调用时初始化按钮
    if (!initialized) {
        pinMode(BUTTON1_PIN, INPUT_PULLUP);
        initialized = true;
    }
    
    // 更新按钮状态
    button1.update();
    
    // 检测按钮按下（下降沿）
    if (button1.fallingEdge()) {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_MODE_SHIFT;
        ThisEvent.EventParam = 0;
        
        // 发布到无线遥控服务
        PostRadioControlService(ThisEvent);
        return true;
    }
    
    return false;
}

/****************************************************************************
 Function
    CheckSendButton
****************************************************************************/
bool CheckSendButton(void) {
    static bool initialized = false;
    
    // 首次调用时初始化按钮
    if (!initialized) {
        pinMode(BUTTON2_PIN, INPUT_PULLUP);
        initialized = true;
    }
    
    // 更新按钮状态
    button2.update();
    
    // 检测按钮按下（下降沿）
    if (button2.fallingEdge()) {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_SEND;
        ThisEvent.EventParam = 0;
        
        // 发布到无线遥控服务
        PostRadioControlService(ThisEvent);
        return true;
    }
    
    return false;
}