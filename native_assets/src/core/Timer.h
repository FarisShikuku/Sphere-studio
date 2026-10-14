#pragma once
#include <chrono>

namespace sphere {
class Timer {
public:
    Timer() { Reset(); }
    void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }
    float ElapsedSeconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.000001f;
    }
    float ElapsedMillis() const {
        return ElapsedSeconds() * 1000.0f;
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};
}\n