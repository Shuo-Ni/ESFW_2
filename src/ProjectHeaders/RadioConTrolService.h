#ifndef RADIOCONTROLSERVICE_H
#define RADIOCONTROLSERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "../FrameworkHeaders/ES_Events.h"
typedef enum {
    ModeA,      
    ModeB,      
    Action,       
    Stop,
    Check_A_Mode           
} RadioState_t;//五种状态
// Public function prototype
bool InitRadioControlService(uint8_t Priority);
bool PostRadioControlService(ES_Event_t ThisEvent);
ES_Event_t RunRadioControlService(ES_Event_t ThisEvent);

#endif
