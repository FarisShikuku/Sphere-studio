#pragma once
#include "Timer.h"
#include <string>

namespace sphere {
class ScopeProfiler {
public:
    ScopeProfiler(const std::string& name) : m_Name(name) {}
    ~ScopeProfiler() {}
private:
    std::string m_Name;
    Timer m_Timer;
};
}

#define SPHERE_PROFILE_SCOPE(name) sphere::ScopeProfiler profiler##__LINE__(name)\n