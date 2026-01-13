#include <Arduino.h>
#include "C:\Users\15263\Documents\GitHub\ESFW_2\src\ProjectHeaders\RadioConTrolService.h"
#include "../ProjectHeaders/RadioTypes.h"
#include "../FrameworkHeaders/ES_Framework.h"
#include "C:\Users\15263\Documents\GitHub\ESFW_2\lib\EByte_LoRa_A28_Series_Library-master\LoRa_A28.h"
#include <Bounce.h>

// Pin Definition
#define LORA_AUX_PIN  15
#define LORA_M0_PIN   19  
#define LORA_M1_PIN   18
#define BUTTON1_PIN 2    // Mode switch button
#define BUTTON2_PIN 3    // Send Button

// Mode Configuration
static const char* ModeNames[] = {"ModeA", "ModeB", "Action", "Stop", "CheckAMode"};
RadioState_t NowState[] = {ModeA,ModeB,Action,Stop,Check_A_Mode};
#define NUM_MODES 5

static uint8_t MyPriority;
static uint8_t CurrentModeIndex = 0;
static RadioMessage CurrentMessage;
static LoRa_A28* loraModule = nullptr;
static ES_Event_t State_A(ES_Event_t ThisEvent);
static ES_Event_t State_B(ES_Event_t ThisEvent);
static ES_Event_t State_Action(ES_Event_t ThisEvent);
static ES_Event_t State_Stop(ES_Event_t ThisEvent);
static ES_Event_t State_CheckAMode(ES_Event_t ThisEvent);
static ES_Event_t Event_handling(ES_Event_t ThisEvent);

static ES_Event_t State_A(ES_Event_t ThisEvent)
{
ES_Event_t ReturnEvent = ThisEvent; 
    Event_handling(ThisEvent);
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    
    return ReturnEvent;
}


static ES_Event_t State_B(ES_Event_t ThisEvent)
{
ES_Event_t ReturnEvent = ThisEvent; 
    Event_handling(ThisEvent);
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    
    return ReturnEvent; 
}


static ES_Event_t State_Action(ES_Event_t ThisEvent)
{
ES_Event_t ReturnEvent = ThisEvent; 
    Event_handling(ThisEvent);
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    
    return ReturnEvent;  
}


static ES_Event_t State_Stop(ES_Event_t ThisEvent)
{
ES_Event_t ReturnEvent = ThisEvent; 
    Event_handling(ThisEvent);
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    
    return ReturnEvent; 
}


static ES_Event_t State_CheckAMode(ES_Event_t ThisEvent)
{
ES_Event_t ReturnEvent = ThisEvent; 
    Event_handling(ThisEvent);
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    
    return ReturnEvent;  
}


static ES_Event_t Event_handling(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent = ThisEvent; 
if (ThisEvent.EventType == ES_MODE_SHIFT) 
    {
        // Mode Switching Section
       CurrentModeIndex++;
            if(CurrentModeIndex>=5)
            {
                CurrentModeIndex=0;
            }
            CurrentMessage.Message = NowState[CurrentModeIndex];
            
            Serial.print("Mode changed to:");
            Serial.println(ModeNames[CurrentModeIndex]);
        
    } else if (ThisEvent.EventType == ES_SEND) 
    {
        // Send Processing Section
        if (loraModule != nullptr) {
            Serial.print("Sending:");
            Serial.print(ModeNames[CurrentModeIndex]);
            ResponseStatus result = loraModule->sendMessage(&CurrentMessage, sizeof(CurrentMessage));
            Serial.println(result.code == A28_SUCCESS ? "  OK" : "  FAIL");
        }
    }
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    
    return ReturnEvent;  
}
/****************************************************************************
 Function
     InitRadioControlService
****************************************************************************/
bool InitRadioControlService(uint8_t Priority) {
    MyPriority = Priority;
    
    Serial.println("=== RadioControlService Initialization ===");
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    // Initialize the LoRa module
    loraModule = new LoRa_A28(&Serial4, LORA_AUX_PIN, LORA_M0_PIN, LORA_M1_PIN);
    
    if (loraModule->begin()) 
    {
        Serial.println("LoRa OK");
    } 
    else 
    {
        Serial.println("LoRa FAIL");
        return false;
    }
    
    // Initialize the current mode
    CurrentModeIndex = 0;
    CurrentMessage.Message = NowState[CurrentModeIndex];
    CurrentMessage.var1 = 0x00;
    
    Serial.print(" Initial mode: ");
    Serial.println(ModeNames[CurrentModeIndex]);

    return true;
}

/****************************************************************************
 Function
     PostRadioControlService
****************************************************************************/
bool PostRadioControlService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunRadioControlService
****************************************************************************/
ES_Event_t RunRadioControlService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    ReturnEvent.EventParam = 0;
    // Event Judgment
    switch (NowState[CurrentModeIndex])
    {
    case ModeA:
        State_A(ThisEvent);
        break;
    case ModeB:
        State_B(ThisEvent);
        break;
    case Action:
        State_Action(ThisEvent);
        break;
    case Stop:
        State_Stop(ThisEvent);
        break;
    case Check_A_Mode:
        State_CheckAMode(ThisEvent);
        break;
    
    default:
        break;
    }
    return ReturnEvent;

}

