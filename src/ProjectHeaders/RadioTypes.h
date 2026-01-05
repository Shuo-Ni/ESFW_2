#ifndef RADIOTYPES_H
#define RADIOTYPES_H

#include <stdint.h>

// 遥控消息类型枚举
typedef enum {
    MODE_A = 0x01,
    MODE_B = 0x02,  
    ACTION = 0x03,
    STOP = 0x04,
    CHECK_A_MODE = 0x05
} RadioMessageType_t;

// 遥控消息结构体
struct RadioMessage {
    uint8_t Message;
    uint8_t var1;
};

#endif