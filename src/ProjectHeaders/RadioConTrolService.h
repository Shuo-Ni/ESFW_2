#ifndef RADIOCONTROLSERVICE_H
#define RADIOCONTROLSERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "../FrameworkHeaders/ES_Events.h"

// Public function prototype
bool InitRadioControlService(uint8_t Priority);
bool PostRadioControlService(ES_Event_t ThisEvent);
ES_Event_t RunRadioControlService(ES_Event_t ThisEvent);

#endif
