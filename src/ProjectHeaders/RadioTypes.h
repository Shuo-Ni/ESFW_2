#ifndef RADIOTYPES_H
#define RADIOTYPES_H

#include <stdint.h>

// Remote Control Message Structure
struct RadioMessage {
    uint8_t Message;
    uint8_t var1;
};

#endif
