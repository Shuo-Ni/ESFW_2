#ifndef RADIOTYPES_H
#define RADIOTYPES_H

#include <stdint.h>

// Remote Control Message Type Enumeration
typedef enum {
    MODE_A = 0x01,
    MODE_B = 0x02,  
    ACTION = 0x03,
    STOP = 0x04,
    CHECK_A_MODE = 0x05
} RadioMessageType_t;

// Remote Control Message Structure
struct RadioMessage {
    uint8_t Message;
    uint8_t var1;
};

#endif
