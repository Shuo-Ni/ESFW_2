#include <Arduino.h>
#include "C:\Users\15263\Documents\GitHub\ESFW_2\src\ProjectHeaders\RadioConTrolService.h"
#include "../ProjectHeaders/RadioTypes.h"
#include "../FrameworkHeaders/ES_Framework.h"
#include "C:\Users\15263\Documents\GitHub\ESFW_2\lib\EByte_LoRa_A28_Series_Library-master\LoRa_A28.h"

// 引脚定义
#define LORA_AUX_PIN  15
#define LORA_M0_PIN   19  
#define LORA_M1_PIN   18

// 模式配置
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
    
    // 初始化LoRa模块
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
    
    // 初始化当前模式
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
    // 事件判断
    if (ThisEvent.EventType == ES_MODE_SHIFT) 
    {
        // 模式切换部分
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
        // 发送处理部分
        if (loraModule != nullptr) {
            Serial.print("Sending:");
            Serial.print(ModeNames[CurrentModeIndex]);
            ResponseStatus result = loraModule->sendMessage(&CurrentMessage, sizeof(CurrentMessage));
            Serial.println(result.code == A28_SUCCESS ? "  OK" : "  FAIL");
        }
    }

    return ReturnEvent;

}