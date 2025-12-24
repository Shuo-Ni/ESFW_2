/*
simple timer class that uses Millis() as backend, for Arduino framework

- by jay
*/
#ifndef _CustomTimer_h
#define _CustomTimer_h

#include <Arduino.h>

class CustomTimer
{
    public:
        CustomTimer(void); // initialization

        void begin(uint16_t millis);
        bool check(void);
    
    private:
        uint32_t _expire;
        uint32_t _current;
        uint32_t _old;
};

#endif