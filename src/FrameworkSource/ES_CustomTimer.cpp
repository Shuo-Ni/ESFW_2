#include "../FrameworkHeaders/ES_CustomTimer.h"

#include <Arduino.h>

CustomTimer::CustomTimer(void)
{
    _expire = 0;
    _current = 0;
    _old = 0;
}

void CustomTimer::begin(uint16_t millisDuration)
{
    _expire = millis() + millisDuration; // set the expire time to be millis into the future;
}

bool CustomTimer::check(void)
{
    _current = millis();
    if (_current > _expire && _old <= _expire)
    {
        _old = _current;
        return true;
    }
    return false;
}