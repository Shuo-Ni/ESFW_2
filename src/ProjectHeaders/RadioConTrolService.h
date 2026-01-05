#ifndef RADIOCONTROLSERVICE_H
#define RADIOCONTROLSERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "../FrameworkHeaders/ES_Events.h"

// 公开函数原型
bool InitRadioControlService(uint8_t Priority);
bool PostRadioControlService(ES_Event_t ThisEvent);
ES_Event_t RunRadioControlService(ES_Event_t ThisEvent);

#endif