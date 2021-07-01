#pragma once

#ifndef BEGIN_SYNSENSE_NAMESPACE
#define BEGIN_SYNSENSE_NAMESPACE \
    namespace synsense           \
    {
#endif

#ifndef END_SYNSENSE_NAMESPACE
#define END_SYNSENSE_NAMESPACE }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)    \
    {                     \
        if (x != nullptr) \
        {                 \
            delete (x);   \
            x = nullptr;  \
        }                 \
    }
#endif