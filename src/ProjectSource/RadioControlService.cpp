#include <Arduino.h>
#include "C:\Users\15263\Documents\GitHub\ESFW_2\src\ProjectHeaders\RadioConTrolService.h"
#include "../ProjectHeaders/RadioTypes.h"
#include "../FrameworkHeaders/ES_Framework.h"
#include "C:\Users\15263\Documents\GitHub\ESFW_2\lib\EByte_LoRa_A28_Series_Library-master\LoRa_A28.h"

// Pin Definition
#define LORA_AUX_PIN  15
#define LORA_M0_PIN   19  
#define LORA_M1_PIN   18

// Mode Configuration
static const uint8_t ModeSequence[] = {MODE_A, MODE_B, ACTION, STOP, CHECK_A_MODE};
static const char* ModeNames[] = {"ModeA", "ModeB", "Action", "Stop", "CheckAMode"};
#define NUM_MODES 5

static uint8_t MyPriority;
static uint8_t CurrentModeIndex = 0;
static RadioMessage CurrentMessage;
static LoRa_A28* loraModule = nullptr;

/****************************************************************************
 Function
     InitRadioControlService
****************************************************************************/
bool InitRadioControlService(uint8_t Priority) {
    MyPriority = Priority;
    
    Serial.println("=== RadioControlService Initialization ===");
    
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
    CurrentMessage.Message = ModeSequence[CurrentModeIndex];
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
    if (ThisEvent.EventType == ES_MODE_SHIFT) 
    {
        // Mode Switching Section
       CurrentModeIndex++;
            if(CurrentModeIndex>=5)
            {
                CurrentModeIndex=0;
            }
            CurrentMessage.Message = ModeSequence[CurrentModeIndex];
            
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

    return ReturnEvent;

}
