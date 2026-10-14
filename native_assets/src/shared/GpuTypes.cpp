#include "GpuTypes.h"
#include "core/Assert.h"

namespace sphere {
// Static assertions ensuring std140/std430 layout consistency with SPIR-V
static_assert(sizeof(GpuCamera) % 16 == 0, "GpuCamera alignment must be 16-byte aligned");
static_assert(sizeof(GpuLight) % 16 == 0, "GpuLight alignment must be 16-byte aligned");
static_assert(sizeof(GpuMaterial) % 16 == 0, "GpuMaterial alignment must be 16-byte aligned");
static_assert(sizeof(GpuMeshlet) % 16 == 0, "GpuMeshlet alignment must be 16-byte aligned");
} // namespace sphere
