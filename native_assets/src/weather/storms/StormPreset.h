#pragma once
#include "StormParams.h"

namespace sphere::weather {

class StormPreset {
public:
    StormPreset() = default;
    void SetParams(const StormParams& params) { m_params = params; }
    const StormParams& GetParams() const { return m_params; }

private:
    StormParams m_params{};
};

} // namespace sphere::weather
