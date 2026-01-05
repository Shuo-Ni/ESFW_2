#include <Arduino.h>
#include "C:\Users\15263\Documents\GitHub\ESFW_2\src\ProjectHeaders\RadioConTrolService.h"
#include "../ProjectHeaders/RadioTypes.h"
#include "../FrameworkHeaders/ES_Framework.h"
#include "C:\Users\15263\Documents\GitHub\ESFW_2\lib\EByte_LoRa_A28_Series_Library-master\LoRa_A28.h"

// 引脚定义 - 根据您的实际硬件修改
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
    
    if (loraModule->begin()) {
        Serial.println("✓ LoRa module initialized successfully");
    } else {
        Serial.println("✗ ERROR: LoRa module initialization failed!");
        return false;
    }
    
    // 初始化当前模式
    CurrentModeIndex = 0;
    CurrentMessage.Message = ModeSequence[CurrentModeIndex];
    CurrentMessage.var1 = 0x00;
    
    Serial.print("✓ Initial mode: ");
    Serial.println(ModeNames[CurrentModeIndex]);
    
    Serial.println("✓ RadioControlService Ready");
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

    switch (ThisEvent.EventType) {
        case ES_INIT:
            // 初始化完成，不需要额外操作
            break;
            
        case ES_MODE_SHIFT:
            // 模式切换事件
            CurrentModeIndex = (CurrentModeIndex + 1) % NUM_MODES;
            CurrentMessage.Message = ModeSequence[CurrentModeIndex];
            
            Serial.print("🔄 Mode changed to: ");
            Serial.println(ModeNames[CurrentModeIndex]);
            break;
            
        case ES_SEND:
            // 发送事件
            if (loraModule) {
                Serial.print(" Sending: ");
                Serial.print(ModeNames[CurrentModeIndex]);
                Serial.print(" (0x");
                Serial.print(CurrentMessage.Message, HEX);
                Serial.println(")");
                
                // 使用sendMessage发送消息
                ResponseStatus result = loraModule->sendMessage(&CurrentMessage, sizeof(CurrentMessage));
                
                if (result.code == A28_SUCCESS) {
                    Serial.println("✓ Message sent successfully!");
                } else {
                    Serial.print("✗ Send failed: ");
                    // 使用您statesNaming.h中的错误描述函数
                    Serial.println(result.getResponseDescription());
                }
            } else {
                Serial.println("✗ LoRa module not available!");
            }
            break;
            
        default:
            // 忽略其他事件
            break;
    }

    return ReturnEvent;
}