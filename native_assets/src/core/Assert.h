#pragma once
#include "Log.h"
#include <cassert>

#define SPHERE_ASSERT(check, msg) \
    if (!(check)) { \
        SPHERE_LOG_ERROR("Assertion failed: {0}", msg); \
        assert(check); \
    }\n