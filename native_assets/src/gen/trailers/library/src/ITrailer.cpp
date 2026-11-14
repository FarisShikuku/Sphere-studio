#include "ITrailer.h"

namespace sphere::trailers {

bool ITrailer::LoadCargo(std::shared_ptr<cargo::ICargo> cargo, uint32_t quantity) {
    if (!cargo || quantity == 0) {
        ClearCargo();
        return false;
    }
    m_LoadedCargo = std::move(cargo);
    m_LoadedQuantity = quantity;
    return true;
}

void ITrailer::ClearCargo() {
    m_LoadedCargo.reset();
    m_LoadedQuantity = 0;
}

float ITrailer::GetTotalMassKg() const {
    float baseMass = GetDef().empty_weight_kg;
    if (m_LoadedCargo && m_LoadedQuantity > 0) {
        baseMass += m_LoadedCargo->GetDef().weight_kg * static_cast<float>(m_LoadedQuantity);
    }
    return baseMass;
}

} // namespace sphere::trailers
