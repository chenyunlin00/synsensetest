#pragma once
#include <stdint.h>

#include "CommonDef.h"

BEGIN_SYNSENSE_NAMESPACE

#pragma pack(1)
struct Event
{
    uint64_t id;
    uint32_t timestamp;

    Event()
    {
        id = 0;
        timestamp = 0;
    }

    Event(uint64_t i, uint32_t t) : id(i), timestamp(t) {}

    Event(const Event &event)
    {
        id = event.id;
        timestamp = event.timestamp;
    }
};
#pragma pack()

END_SYNSENSE_NAMESPACE