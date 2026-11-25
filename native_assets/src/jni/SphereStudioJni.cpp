#include <jni.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <unordered_map>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <vulkan/vulkan.h>
#include "shared/materials/spec/shared_spec.h"
#include "core/Application.h"
#include "core/Log.h"
#include "shared/GeneratorRegistry.h"
#include "vulkan/VulkanNativeSceneRenderer.h"
#include "weather/clouds/CloudRaymarch.h"
#include "weather/clouds/CloudGen.h"
#include "scene/NativeMeshRasterizer.h"
#include "CargoRegistry.h"
#include "TrailerRegistry.h"
#include "VehicleRegistry.h"
#include "EngineAudioSystem.h"
#include "WeatherAudioSystem.h"
#include "WorldStreamingMeshlets.h"
#include "WaterSimulationEngine.hpp"
#include "BuildingSystem.h"
#define LOG_TAG "SphereStudioNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace {
inline sphere::Vec4 UnpackArgbToVec4(jint argb, float alphaOverride = 1.0f) {
    float r = static_cast<float>((argb >> 16) & 0xFF) / 255.0f;
    float g = static_cast<float>((argb >> 8) & 0xFF) / 255.0f;
    float b = static_cast<float>(argb & 0xFF) / 255.0f;
    return sphere::Vec4(r, g, b, alphaOverride);
}

inline bool Vec4Near(const sphere::Vec4& a, const sphere::Vec4& b) {
    return std::abs(a.x - b.x) < 1e-3f &&
           std::abs(a.y - b.y) < 1e-3f &&
           std::abs(a.z - b.z) < 1e-3f &&
           std::abs(a.w - b.w) < 1e-3f;
}

inline bool FloatNear(float a, float b) {
    return std::abs(a - b) < 1e-4f;
}

inline float FiniteOr(float v, float fallback) {
    return std::isfinite(v) ? v : fallback;
}

std::mutex gGeometryRegistryMutex;
} // namespace

extern "C" {
JNIEXPORT jboolean JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeInitEngine(JNIEnv* env, jclass clazz) {
    LOGI("Sphere Studio Native C++20 Core initializing with Sascha Willems Vulkan 1.3 pipeline...");
    sphere::Log::Init();
    sphere::GeneratorRegistry::Get().Initialize();
    procedural::buildings::BuildingSystem::instance().initialize(procedural::GeneratorContext{});
    LOGI("Native Vulkan C++ engine initialized. Total registered KTX2 textures: %zu", sphere::textures::kRegisteredTextures.size());
    return JNI_TRUE;
}
JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetEngineVersion(JNIEnv* env, jclass clazz) {
    std::string version = "SphereStudioEngine 1.3 (Sascha Willems Vulkan Framework / C++20 / NDK r26b)";
    return env->NewStringUTF(version.c_str());
}
JNIEXPORT jint JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetRegisteredTextureCount(JNIEnv* env, jclass clazz) {
    return static_cast<jint>(sphere::textures::kRegisteredTextures.size());
}
JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTextureId(JNIEnv* env, jclass clazz, jint index) {
    if (index >= 0 && index < static_cast<jint>(sphere::textures::kRegisteredTextures.size())) {
        return env->NewStringUTF(sphere::textures::kRegisteredTextures[index].id);
    }
    return env->NewStringUTF("");
}
JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTextureDisplayName(JNIEnv* env, jclass clazz, jint index) {
    if (index >= 0 && index < static_cast<jint>(sphere::textures::kRegisteredTextures.size())) {
        return env->NewStringUTF(sphere::textures::kRegisteredTextures[index].displayName);
    }
    return env->NewStringUTF("");
}
JNIEXPORT jint JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTextureBindlessSlot(JNIEnv* env, jclass clazz, jint index) {
    if (index >= 0 && index < static_cast<jint>(sphere::textures::kRegisteredTextures.size())) {
        return static_cast<jint>(sphere::textures::kRegisteredTextures[index].bindlessSlot);
    }
    return -1;
}
JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTextureKtx2Path(JNIEnv* env, jclass clazz, jint index) {
    if (index >= 0 && index < static_cast<jint>(sphere::textures::kRegisteredTextures.size())) {
        return env->NewStringUTF(sphere::textures::kRegisteredTextures[index].ktx2Path);
    }
    return env->NewStringUTF("");
}
JNIEXPORT jfloat JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeEvaluateCookTorrance(
    JNIEnv* env, jclass clazz, jfloat roughness, jfloat metallic, jfloat NdotL, jfloat NdotV, jfloat NdotH, jfloat VdotH
) {
    // Exact Sascha Willems Split-Sum Cook-Torrance Microfacet BRDF
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    
    // Normal Distribution Function (GGX / Trowbridge-Reitz)
    float denom = (NdotH * NdotH * (alpha2 - 1.0f) + 1.0f);
    float D = alpha2 / (3.14159265f * denom * denom + 1e-5f);
    
    // Geometric Shadowing (Smith Schlick-GGX)
    float k = (roughness + 1.0f);
    k = (k * k) / 8.0f;
    float g1 = NdotL / (NdotL * (1.0f - k) + k + 1e-5f);
    float g2 = NdotV / (NdotV * (1.0f - k) + k + 1e-5f);
    float G = g1 * g2;
    
    // Fresnel-Schlick
    float F0 = 0.04f * (1.0f - metallic) + metallic;
    float F = F0 + (1.0f - F0) * pow(1.0f - VdotH, 5.0f);
    
    float specular = (D * G * F) / (4.0f * NdotL * NdotV + 1e-4f);
    return specular;
}

JNIEXPORT jfloat JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeEvaluateSpotAttenuation(
    JNIEnv* env, jclass clazz, jfloat cosAngle, jfloat cosInner, jfloat cosOuter, jfloat exponent
) {
    float diff = cosInner - cosOuter;
    if (diff < 0.001f) diff = 0.001f;
    float atten = (cosAngle - cosOuter) / diff;
    if (atten < 0.0f) atten = 0.0f;
    if (atten > 1.0f) atten = 1.0f;
    return std::pow(atten, exponent);
}

JNIEXPORT jfloat JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeEvaluateDistanceAttenuation(
    JNIEnv* env, jclass clazz, jfloat distance, jfloat range
) {
    if (distance <= 0.001f) distance = 0.001f;
    float invSq = 1.0f / (distance * distance);
    float factor = distance / range;
    float smoothFactor = 1.0f - factor * factor * factor * factor;
    if (smoothFactor < 0.0f) smoothFactor = 0.0f;
    return invSq * (smoothFactor * smoothFactor);
}

// ---------------------------------------------------------------------------
// Native Vulkan C++ Scene WSI Bridge (Sascha Willems Android Pattern)
// ---------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeCreateVulkanSurface(
    JNIEnv* env, jclass clazz, jobject surface, jint width, jint height
) {
    LOGI("JNI: nativeCreateVulkanSurface requested with dimensions %dx%d", width, height);
    if (!surface) {
        LOGI("JNI: surface is null, cannot create native window");
        return JNI_FALSE;
    }
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (!window) {
        LOGI("JNI: Failed to acquire ANativeWindow from Surface");
        return JNI_FALSE;
    }
    bool success = sphere::vk::VulkanNativeSceneRenderer::Get().InitializeSurface(
        window, static_cast<uint32_t>(width), static_cast<uint32_t>(height)
    );
    return success ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeResizeVulkanSurface(
    JNIEnv* env, jclass clazz, jint width, jint height
) {
    sphere::vk::VulkanNativeSceneRenderer::Get().Resize(
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)
    );
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeDestroyVulkanSurface(
    JNIEnv* env, jclass clazz
) {
    LOGI("JNI: nativeDestroyVulkanSurface cleaning up Vulkan surface...");
    sphere::vk::VulkanNativeSceneRenderer::Get().DestroySurface();
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderVulkanFrame(
    JNIEnv* env, jclass clazz,
    jfloat posX, jfloat posY, jfloat posZ,
    jfloat intensity, jfloat colorR, jfloat colorG, jfloat colorB,
    jfloat roughness, jfloat metallic,
    jfloat width, jfloat height, jfloat depth,
    jint stageMode
) {
    sphere::vk::NativeRenderParams params;
    params.lightPosX = posX;
    params.lightPosY = posY;
    params.lightPosZ = posZ;
    params.lightIntensity = intensity;
    params.lightColorR = colorR;
    params.lightColorG = colorG;
    params.lightColorB = colorB;
    params.roughness = roughness;
    params.metallic = metallic;
    params.objectWidth = width;
    params.objectHeight = height;
    params.objectDepth = depth;
    params.stageMode = static_cast<uint32_t>(stageMode);

    sphere::vk::VulkanNativeSceneRenderer::Get().RenderFrame(params);
}

JNIEXPORT jboolean JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeIsVulkanInitialized(
    JNIEnv* env, jclass clazz
) {
    return sphere::vk::VulkanNativeSceneRenderer::Get().IsInitialized() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeIsVulkanGpuAvailable(
    JNIEnv* env, jclass clazz
) {
    return sphere::vk::VulkanNativeSceneRenderer::IsGpuNodeAccessible() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderVolumetricClouds(
    JNIEnv* env, jclass clazz,
    jintArray outArray, jint width, jint height,
    jfloat camYaw, jfloat camPitch,
    jfloat sunAzimuth, jfloat sunElevation,
    jint cloudType, jfloat coverage, jfloat density,
    jfloat time
) {
    if (!outArray || width <= 0 || height <= 0) {
        return JNI_FALSE;
    }

    jsize len = env->GetArrayLength(outArray);
    if (len < width * height) {
        return JNI_FALSE;
    }

    jint* pixels = static_cast<jint*>(env->GetPrimitiveArrayCritical(outArray, nullptr));
    if (!pixels) {
        return JNI_FALSE;
    }

    sphere::weather::CloudParams params;
    params.coverage = std::clamp(coverage, 0.05f, 1.0f);
    params.density = std::clamp(density, 0.1f, 3.0f);

    switch (cloudType) {
        case 0: params.type = sphere::weather::CloudType::Stratus; break;
        case 1: params.type = sphere::weather::CloudType::Stratocumulus; break;
        case 2: params.type = sphere::weather::CloudType::Cumulus; break;
        case 3: params.type = sphere::weather::CloudType::Cumulonimbus; break;
        case 4: params.type = sphere::weather::CloudType::Altocumulus; break;
        case 5: params.type = sphere::weather::CloudType::Cirrus; break;
        default: params.type = sphere::weather::CloudType::Stratocumulus; break;
    }

    sphere::weather::CloudGen generator(params);
    sphere::weather::CloudRaymarch raymarcher;

    raymarcher.RenderCloudBuffer(
        reinterpret_cast<uint32_t*>(pixels),
        width, height,
        camYaw, camPitch,
        sunAzimuth, sunElevation,
        generator, time
    );

    env->ReleasePrimitiveArrayCritical(outArray, pixels, 0);
    return JNI_TRUE;
}

// ---------------------------------------------------------------------------
// Cargo & Commodities Procedural JNI Bridge
// ---------------------------------------------------------------------------

JNIEXPORT jint JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoCommoditiesCount(
    JNIEnv* env, jclass clazz
) {
    return static_cast<jint>(sphere::cargo::CargoRegistry::Get().GetTotalCommoditiesCount());
}

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoName(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        return env->NewStringUTF(sphere::cargo::GetCargoName(static_cast<sphere::cargo::CargoID>(id)));
    }
    return env->NewStringUTF("Unknown Cargo");
}

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoCategory(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        return env->NewStringUTF(sphere::cargo::GetCargoCategory(static_cast<sphere::cargo::CargoID>(id)));
    }
    return env->NewStringUTF("Unknown Category");
}

JNIEXPORT jfloat JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoWeight(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        const auto& def = sphere::cargo::CargoRegistry::Get().GetDef(static_cast<sphere::cargo::CargoID>(id));
        return def.weight_kg;
    }
    return 50.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoVolume(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        const auto& def = sphere::cargo::CargoRegistry::Get().GetDef(static_cast<sphere::cargo::CargoID>(id));
        return def.volume_m3;
    }
    return 0.08f;
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoDimensions(
    JNIEnv* env, jclass clazz, jint id
) {
    jfloatArray result = env->NewFloatArray(3);
    if (!result) return nullptr;

    jfloat dims[3] = {0.8f, 0.5f, 0.2f};
    if (id >= 0 && id < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        const auto& def = sphere::cargo::CargoRegistry::Get().GetDef(static_cast<sphere::cargo::CargoID>(id));
        dims[0] = def.dimensions.x;
        dims[1] = def.dimensions.y;
        dims[2] = def.dimensions.z;
    }
    env->SetFloatArrayRegion(result, 0, 3, dims);
    return result;
}

JNIEXPORT jintArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRequestCargoMeshStats(
    JNIEnv* env, jclass clazz, jint id, jint lod
) {
    jintArray result = env->NewIntArray(2);
    if (!result) return nullptr;

    jint stats[2] = {0, 0};
    if (id >= 0 && id < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        std::shared_ptr<sphere::Mesh> mesh;
        {
            std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
            mesh = sphere::cargo::CargoRegistry::Get().RequestMesh(
                static_cast<sphere::cargo::CargoID>(id), static_cast<uint32_t>(std::max(0, lod))
            );
        }
        if (mesh) {
            stats[0] = static_cast<jint>(mesh->vertices.size());
            stats[1] = static_cast<jint>(mesh->indices.size() / 3);
        }
    }
    env->SetIntArrayRegion(result, 0, 2, stats);
    return result;
}

// ---------------------------------------------------------------------------
// Trailers & Haulage Procedural JNI Bridge
// ---------------------------------------------------------------------------

JNIEXPORT jint JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTrailersCount(
    JNIEnv* env, jclass clazz
) {
    return static_cast<jint>(sphere::trailers::TrailerRegistry::Get().GetTotalTrailersCount());
}

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTrailerName(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
        return env->NewStringUTF(sphere::trailers::GetTrailerName(static_cast<sphere::trailers::TrailerID>(id)));
    }
    return env->NewStringUTF("Unknown Trailer");
}

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTrailerCategory(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
        return env->NewStringUTF(sphere::trailers::GetTrailerCategory(static_cast<sphere::trailers::TrailerID>(id)));
    }
    return env->NewStringUTF("Unknown Category");
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetTrailerSpecs(
    JNIEnv* env, jclass clazz, jint id
) {
    jfloatArray result = env->NewFloatArray(6);
    if (!result) return nullptr;

    jfloat specs[6] = {6200.f, 32000.f, 13.6f, 2.55f, 1.55f, 1.35f};
    if (id >= 0 && id < static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        const auto& def = sphere::trailers::TrailerRegistry::Get().GetDef(static_cast<sphere::trailers::TrailerID>(id));
        specs[0] = def.empty_weight_kg;
        specs[1] = def.max_payload_kg;
        specs[2] = def.dimensions.x;
        specs[3] = def.dimensions.y;
        specs[4] = def.dimensions.z;
        specs[5] = def.deck_height_m;
    }
    env->SetFloatArrayRegion(result, 0, 6, specs);
    return result;
}

JNIEXPORT jintArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRequestTrailerMeshStats(
    JNIEnv* env, jclass clazz, jint id, jint lod
) {
    jintArray result = env->NewIntArray(2);
    if (!result) return nullptr;

    jint stats[2] = {0, 0};
    if (id >= 0 && id < static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
        std::shared_ptr<sphere::Mesh> mesh;
        {
            std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
            mesh = sphere::trailers::TrailerRegistry::Get().RequestMesh(
                static_cast<sphere::trailers::TrailerID>(id), static_cast<uint32_t>(std::max(0, lod))
            );
        }
        if (mesh) {
            stats[0] = static_cast<jint>(mesh->vertices.size());
            stats[1] = static_cast<jint>(mesh->indices.size() / 3);
        }
    }
    env->SetIntArrayRegion(result, 0, 2, stats);
    return result;
}

JNIEXPORT jintArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderCargoBitmap(
    JNIEnv* env, jclass clazz,
    jobject bitmap,
    jint cargoId,
    jint stackingMode,
    jfloat rotX,
    jfloat rotY,
    jfloat zoom,
    jboolean showPallet,
    jboolean showStraps,
    jboolean showShrinkwrap,
    jint primaryArgb,
    jint secondaryArgb,
    jfloat roughness,
    jfloat metallic,
    jfloat wear,
    jfloat wrinkle,
    jfloat opacity,
    jboolean lowDetail
) {
    jintArray result = env->NewIntArray(2);
    if (!result) return nullptr;
    jint stats[2] = {0, 0};

    if (!bitmap || cargoId < 0 || cargoId >= static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
        env->SetIntArrayRegion(result, 0, 2, stats);
        return result;
    }

    AndroidBitmapInfo info{};
    if (AndroidBitmap_getInfo(env, bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS ||
        info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 ||
        info.width == 0 || info.height == 0 ||
        info.stride < info.width * 4u) {
        env->SetIntArrayRegion(result, 0, 2, stats);
        return result;
    }

    const float safeRotX = FiniteOr(rotX, 18.0f);
    const float safeRotY = FiniteOr(rotY, -35.0f);
    const float safeZoom = std::clamp(FiniteOr(zoom, 1.0f), 0.25f, 4.0f);
    const float effRough = std::clamp(FiniteOr(roughness, 0.65f), 0.04f, 1.0f);
    const float effMetal = std::clamp(FiniteOr(metallic, 0.0f), 0.0f, 1.0f);
    const float effWear = std::clamp(FiniteOr(wear, 0.0f), 0.0f, 1.0f);
    const float effWrinkle = std::clamp(FiniteOr(wrinkle, 0.0f), 0.0f, 1.0f);
    const float rawOpacity = FiniteOr(opacity, 1.0f);
    const float effOpacity = (rawOpacity >= 0.99f) ? 1.0f : std::clamp(rawOpacity, 0.15f, 1.0f);

    std::shared_ptr<sphere::Mesh> cargoMesh;
    std::shared_ptr<sphere::Mesh> palletMesh;
    {
        static std::unordered_map<int32_t, uint32_t> sLastLod;
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);

        auto cid = static_cast<sphere::cargo::CargoID>(cargoId);
        auto cargo = sphere::cargo::CargoRegistry::Get().GetCargo(cid);
        if (!cargo) {
            env->SetIntArrayRegion(result, 0, 2, stats);
            return result;
        }

        sphere::cargo::CargoDef nextDef = cargo->GetDef();
        sphere::Vec4 pCol = UnpackArgbToVec4(primaryArgb, 1.0f);
        sphere::Vec4 sCol = UnpackArgbToVec4(secondaryArgb, 1.0f);

        bool defChanged =
            !Vec4Near(nextDef.base_color, pCol) ||
            !Vec4Near(nextDef.label_color, sCol) ||
            !FloatNear(nextDef.roughness, effRough) ||
            !FloatNear(nextDef.metallic, effMetal) ||
            !FloatNear(nextDef.wear_and_tear, effWear) ||
            !FloatNear(nextDef.wrinkle_or_corrugation, effWrinkle) ||
            !FloatNear(nextDef.opacity_bias, effOpacity);

        if (defChanged) {
            nextDef.base_color = pCol;
            nextDef.label_color = sCol;
            nextDef.roughness = effRough;
            nextDef.metallic = effMetal;
            nextDef.wear_and_tear = effWear;
            nextDef.wrinkle_or_corrugation = effWrinkle;
            nextDef.opacity_bias = effOpacity;
            sphere::cargo::CargoRegistry::Get().SetDef(cid, nextDef);
        }

        uint32_t lod = lowDetail ? 1u : 0u;
        auto cached = cargo->GetCachedMesh();
        auto lodIt = sLastLod.find(cargoId);
        if (!cached || lodIt == sLastLod.end() || lodIt->second != lod) {
            cargoMesh = cargo->GenerateMesh(lod);
            sLastLod[cargoId] = lod;
        } else {
            cargoMesh = cached;
        }

        if ((showPallet || stackingMode > 0) && cid != sphere::cargo::CargoID::PALLET) {
            palletMesh = sphere::cargo::CargoRegistry::Get().RequestMesh(sphere::cargo::CargoID::PALLET, 0);
        }
    }

    if (cargoMesh) {
        stats[0] = static_cast<jint>(cargoMesh->vertices.size());
        stats[1] = static_cast<jint>(cargoMesh->indices.size() / 3);
    }

    void* pixelsPtr = nullptr;
    if (cargoMesh && AndroidBitmap_lockPixels(env, bitmap, &pixelsPtr) == ANDROID_BITMAP_RESULT_SUCCESS && pixelsPtr) {
        sphere::scene::CargoRasterParams rParams;
        rParams.cargoId = cargoId;
        rParams.stackingMode = stackingMode;
        rParams.rotXDeg = safeRotX;
        rParams.rotYDeg = safeRotY;
        rParams.zoom = safeZoom;
        rParams.showPallet = (showPallet == JNI_TRUE);
        rParams.showStraps = (showStraps == JNI_TRUE);
        rParams.showShrinkwrap = (showShrinkwrap == JNI_TRUE);
        rParams.roughness = effRough;
        rParams.metallic = effMetal;

        sphere::scene::NativeMeshRasterizer::RenderCargoBitmap(
            static_cast<uint32_t*>(pixelsPtr),
            static_cast<int32_t>(info.stride / 4),
            static_cast<int32_t>(info.width),
            static_cast<int32_t>(info.height),
            cargoMesh.get(),
            palletMesh.get(),
            rParams
        );
        AndroidBitmap_unlockPixels(env, bitmap);
    }

    env->SetIntArrayRegion(result, 0, 2, stats);
    return result;
}

JNIEXPORT jintArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderTrailerBitmap(
    JNIEnv* env, jclass clazz,
    jobject bitmap,
    jint trailerId,
    jfloat rotX,
    jfloat rotY,
    jfloat zoom,
    jboolean landingGearExtended,
    jboolean showLoadedCargo,
    jint cargoId,
    jint cargoQty,
    jfloat curtainOpenRatio,
    jfloat hydraulicLiftRatio,
    jfloat curtainOpacity,
    jint baseArgb,
    jint secondaryArgb,
    jfloat roughness,
    jfloat metallic,
    jint axleCount,
    jint suspensionOrdinal,
    jint brakeOrdinal,
    jboolean hasLiftAxle,
    jboolean liftAxleRaised,
    jboolean hasSteerableAxle,
    jfloat steerAngleDeg
) {
    jintArray result = env->NewIntArray(2);
    if (!result) return nullptr;
    jint stats[2] = {0, 0};

    if (!bitmap || trailerId < 0 || trailerId >= static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
        env->SetIntArrayRegion(result, 0, 2, stats);
        return result;
    }

    AndroidBitmapInfo info{};
    if (AndroidBitmap_getInfo(env, bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS ||
        info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 ||
        info.width == 0 || info.height == 0 ||
        info.stride < info.width * 4u) {
        env->SetIntArrayRegion(result, 0, 2, stats);
        return result;
    }

    const float safeRotX = FiniteOr(rotX, 16.0f);
    const float safeRotY = FiniteOr(rotY, -35.0f);
    const float safeZoom = std::clamp(FiniteOr(zoom, 1.0f), 0.25f, 4.0f);
    const float effRough = std::clamp(FiniteOr(roughness, 0.55f), 0.04f, 1.0f);
    const float effMetal = std::clamp(FiniteOr(metallic, 0.15f), 0.0f, 1.0f);
    const float effCurtainOpen = std::clamp(FiniteOr(curtainOpenRatio, 0.0f), 0.0f, 1.0f);
    const float effHydLift = std::clamp(FiniteOr(hydraulicLiftRatio, 0.0f), 0.0f, 1.0f);
    const float rawOpacity = FiniteOr(curtainOpacity, 1.0f);
    const float effOpacity = (rawOpacity >= 0.99f) ? 1.0f : std::clamp(rawOpacity, 0.15f, 1.0f);
    const float effSteerDeg = std::clamp(FiniteOr(steerAngleDeg, 0.0f), -35.0f, 35.0f);

    std::shared_ptr<sphere::Mesh> trailerMesh;
    std::shared_ptr<sphere::Mesh> loadedCargoMesh;
    float floorY = 1.25f;
    float deckL = 13.6f;
    float deckW = 2.55f;

    {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);

        auto tid = static_cast<sphere::trailers::TrailerID>(trailerId);
        auto trailer = sphere::trailers::TrailerRegistry::Get().GetTrailer(tid);
        if (!trailer) {
            env->SetIntArrayRegion(result, 0, 2, stats);
            return result;
        }

        sphere::trailers::TrailerDef nextDef = trailer->GetDef();
        sphere::Vec4 pCol = UnpackArgbToVec4(baseArgb, 1.0f);
        sphere::Vec4 sCol = UnpackArgbToVec4(secondaryArgb, 1.0f);
        uint32_t effAxles = static_cast<uint32_t>(std::max(1, axleCount));
        auto effSusp = static_cast<sphere::trailers::SuspensionType>(std::clamp(suspensionOrdinal, 0, 2));
        auto effBrake = static_cast<sphere::trailers::BrakeType>(std::clamp(brakeOrdinal, 0, 1));
        bool effLanding = (landingGearExtended == JNI_TRUE);
        bool effHasLift = (hasLiftAxle == JNI_TRUE);
        bool effLiftRaised = (liftAxleRaised == JNI_TRUE);
        bool effHasSteer = (hasSteerableAxle == JNI_TRUE);

        bool defChanged =
            !Vec4Near(nextDef.base_color, pCol) ||
            !Vec4Near(nextDef.accent_color, sCol) ||
            !FloatNear(nextDef.opacity_bias, effOpacity) ||
            !FloatNear(nextDef.roughness, effRough) ||
            !FloatNear(nextDef.metallic, effMetal) ||
            !FloatNear(nextDef.curtain_open_ratio, effCurtainOpen) ||
            !FloatNear(nextDef.hydraulic_lift_ratio, effHydLift) ||
            !FloatNear(nextDef.steer_angle_deg, effSteerDeg) ||
            nextDef.axle_count != effAxles ||
            nextDef.suspension_type != effSusp ||
            nextDef.brake_type != effBrake ||
            nextDef.landing_gear_extended != effLanding ||
            nextDef.has_lift_axle != effHasLift ||
            nextDef.lift_axle_raised != effLiftRaised ||
            nextDef.has_steerable_axle != effHasSteer;

        if (defChanged) {
            nextDef.base_color = pCol;
            nextDef.accent_color = sCol;
            nextDef.opacity_bias = effOpacity;
            nextDef.roughness = effRough;
            nextDef.metallic = effMetal;
            nextDef.curtain_open_ratio = effCurtainOpen;
            nextDef.hydraulic_lift_ratio = effHydLift;
            nextDef.steer_angle_deg = effSteerDeg;
            nextDef.axle_count = effAxles;
            nextDef.suspension_type = effSusp;
            nextDef.brake_type = effBrake;
            nextDef.landing_gear_extended = effLanding;
            nextDef.has_lift_axle = effHasLift;
            nextDef.lift_axle_raised = effLiftRaised;
            nextDef.has_steerable_axle = effHasSteer;
            sphere::trailers::TrailerRegistry::Get().SetDef(tid, nextDef);
        }

        floorY = nextDef.deck_height_m;
        deckL = nextDef.dimensions.x;
        deckW = nextDef.dimensions.y;

        trailerMesh = trailer->GetCachedMesh();
        if (!trailerMesh) {
            trailerMesh = trailer->GenerateMesh(0);
        }

        if (showLoadedCargo && cargoQty > 0 &&
            cargoId >= 0 && cargoId < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
            loadedCargoMesh = sphere::cargo::CargoRegistry::Get().RequestMesh(
                static_cast<sphere::cargo::CargoID>(cargoId), 1
            );
        }
    }

    if (trailerMesh) {
        stats[0] = static_cast<jint>(trailerMesh->vertices.size());
        stats[1] = static_cast<jint>(trailerMesh->indices.size() / 3);
    }

    void* pixelsPtr = nullptr;
    if (trailerMesh && AndroidBitmap_lockPixels(env, bitmap, &pixelsPtr) == ANDROID_BITMAP_RESULT_SUCCESS && pixelsPtr) {
        sphere::scene::TrailerRasterParams rParams;
        rParams.trailerId = trailerId;
        rParams.rotXDeg = safeRotX;
        rParams.rotYDeg = safeRotY;
        rParams.zoom = safeZoom;
        rParams.roughness = effRough;
        rParams.metallic = effMetal;
        rParams.cargoFloorY = floorY;
        rParams.deckLengthM = deckL;
        rParams.deckWidthM = deckW;
        rParams.showLoadedCargo = (showLoadedCargo == JNI_TRUE);
        rParams.cargoQuantity = cargoQty;

        sphere::scene::NativeMeshRasterizer::RenderTrailerBitmap(
            static_cast<uint32_t*>(pixelsPtr),
            static_cast<int32_t>(info.stride / 4),
            static_cast<int32_t>(info.width),
            static_cast<int32_t>(info.height),
            trailerMesh.get(),
            loadedCargoMesh.get(),
            rParams
        );
        AndroidBitmap_unlockPixels(env, bitmap);
    }

    env->SetIntArrayRegion(result, 0, 2, stats);
    return result;
}

// ---------------------------------------------------------------------------
// Vehicles & Transports Procedural JNI Bridge (Lorries & Buses)
// ---------------------------------------------------------------------------

JNIEXPORT jint JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetVehiclesCount(
    JNIEnv* env, jclass clazz
) {
    return static_cast<jint>(sphere::vehicles::VehicleRegistry::Get().GetTotalVehiclesCount());
}

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetVehicleName(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::vehicles::VehicleID::COUNT)) {
        return env->NewStringUTF(sphere::vehicles::GetVehicleName(static_cast<sphere::vehicles::VehicleID>(id)));
    }
    return env->NewStringUTF("Unknown Vehicle");
}

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetVehicleCategory(
    JNIEnv* env, jclass clazz, jint id
) {
    if (id >= 0 && id < static_cast<jint>(sphere::vehicles::VehicleID::COUNT)) {
        return env->NewStringUTF(sphere::vehicles::GetVehicleCategory(static_cast<sphere::vehicles::VehicleID>(id)));
    }
    return env->NewStringUTF("Unknown Category");
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetVehicleSpecs(
    JNIEnv* env, jclass clazz, jint id
) {
    jfloatArray result = env->NewFloatArray(10);
    if (!result) return nullptr;

    jfloat specs[10] = {7250.f, 18000.f, 4.8f, 0.f, 1150.f, 290.f, 2.f, 8.2f, 2.5f, 3.45f};
    if (id >= 0 && id < static_cast<jint>(sphere::vehicles::VehicleID::COUNT)) {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        const auto& def = sphere::vehicles::VehicleRegistry::Get().GetDef(static_cast<sphere::vehicles::VehicleID>(id));
        specs[0] = def.kerb_weight_kg;
        specs[1] = def.max_gvw_kg;
        specs[2] = def.wheelbase_m;
        specs[3] = def.fifth_wheel_offset_m;
        specs[4] = def.engine_torque_nm;
        specs[5] = def.engine_power_hp;
        specs[6] = static_cast<float>(def.passenger_capacity);
        specs[7] = def.dimensions.x;
        specs[8] = def.dimensions.y;
        specs[9] = def.dimensions.z;
    }
    env->SetFloatArrayRegion(result, 0, 10, specs);
    return result;
}

JNIEXPORT jintArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderVehicleBitmap(
    JNIEnv* env, jclass clazz,
    jobject bitmap,
    jint vehicleId,
    jfloat rotX,
    jfloat rotY,
    jfloat zoom,
    jint liveryOrdinal,
    jint randomSeed,
    jint baseArgb,
    jint accentArgb,
    jfloat roughness,
    jfloat metallic,
    jfloat wheelbaseM,
    jfloat fifthWheelOffsetM,
    jfloat actionRatio,
    jfloat doorOpenRatio,
    jfloat steerAngleDeg,
    jboolean headlightsOn,
    jboolean hasRoofRack,
    jboolean hasExhaustStacks,
    jboolean hitchTrailer,
    jint trailerId,
    jboolean showLoadedCargo,
    jint cargoId,
    jint cargoQty,
    jboolean isRhd,
    jboolean insideCabinView
) {
    jintArray result = env->NewIntArray(4);
    if (!result) return nullptr;
    jint stats[4] = {0, 0, 0, 0}; // [vehicleVtx, vehicleTris, totalVtx, totalTris]

    if (!bitmap || vehicleId < 0 || vehicleId >= static_cast<jint>(sphere::vehicles::VehicleID::COUNT)) {
        env->SetIntArrayRegion(result, 0, 4, stats);
        return result;
    }

    AndroidBitmapInfo info{};
    if (AndroidBitmap_getInfo(env, bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS ||
        info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 ||
        info.width == 0 || info.height == 0 ||
        info.stride < info.width * 4u) {
        env->SetIntArrayRegion(result, 0, 4, stats);
        return result;
    }

    const float safeRotX = FiniteOr(rotX, 16.0f);
    const float safeRotY = FiniteOr(rotY, -35.0f);
    const float safeZoom = std::clamp(FiniteOr(zoom, 1.0f), 0.25f, 4.0f);
    const float effRough = std::clamp(FiniteOr(roughness, 0.35f), 0.04f, 1.0f);
    const float effMetal = std::clamp(FiniteOr(metallic, 0.35f), 0.0f, 1.0f);
    const float effAction = std::clamp(FiniteOr(actionRatio, 0.0f), 0.0f, 1.0f);
    const float effDoorOpen = std::clamp(FiniteOr(doorOpenRatio, 0.0f), 0.0f, 1.0f);
    const float effSteer = std::clamp(FiniteOr(steerAngleDeg, 0.0f), -30.0f, 30.0f);

    std::shared_ptr<sphere::Mesh> vehicleMesh;
    std::shared_ptr<sphere::Mesh> hitchedTrailerMesh;
    std::shared_ptr<sphere::Mesh> loadedCargoMesh;
    float effectiveFifthWheel = 0.0f;
    float trailerLengthM = 13.6f;
    float trailerWidthM = 2.55f;
    float trailerKingpinOffsetM = 1.65f;
    float trailerDeckY = 1.35f;
    bool effRhd = (isRhd == JNI_TRUE);
    bool effInsideCabin = (insideCabinView == JNI_TRUE);

    {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);

        auto vid = static_cast<sphere::vehicles::VehicleID>(vehicleId);
        auto vehicle = sphere::vehicles::VehicleRegistry::Get().GetVehicle(vid);
        if (!vehicle) {
            env->SetIntArrayRegion(result, 0, 4, stats);
            return result;
        }

        sphere::vehicles::VehicleDef nextDef = vehicle->GetDef();
        const float effWheelbase = std::clamp(FiniteOr(wheelbaseM, nextDef.wheelbase_m), 1.5f, 12.0f);
        const float effFifthWheel = std::clamp(FiniteOr(fifthWheelOffsetM, nextDef.fifth_wheel_offset_m), -2.0f, 6.0f);
        sphere::Vec4 pCol = UnpackArgbToVec4(baseArgb, 1.0f);
        sphere::Vec4 sCol = UnpackArgbToVec4(accentArgb, 1.0f);
        auto effLivery = static_cast<sphere::vehicles::LiveryID>(std::clamp(liveryOrdinal, 0, 5));
        uint32_t effSeed = static_cast<uint32_t>(randomSeed == 0 ? 0x5A1C0001 : randomSeed);
        bool effHeadlights = (headlightsOn == JNI_TRUE);
        bool effRoofRack = (hasRoofRack == JNI_TRUE);
        bool effExhaust = (hasExhaustStacks == JNI_TRUE);

        bool defChanged =
            nextDef.livery != effLivery ||
            nextDef.seed != effSeed ||
            !Vec4Near(nextDef.base_color, pCol) ||
            !Vec4Near(nextDef.accent_color, sCol) ||
            !FloatNear(nextDef.roughness, effRough) ||
            !FloatNear(nextDef.metallic, effMetal) ||
            !FloatNear(nextDef.wheelbase_m, effWheelbase) ||
            !FloatNear(nextDef.fifth_wheel_offset_m, effFifthWheel) ||
            !FloatNear(nextDef.action_ratio, effAction) ||
            !FloatNear(nextDef.door_open_ratio, effDoorOpen) ||
            !FloatNear(nextDef.steer_angle_deg, effSteer) ||
            nextDef.headlights_on != effHeadlights ||
            nextDef.has_roof_rack != effRoofRack ||
            nextDef.has_exhaust_stacks != effExhaust ||
            nextDef.is_rhd != effRhd;

        if (defChanged) {
            nextDef.livery = effLivery;
            nextDef.seed = effSeed;
            nextDef.base_color = pCol;
            nextDef.accent_color = sCol;
            nextDef.roughness = effRough;
            nextDef.metallic = effMetal;
            nextDef.wheelbase_m = effWheelbase;
            nextDef.fifth_wheel_offset_m = effFifthWheel;
            nextDef.action_ratio = effAction;
            nextDef.door_open_ratio = effDoorOpen;
            nextDef.steer_angle_deg = effSteer;
            nextDef.headlights_on = effHeadlights;
            nextDef.has_roof_rack = effRoofRack;
            nextDef.has_exhaust_stacks = effExhaust;
            nextDef.is_rhd = effRhd;
            sphere::vehicles::VehicleRegistry::Get().SetDef(vid, nextDef);
        }

        effectiveFifthWheel = nextDef.fifth_wheel_offset_m;
        vehicleMesh = vehicle->GetCachedMesh();
        if (!vehicleMesh) {
            vehicleMesh = vehicle->GenerateMesh(0);
        }

        if (hitchTrailer == JNI_TRUE && effectiveFifthWheel > 0.1f &&
            trailerId >= 0 && trailerId < static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
            auto tid = static_cast<sphere::trailers::TrailerID>(trailerId);
            auto trailer = sphere::trailers::TrailerRegistry::Get().GetTrailer(tid);
            if (trailer) {
                sphere::trailers::TrailerDef tDef = trailer->GetDef();
                if (tDef.landing_gear_extended) {
                    tDef.landing_gear_extended = false;
                    sphere::trailers::TrailerRegistry::Get().SetDef(tid, tDef);
                }
                trailerLengthM = tDef.dimensions.x;
                trailerWidthM = tDef.dimensions.y;
                trailerKingpinOffsetM = tDef.kingpin_offset_m;
                trailerDeckY = tDef.deck_height_m;
                hitchedTrailerMesh = trailer->GetCachedMesh();
                if (!hitchedTrailerMesh) {
                    hitchedTrailerMesh = trailer->GenerateMesh(0);
                }

                if (showLoadedCargo == JNI_TRUE && cargoQty > 0 &&
                    cargoId >= 0 && cargoId < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
                    loadedCargoMesh = sphere::cargo::CargoRegistry::Get().RequestMesh(
                        static_cast<sphere::cargo::CargoID>(cargoId), 1
                    );
                }
            }
        }
    }

    if (vehicleMesh) {
        stats[0] = static_cast<jint>(vehicleMesh->vertices.size());
        stats[1] = static_cast<jint>(vehicleMesh->indices.size() / 3);
        stats[2] = stats[0];
        stats[3] = stats[1];
    }
    if (hitchedTrailerMesh) {
        stats[2] += static_cast<jint>(hitchedTrailerMesh->vertices.size());
        stats[3] += static_cast<jint>(hitchedTrailerMesh->indices.size() / 3);
    }

    void* pixelsPtr = nullptr;
    if (vehicleMesh && AndroidBitmap_lockPixels(env, bitmap, &pixelsPtr) == ANDROID_BITMAP_RESULT_SUCCESS && pixelsPtr) {
        sphere::scene::VehicleRasterParams rParams;
        rParams.vehicleId = vehicleId;
        rParams.rotXDeg = safeRotX;
        rParams.rotYDeg = safeRotY;
        rParams.zoom = safeZoom;
        rParams.roughness = effRough;
        rParams.metallic = effMetal;
        rParams.fifthWheelOffsetM = effectiveFifthWheel;
        rParams.showHitchedTrailer = (hitchTrailer == JNI_TRUE && hitchedTrailerMesh != nullptr);
        rParams.trailerLengthM = trailerLengthM;
        rParams.trailerWidthM = trailerWidthM;
        rParams.trailerKingpinOffsetM = trailerKingpinOffsetM;
        rParams.trailerDeckY = trailerDeckY;
        rParams.showLoadedCargo = (showLoadedCargo == JNI_TRUE);
        rParams.cargoQuantity = cargoQty;
        rParams.insideCabinView = effInsideCabin;
        rParams.isRhd = effRhd;

        sphere::scene::NativeMeshRasterizer::RenderVehicleBitmap(
            static_cast<uint32_t*>(pixelsPtr),
            static_cast<int32_t>(info.stride / 4),
            static_cast<int32_t>(info.width),
            static_cast<int32_t>(info.height),
            vehicleMesh.get(),
            hitchedTrailerMesh.get(),
            loadedCargoMesh.get(),
            rParams
        );
        AndroidBitmap_unlockPixels(env, bitmap);
    }

    env->SetIntArrayRegion(result, 0, 4, stats);
    return result;
}

// ---------------------------------------------------------------------------
// Hybrid Procedural + RAM PCM Vehicle Audio & Spatial Meshlet Streaming Bridge
// ---------------------------------------------------------------------------

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeConfigureVehicleAudio(
    JNIEnv* env, jclass clazz,
    jint categoryOrdinal,
    jfloat powerHp,
    jfloat torqueNm,
    jfloat kerbKg,
    jfloat combinedLoadKg
) {
    sphere::audio::EngineAudioSystem::Get().ConfigureVehicle(
        static_cast<int32_t>(categoryOrdinal),
        powerHp,
        torqueNm,
        kerbKg,
        combinedLoadKg
    );
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeSetVehicleIgnition(
    JNIEnv* env, jclass clazz,
    jboolean turnOn
) {
    sphere::audio::EngineAudioSystem::Get().SetIgnition(turnOn == JNI_TRUE);
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeSetVehicleAudioControls(
    JNIEnv* env, jclass clazz,
    jfloat throttle,
    jfloat brake,
    jboolean cruiseDrive,
    jboolean muted
) {
    sphere::audio::EngineAudioSystem::Get().SetControls(
        throttle,
        brake,
        cruiseDrive == JNI_TRUE,
        muted == JNI_TRUE
    );
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeTriggerVehicleOneShot(
    JNIEnv* env, jclass clazz,
    jint oneShotId,
    jfloat gain
) {
    auto id = static_cast<sphere::audio::OneShotSoundId>(std::clamp(oneShotId, 0, 4));
    sphere::audio::EngineAudioSystem::Get().TriggerOneShot(id, gain);
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderVehicleAudioBlock(
    JNIEnv* env, jclass clazz,
    jshortArray outPcmArray,
    jint sampleCount
) {
    jfloatArray telemetryOut = env->NewFloatArray(7);
    if (!telemetryOut) return nullptr;

    jint clampedCount = std::max<jint>(0, sampleCount);
    if (outPcmArray && clampedCount > 0) {
        jsize arrLen = env->GetArrayLength(outPcmArray);
        jint writeCount = std::min<jint>(clampedCount, arrLen);
        if (writeCount > 0) {
            std::vector<int16_t> pcmBuf(static_cast<size_t>(writeCount), 0);
            sphere::audio::EngineAudioSystem::Get().RenderAudioBlock(pcmBuf.data(), writeCount);
            env->SetShortArrayRegion(outPcmArray, 0, writeCount, pcmBuf.data());
        }
    }

    auto t = sphere::audio::EngineAudioSystem::Get().GetTelemetry();
    jfloat vals[7] = {
        static_cast<jfloat>(t.ignition_state),
        t.rpm,
        t.speed_kmh,
        static_cast<jfloat>(t.gear),
        t.turbo_boost_bar,
        t.distance_m,
        static_cast<jfloat>(t.has_active_sound)
    };
    env->SetFloatArrayRegion(telemetryOut, 0, 7, vals);
    return telemetryOut;
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetVehicleAudioTelemetry(
    JNIEnv* env, jclass clazz
) {
    jfloatArray telemetryOut = env->NewFloatArray(7);
    if (!telemetryOut) return nullptr;
    auto t = sphere::audio::EngineAudioSystem::Get().GetTelemetry();
    jfloat vals[7] = {
        static_cast<jfloat>(t.ignition_state),
        t.rpm,
        t.speed_kmh,
        static_cast<jfloat>(t.gear),
        t.turbo_boost_bar,
        t.distance_m,
        static_cast<jfloat>(t.has_active_sound)
    };
    env->SetFloatArrayRegion(telemetryOut, 0, 7, vals);
    return telemetryOut;
}

JNIEXPORT jintArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetVehicleStreamingStats(
    JNIEnv* env, jclass clazz,
    jint vehicleId,
    jboolean hitchTrailer,
    jint trailerId,
    jboolean showLoadedCargo,
    jint cargoId,
    jint cargoQty,
    jfloat distanceDrivenM
) {
    jintArray outStats = env->NewIntArray(7);
    if (!outStats) return nullptr;

    std::shared_ptr<sphere::Mesh> vehicleMesh;
    std::shared_ptr<sphere::Mesh> trailerMesh;
    std::shared_ptr<sphere::Mesh> cargoMesh;

    if (vehicleId >= 0 && vehicleId < static_cast<jint>(sphere::vehicles::VehicleID::COUNT)) {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        auto vid = static_cast<sphere::vehicles::VehicleID>(vehicleId);
        auto v = sphere::vehicles::VehicleRegistry::Get().GetVehicle(vid);
        if (v) {
            vehicleMesh = v->GetCachedMesh();
            if (!vehicleMesh) vehicleMesh = v->GenerateMesh(0);
            if (hitchTrailer == JNI_TRUE && v->GetDef().fifth_wheel_offset_m > 0.1f &&
                trailerId >= 0 && trailerId < static_cast<jint>(sphere::trailers::TrailerID::COUNT)) {
                trailerMesh = sphere::trailers::TrailerRegistry::Get().RequestMesh(
                    static_cast<sphere::trailers::TrailerID>(trailerId), 0
                );
                if (showLoadedCargo == JNI_TRUE && cargoQty > 0 &&
                    cargoId >= 0 && cargoId < static_cast<jint>(sphere::cargo::CargoID::COUNT)) {
                    cargoMesh = sphere::cargo::CargoRegistry::Get().RequestMesh(
                        static_cast<sphere::cargo::CargoID>(cargoId), 1
                    );
                }
            }
        }
    }

    auto st = sphere::scene::WorldStreamingMeshlets::EvaluateTelemetry(
        vehicleMesh.get(),
        trailerMesh.get(),
        cargoMesh.get(),
        cargoQty,
        FiniteOr(distanceDrivenM, 0.0f)
    );

    jint vals[7] = {
        st.grid_cell_x,
        st.grid_cell_z,
        st.loaded_ring_cells,
        st.hero_meshlet_count,
        st.max_tris_per_meshlet,
        st.hlod_proxy_batches,
        st.hlod_culled_tris
    };
    env->SetIntArrayRegion(outStats, 0, 7, vals);
    return outStats;
}

// ---------------------------------------------------------------------------
// Hybrid Procedural + RAM PCM Weather Audio Bridge
// ---------------------------------------------------------------------------

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeConfigureWeatherAudio(
    JNIEnv* env, jclass clazz,
    jfloat rainIntensity,
    jfloat snowIntensity,
    jfloat windSpeedKmh,
    jfloat windAzimuthDeg,
    jfloat fogDensity,
    jboolean stormActive,
    jboolean enabled,
    jboolean muted
) {
    sphere::audio::WeatherAudioSystem::Get().SetWeatherParams(
        rainIntensity,
        snowIntensity,
        windSpeedKmh,
        windAzimuthDeg,
        fogDensity,
        stormActive == JNI_TRUE,
        enabled == JNI_TRUE,
        muted == JNI_TRUE
    );
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeTriggerWeatherThunder(
    JNIEnv* env, jclass clazz,
    jboolean closeStrike,
    jfloat gain
) {
    sphere::audio::WeatherAudioSystem::Get().TriggerThunder(closeStrike == JNI_TRUE, gain);
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderWeatherAudioBlock(
    JNIEnv* env, jclass clazz,
    jshortArray outPcmArray,
    jint sampleCount
) {
    jfloatArray telemetryOut = env->NewFloatArray(5);
    if (!telemetryOut) return nullptr;

    jint clampedCount = std::max<jint>(0, sampleCount);
    if (outPcmArray && clampedCount > 0) {
        jsize arrLen = env->GetArrayLength(outPcmArray);
        jint writeCount = std::min<jint>(clampedCount, arrLen);
        if (writeCount > 0) {
            std::vector<int16_t> pcmBuf(static_cast<size_t>(writeCount), 0);
            sphere::audio::WeatherAudioSystem::Get().RenderAudioBlock(pcmBuf.data(), writeCount);
            env->SetShortArrayRegion(outPcmArray, 0, writeCount, pcmBuf.data());
        }
    }

    auto t = sphere::audio::WeatherAudioSystem::Get().GetTelemetry();
    jfloat vals[5] = {
        static_cast<jfloat>(t.active),
        t.rain_level,
        t.wind_level,
        t.thunder_level,
        static_cast<jfloat>(t.has_active_sound)
    };
    env->SetFloatArrayRegion(telemetryOut, 0, 5, vals);
    return telemetryOut;
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetWeatherAudioTelemetry(
    JNIEnv* env, jclass clazz
) {
    jfloatArray telemetryOut = env->NewFloatArray(5);
    if (!telemetryOut) return nullptr;
    auto t = sphere::audio::WeatherAudioSystem::Get().GetTelemetry();
    jfloat vals[5] = {
        static_cast<jfloat>(t.active),
        t.rain_level,
        t.wind_level,
        t.thunder_level,
        static_cast<jfloat>(t.has_active_sound)
    };
    env->SetFloatArrayRegion(telemetryOut, 0, 5, vals);
    return telemetryOut;
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeUploadCustomAudioSample(
    JNIEnv* env, jclass clazz,
    jint slotId,
    jshortArray pcm16Array,
    jint sampleRate
) {
    (void)clazz;
    if (!pcm16Array || sampleRate <= 0) return;
    jsize len = env->GetArrayLength(pcm16Array);
    if (len <= 0) return;
    jshort* elems = env->GetShortArrayElements(pcm16Array, nullptr);
    if (!elems) return;

    const int16_t* pcmPtr = reinterpret_cast<const int16_t*>(elems);
    if (slotId >= 0 && slotId < 10) {
        sphere::audio::EngineAudioSystem::Get().UploadCustomSample(
            static_cast<int32_t>(slotId),
            pcmPtr,
            static_cast<int32_t>(len),
            static_cast<int32_t>(sampleRate)
        );
    } else if (slotId >= 10) {
        sphere::audio::WeatherAudioSystem::Get().UploadCustomSample(
            static_cast<int32_t>(slotId),
            pcmPtr,
            static_cast<int32_t>(len),
            static_cast<int32_t>(sampleRate)
        );
    }
    env->ReleaseShortArrayElements(pcm16Array, elems, JNI_ABORT);
}

// ---------------------------------------------------------------------------
// Vulkan SPH & Screen-Space Fluid Rendering (gen/water) JNI Bridge
// ---------------------------------------------------------------------------

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeConfigureWaterSimulation(
    JNIEnv* env, jclass clazz,
    jint particleCount,
    jint scenarioOrdinal,
    jfloat smoothingRadius,
    jfloat targetDensity,
    jfloat pressureMultiplier,
    jfloat nearPressureMultiplier,
    jfloat viscosityStrength,
    jfloat surfaceTension,
    jfloat collisionDamping,
    jfloat gravityX,
    jfloat gravityY,
    jfloat gravityZ,
    jfloat windX,
    jfloat windZ,
    jfloat timeScale,
    jint substeps,
    jfloat boundsWidth,
    jfloat boundsHeight,
    jfloat boundsDepth,
    jboolean waveMakerEnabled,
    jfloat waveFrequency,
    jfloat waveAmplitude,
    jboolean obstacleEnabled,
    jfloat obstacleX,
    jfloat obstacleY,
    jfloat obstacleZ,
    jfloat obstacleRadius,
    jint debugModeOrdinal,
    jfloat particleVisualRadius,
    jboolean bilateralBlurEnabled,
    jint blurRadiusPx,
    jfloat blurSpatialSigma,
    jfloat blurDepthSigma,
    jfloat waterR,
    jfloat waterG,
    jfloat waterB,
    jfloat absorbR,
    jfloat absorbG,
    jfloat absorbB,
    jfloat absorbStrength,
    jfloat indexOfRefraction,
    jfloat refractionStrength,
    jfloat fresnelPower,
    jfloat fresnelBias,
    jfloat specularPower,
    jfloat reflectionIntensity,
    jfloat foamThreshold,
    jfloat causticsIntensity,
    jfloat sunAzimuthDeg,
    jfloat sunElevationDeg,
    jboolean showContainerBox,
    jboolean showFloorGrid,
    jfloat camYawDeg,
    jfloat camPitchDeg,
    jfloat camDistance
) {
    (void)env;
    (void)clazz;
    sphere::water::FluidPhysicsParams phys{};
    phys.particleCount = static_cast<uint32_t>(std::clamp<jint>(particleCount, 128, 4096));
    phys.scenario = static_cast<sphere::water::WaterSpawnScenario>(std::clamp<jint>(scenarioOrdinal, 0, 5));
    phys.smoothingRadius = std::clamp(smoothingRadius, 0.12f, 0.70f);
    phys.targetDensity = std::clamp(targetDensity, 1.0f, 30.0f);
    phys.pressureMultiplier = std::clamp(pressureMultiplier, 1.0f, 150.0f);
    phys.nearPressureMultiplier = std::clamp(nearPressureMultiplier, 0.0f, 80.0f);
    phys.viscosityStrength = std::clamp(viscosityStrength, 0.0f, 2.0f);
    phys.surfaceTension = std::clamp(surfaceTension, 0.0f, 2.5f);
    phys.collisionDamping = std::clamp(collisionDamping, 0.05f, 0.99f);
    phys.gravityX = gravityX;
    phys.gravityY = gravityY;
    phys.gravityZ = gravityZ;
    phys.windForceX = windX;
    phys.windForceZ = windZ;
    phys.timeScale = std::clamp(timeScale, 0.1f, 2.5f);
    phys.substeps = static_cast<uint32_t>(std::clamp<jint>(substeps, 1, 4));
    phys.boundsWidth = std::clamp(boundsWidth, 1.2f, 6.0f);
    phys.boundsHeight = std::clamp(boundsHeight, 1.2f, 5.0f);
    phys.boundsDepth = std::clamp(boundsDepth, 1.2f, 6.0f);
    phys.waveMakerEnabled = (waveMakerEnabled == JNI_TRUE);
    phys.waveFrequency = std::clamp(waveFrequency, 0.1f, 4.0f);
    phys.waveAmplitude = std::clamp(waveAmplitude, 0.0f, 1.5f);
    phys.obstacleEnabled = (obstacleEnabled == JNI_TRUE);
    phys.obstacleX = obstacleX;
    phys.obstacleY = obstacleY;
    phys.obstacleZ = obstacleZ;
    phys.obstacleRadius = std::clamp(obstacleRadius, 0.15f, 1.0f);

    sphere::water::FluidRenderParams ren{};
    ren.debugMode = static_cast<sphere::water::FluidDebugViewMode>(std::clamp<jint>(debugModeOrdinal, 0, 5));
    ren.particleVisualRadius = std::clamp(particleVisualRadius, 0.03f, 0.25f);
    ren.bilateralBlurEnabled = (bilateralBlurEnabled == JNI_TRUE);
    ren.blurRadiusPx = std::clamp<int32_t>(blurRadiusPx, 1, 10);
    ren.blurSpatialSigma = std::clamp(blurSpatialSigma, 0.5f, 8.0f);
    ren.blurDepthSigma = std::clamp(blurDepthSigma, 0.01f, 0.5f);
    ren.waterColorR = waterR;
    ren.waterColorG = waterG;
    ren.waterColorB = waterB;
    ren.absorptionR = absorbR;
    ren.absorptionG = absorbG;
    ren.absorptionB = absorbB;
    ren.absorptionStrength = std::clamp(absorbStrength, 0.1f, 6.0f);
    ren.indexOfRefraction = std::clamp(indexOfRefraction, 1.0f, 2.0f);
    ren.refractionStrength = std::clamp(refractionStrength, 0.0f, 0.6f);
    ren.fresnelPower = std::clamp(fresnelPower, 1.0f, 8.0f);
    ren.fresnelBias = std::clamp(fresnelBias, 0.0f, 0.3f);
    ren.specularPower = std::clamp(specularPower, 8.0f, 512.0f);
    ren.reflectionIntensity = std::clamp(reflectionIntensity, 0.0f, 1.5f);
    ren.foamThreshold = std::clamp(foamThreshold, 0.4f, 6.0f);
    ren.causticsIntensity = std::clamp(causticsIntensity, 0.0f, 1.5f);
    ren.sunAzimuthDeg = sunAzimuthDeg;
    ren.sunElevationDeg = std::clamp(sunElevationDeg, 5.0f, 88.0f);
    ren.showContainerBox = (showContainerBox == JNI_TRUE);
    ren.showFloorGrid = (showFloorGrid == JNI_TRUE);

    sphere::water::WaterSimulationEngine::Get().Configure(phys, ren, camYawDeg, camPitchDeg, camDistance);
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeResetWaterSimulation(
    JNIEnv* env, jclass clazz,
    jint scenarioOrdinal,
    jint particleCount
) {
    (void)env;
    (void)clazz;
    auto scenario = static_cast<sphere::water::WaterSpawnScenario>(std::clamp<jint>(scenarioOrdinal, 0, 5));
    uint32_t count = static_cast<uint32_t>(std::clamp<jint>(particleCount, 128, 4096));
    sphere::water::WaterSimulationEngine::Get().ResetSimulation(scenario, count);
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeConfigureWaterfall(
    JNIEnv* env, jclass clazz,
    jboolean enabled,
    jfloat cliffHeight,
    jfloat cliffWidth,
    jfloat cliffDepth,
    jfloat flowVolume,
    jfloat lipSlopeDeg,
    jfloat undercut,
    jint steppedLedges,
    jfloat roughness,
    jfloat rockChanneling,
    jfloat airDragAndMist,
    jfloat plungePoolDepth
) {
    (void)env;
    (void)clazz;
    sphere::water::WaterSimulationEngine::Get().ConfigureWaterfall(
        enabled == JNI_TRUE,
        cliffHeight,
        cliffWidth,
        cliffDepth,
        flowVolume,
        lipSlopeDeg,
        undercut,
        static_cast<int32_t>(steppedLedges),
        roughness,
        rockChanneling,
        airDragAndMist,
        plungePoolDepth
    );
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeInjectWaterImpulse(
    JNIEnv* env, jclass clazz,
    jfloat normU, jfloat normV, jfloat dragDx, jfloat dragDy, jfloat strength
) {
    (void)env;
    (void)clazz;
    sphere::water::WaterSimulationEngine::Get().InjectScreenImpulse(normU, normV, dragDx, dragDy, strength);
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeTriggerWaterPulse(
    JNIEnv* env, jclass clazz,
    jint pulseType, jfloat strength
) {
    (void)env;
    (void)clazz;
    sphere::water::WaterSimulationEngine::Get().TriggerSplashPulse(pulseType, strength);
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeStepAndRenderWaterBitmap(
    JNIEnv* env, jclass clazz,
    jobject bitmap,
    jfloat dtSeconds,
    jboolean isPlaying
) {
    (void)clazz;
    jfloatArray outTelemetry = env->NewFloatArray(9);
    if (!outTelemetry) return nullptr;

    jfloat vals[9] = {0};
    AndroidBitmapInfo info{};
    if (bitmap &&
        AndroidBitmap_getInfo(env, bitmap, &info) == ANDROID_BITMAP_RESULT_SUCCESS &&
        info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 &&
        info.width > 0 && info.height > 0 &&
        info.stride >= info.width * 4u) {
        void* pixelsPtr = nullptr;
        if (AndroidBitmap_lockPixels(env, bitmap, &pixelsPtr) == ANDROID_BITMAP_RESULT_SUCCESS && pixelsPtr) {
            auto t = sphere::water::WaterSimulationEngine::Get().StepAndRenderBitmap(
                static_cast<uint32_t*>(pixelsPtr),
                static_cast<int32_t>(info.stride / 4),
                info.width,
                info.height,
                std::clamp(FiniteOr(dtSeconds, 0.016f), 0.001f, 0.05f),
                isPlaying == JNI_TRUE
            );
            AndroidBitmap_unlockPixels(env, bitmap);

            vals[0] = static_cast<jfloat>(t.activeParticles);
            vals[1] = static_cast<jfloat>(t.occupiedCells);
            vals[2] = static_cast<jfloat>(t.hashTableSize);
            vals[3] = t.physicsStepMs;
            vals[4] = t.renderPassMs;
            vals[5] = t.avgDensity;
            vals[6] = t.maxSpeed;
            vals[7] = t.kineticEnergy;
            vals[8] = t.vulkanGpuActive ? 1.0f : 0.0f;
        }
    }
    env->SetFloatArrayRegion(outTelemetry, 0, 9, vals);
    return outTelemetry;
}

JNIEXPORT void JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeConfigureWaterSurface(
    JNIEnv* env, jclass clazz,
    jboolean enabled,
    jint gridResolution,
    jfloat patchSizeM,
    jfloat windSpeedMs,
    jfloat windDirectionDeg,
    jfloat amplitudeScale,
    jfloat choppinessLambda,
    jfloat smallWaveCutoff,
    jfloat timeScale,
    jfloat waterDepthM,
    jfloat seabedRoughness,
    jfloat seabedScale,
    jfloat sunAzimuthDeg,
    jfloat sunElevationDeg,
    jfloat skyTurbidity,
    jfloat indexOfRefraction,
    jfloat absorptionStrength,
    jfloat scatterStrength,
    jfloat foamThreshold,
    jfloat reflectionStrength,
    jint debugModeOrdinal,
    jboolean cutoutEnabled,
    jfloat cutoutWidthM,
    jfloat cutoutHeightM,
    jfloat cutoutDepthM
) {
    (void)env;
    (void)clazz;
    sphere::water::WaterSurfaceParams p{};
    p.enabled = (enabled == JNI_TRUE);
    p.gridResolution = static_cast<int32_t>(gridResolution);
    p.patchSizeM = patchSizeM;
    p.windSpeedMs = windSpeedMs;
    p.windDirectionDeg = windDirectionDeg;
    p.amplitudeScale = amplitudeScale;
    p.choppinessLambda = choppinessLambda;
    p.smallWaveCutoff = smallWaveCutoff;
    p.timeScale = timeScale;
    p.waterDepthM = waterDepthM;
    p.seabedRoughness = seabedRoughness;
    p.seabedScale = seabedScale;
    p.sunAzimuthDeg = sunAzimuthDeg;
    p.sunElevationDeg = sunElevationDeg;
    p.skyTurbidity = skyTurbidity;
    p.indexOfRefraction = indexOfRefraction;
    p.absorptionStrength = absorptionStrength;
    p.scatterStrength = scatterStrength;
    p.foamThreshold = foamThreshold;
    p.reflectionStrength = reflectionStrength;
    p.cutoutEnabled = (cutoutEnabled == JNI_TRUE);
    p.cutoutWidthM = std::clamp(cutoutWidthM, 2.0f, 36.0f);
    p.cutoutHeightM = std::clamp(cutoutHeightM, 2.0f, 28.0f);
    p.cutoutDepthM = std::clamp(cutoutDepthM, 2.0f, 36.0f);
    p.debugMode = static_cast<sphere::water::WaterSurfaceDebugMode>(std::clamp<jint>(debugModeOrdinal, 0, 5));
    sphere::water::WaterSimulationEngine::Get().ConfigureWaterSurface(p);
}

} // extern "C"

namespace {

inline sphere::Vec4 ComputeGlassSlotColor(int32_t glassTypeIdx, float windowIor) {
    float r = 0.14f, g = 0.42f, b = 0.62f;
    switch (glassTypeIdx) {
        case 1: r = 0.12f; g = 0.15f; b = 0.20f; break; // SolarTintedSmoke
        case 2: r = 0.58f; g = 0.66f; b = 0.74f; break; // ReflectiveChromeMirror
        case 3: r = 0.08f; g = 0.48f; b = 0.36f; break; // LowEEmerald
        case 4: r = 0.42f; g = 0.25f; b = 0.12f; break; // BronzeArchitectural
        case 5: r = 0.82f; g = 0.87f; b = 0.92f; break; // FrostedSatinPrivacy
        case 6: r = 0.86f; g = 0.64f; b = 0.18f; break; // GoldMirroredLuxury
        case 7: r = 0.42f; g = 0.28f; b = 0.78f; break; // StainedGlassArtisan
        case 8: r = 0.38f; g = 0.46f; b = 0.52f; break; // WiredSafety
        default: break;                               // ClearFloat
    }
    float safeIor = std::clamp(windowIor, 1.0f, 2.8f);
    float alpha = 0.32f;
    if (glassTypeIdx == 5) {
        alpha = std::clamp(0.88f + 0.10f * ((safeIor - 1.0f) / 1.8f), 0.88f, 0.98f);
    } else {
        float iorNorm = std::clamp((safeIor - 1.0f) / 0.55f, 0.0f, 1.0f);
        float smooth = iorNorm * iorNorm * (3.0f - 2.0f * iorNorm);
        alpha = std::clamp(0.28f + 0.72f * smooth, 0.28f, 1.0f);
    }
    return sphere::Vec4(r, g, b, alpha);
}

inline sphere::Vec4 ResolveBuildingMaterialColor(
    uint32_t materialId,
    float u,
    float v,
    const sphere::Vec4& wallTint,
    bool hasCustomWallTint,
    int32_t glassTypeIdx,
    float windowIor
) {
    using MS = procedural::buildings::MaterialSlot;
    switch (static_cast<MS>(materialId)) {
        case MS::WallConcrete: {
            sphere::Vec4 base(0.73f, 0.74f, 0.76f, 1.0f);
            if (hasCustomWallTint) {
                base.x = base.x * 0.45f + wallTint.x * 0.55f;
                base.y = base.y * 0.45f + wallTint.y * 0.55f;
                base.z = base.z * 0.45f + wallTint.z * 0.55f;
            }
            return base;
        }
        case MS::Glass:
            return ComputeGlassSlotColor(glassTypeIdx, windowIor);
        case MS::FrameMetal:
            return sphere::Vec4(0.18f, 0.20f, 0.24f, 1.0f);
        case MS::DoorWoodMetal:
            return sphere::Vec4(0.42f, 0.25f, 0.14f, 1.0f);
        case MS::RoofSurface:
            return sphere::Vec4(0.36f, 0.21f, 0.18f, 1.0f);
        case MS::Foundation:
            return sphere::Vec4(0.26f, 0.28f, 0.31f, 1.0f);
        case MS::HvacMetal:
            return sphere::Vec4(0.58f, 0.62f, 0.66f, 1.0f);
        case MS::GlassTinted:
            return ComputeGlassSlotColor(1, windowIor);
        case MS::GlassReflective:
            return ComputeGlassSlotColor(2, windowIor);
        case MS::GlassFrosted:
            return ComputeGlassSlotColor(5, windowIor);
        case MS::GlassEmerald:
            return ComputeGlassSlotColor(3, windowIor);
        case MS::GlassGold:
            return ComputeGlassSlotColor(6, windowIor);
        case MS::GlassWired:
            return ComputeGlassSlotColor(8, windowIor);
        case MS::GlassBronze:
            return ComputeGlassSlotColor(4, windowIor);
        case MS::GlassStained:
            return ComputeGlassSlotColor(7, windowIor);
        case MS::BalconyMetal:
            return sphere::Vec4(0.20f, 0.23f, 0.27f, 1.0f);
        case MS::BalconyGlass: {
            sphere::Vec4 g = ComputeGlassSlotColor(glassTypeIdx, std::min(windowIor, 1.38f));
            g.w = std::min(g.w, 0.52f);
            return g;
        }
        case MS::WoodShutters:
            return sphere::Vec4(0.32f, 0.20f, 0.12f, 1.0f);
        case MS::CorniceStone:
            return sphere::Vec4(0.86f, 0.84f, 0.78f, 1.0f);
        case MS::BrassHardware:
            return sphere::Vec4(0.84f, 0.68f, 0.26f, 1.0f);
        case MS::WallBrick: {
            float fy = v * 12.0f;
            float row = std::floor(fy);
            float fx = u * 6.0f + std::fmod(row, 2.0f) * 0.5f;
            float cx = fx - std::floor(fx);
            float cy = fy - row;
            bool isBrick = (cx > 0.06f && cx < 0.94f && cy > 0.10f && cy < 0.90f);
            if (!isBrick) return sphere::Vec4(0.72f, 0.69f, 0.65f, 1.0f);
            float grain = 0.92f + 0.12f * std::sin(std::floor(fx) * 12.9898f + row * 78.233f);
            return sphere::Vec4(
                std::clamp(0.56f * grain, 0.0f, 1.0f),
                std::clamp(0.23f * grain, 0.0f, 1.0f),
                std::clamp(0.18f * grain, 0.0f, 1.0f),
                1.0f
            );
        }
        case MS::WallLimestone:
            return sphere::Vec4(0.86f, 0.82f, 0.73f, 1.0f);
        case MS::WallStucco:
            return hasCustomWallTint ? wallTint : sphere::Vec4(0.91f, 0.90f, 0.86f, 1.0f);
        case MS::WallCorten:
            return sphere::Vec4(0.54f, 0.24f, 0.12f, 1.0f);
        case MS::WallBasalt:
            return sphere::Vec4(0.16f, 0.18f, 0.21f, 1.0f);
        case MS::WallTimber:
            return sphere::Vec4(0.56f, 0.36f, 0.20f, 1.0f);
        case MS::PergolaWood:
            return sphere::Vec4(0.48f, 0.29f, 0.15f, 1.0f);
        case MS::GreenTurfDeck:
            return sphere::Vec4(0.22f, 0.56f, 0.28f, 1.0f);
        case MS::PlanterBox:
            return sphere::Vec4(0.46f, 0.38f, 0.31f, 1.0f);
        case MS::WallBlackAluminum:
            return sphere::Vec4(0.12f, 0.14f, 0.17f, 1.0f);
        case MS::WallChampagneAluminum:
            return sphere::Vec4(0.79f, 0.73f, 0.63f, 1.0f);
        case MS::WallGunmetalAluminum:
            return sphere::Vec4(0.30f, 0.33f, 0.37f, 1.0f);
        case MS::WallBronzeAluminum:
            return sphere::Vec4(0.44f, 0.32f, 0.21f, 1.0f);
        case MS::WallSilverAluminum:
            return sphere::Vec4(0.76f, 0.79f, 0.83f, 1.0f);
        case MS::WallSpandrelGlass:
            return sphere::Vec4(0.14f, 0.26f, 0.38f, 1.0f);
        case MS::StadiumSeatBlue:
            return sphere::Vec4(0.12f, 0.42f, 0.85f, 1.0f);
        case MS::TrackRed:
            return sphere::Vec4(0.76f, 0.22f, 0.18f, 1.0f);
        case MS::SafetyYellow:
            return sphere::Vec4(0.95f, 0.78f, 0.14f, 1.0f);
        case MS::TarmacDark:
            return sphere::Vec4(0.16f, 0.17f, 0.19f, 1.0f);
        case MS::CommercialAccentRed:
            return sphere::Vec4(0.84f, 0.16f, 0.18f, 1.0f);
        case MS::CommercialSignOrange:
            return sphere::Vec4(0.96f, 0.48f, 0.10f, 1.0f);
        case MS::TireRubber:
            return sphere::Vec4(0.10f, 0.10f, 0.11f, 1.0f);
        case MS::ShopFloorEpoxy:
            return sphere::Vec4(0.36f, 0.44f, 0.50f, 1.0f);
        case MS::CardboardTan:
            return sphere::Vec4(0.68f, 0.53f, 0.34f, 1.0f);
        case MS::VehicleBlue:
            return sphere::Vec4(0.18f, 0.44f, 0.82f, 1.0f);
        case MS::VehicleWhite:
            return sphere::Vec4(0.92f, 0.94f, 0.96f, 1.0f);
        case MS::HydraulicGreen:
            return sphere::Vec4(0.16f, 0.68f, 0.36f, 1.0f);
        case MS::LightBulbWarm:
            return sphere::Vec4(1.00f, 0.88f, 0.62f, 1.0f);
        case MS::LightBulbDaylight:
            return sphere::Vec4(0.90f, 0.96f, 1.00f, 1.0f);
        case MS::LightFixtureMetal:
            return sphere::Vec4(0.26f, 0.28f, 0.32f, 1.0f);
        default:
            return sphere::Vec4(0.75f, 0.76f, 0.78f, 1.0f);
    }
}

std::shared_ptr<sphere::Mesh> ConvertBuildingPayloadToSphereMesh(
    const procedural::GeneratedMeshPayload& payload,
    jint wallTintArgb,
    jint glassTypeIdx,
    float windowIor
) {
    auto mesh = std::make_shared<sphere::Mesh>();
    mesh->name = "ProceduralBuildingMesh";

    const size_t floatsPerVert = 9;
    const size_t vertCount = payload.vertexBuffer.size() / floatsPerVert;
    if (vertCount == 0 || payload.indexBuffer.size() < 3) {
        return mesh;
    }

    const sphere::Vec4 wallTint = UnpackArgbToVec4(wallTintArgb, 1.0f);
    const bool hasCustomWallTint = (static_cast<uint32_t>(wallTintArgb) & 0x00FFFFFFu) != 0x00E2E8F0u;

    mesh->vertices.reserve(vertCount);
    for (size_t i = 0; i < vertCount; ++i) {
        const size_t base = i * floatsPerVert;
        sphere::Vertex v;
        v.position = sphere::Vec3(
            payload.vertexBuffer[base + 0],
            payload.vertexBuffer[base + 1],
            payload.vertexBuffer[base + 2]
        );
        v.normal = sphere::Vec3(
            payload.vertexBuffer[base + 3],
            payload.vertexBuffer[base + 4],
            payload.vertexBuffer[base + 5]
        );
        float u = payload.vertexBuffer[base + 6];
        float tv = payload.vertexBuffer[base + 7];
        uint32_t matId = static_cast<uint32_t>(std::max(0.0f, payload.vertexBuffer[base + 8]) + 0.5f);
        v.uv[0] = u;
        v.uv[1] = tv;
        v.color = ResolveBuildingMaterialColor(
            matId, u, tv, wallTint, hasCustomWallTint, glassTypeIdx, windowIor
        );
        mesh->vertices.push_back(v);
    }

    // Draw opaque triangles first, translucent glass triangles second for proper Z-buffer compositing
    std::vector<uint32_t> opaqueIndices;
    std::vector<uint32_t> glassIndices;
    opaqueIndices.reserve(payload.indexBuffer.size());
    glassIndices.reserve(payload.indexBuffer.size() / 4);

    const size_t triCount = payload.indexBuffer.size() / 3;
    for (size_t t = 0; t < triCount; ++t) {
        uint32_t i0 = payload.indexBuffer[t * 3 + 0];
        uint32_t i1 = payload.indexBuffer[t * 3 + 1];
        uint32_t i2 = payload.indexBuffer[t * 3 + 2];
        if (i0 >= vertCount || i1 >= vertCount || i2 >= vertCount) continue;
        float minAlpha = std::min({
            mesh->vertices[i0].color.w,
            mesh->vertices[i1].color.w,
            mesh->vertices[i2].color.w
        });
        if (minAlpha < 0.99f) {
            glassIndices.push_back(i0);
            glassIndices.push_back(i1);
            glassIndices.push_back(i2);
        } else {
            opaqueIndices.push_back(i0);
            opaqueIndices.push_back(i1);
            opaqueIndices.push_back(i2);
        }
    }

    mesh->indices.reserve(opaqueIndices.size() + glassIndices.size());
    mesh->indices.insert(mesh->indices.end(), opaqueIndices.begin(), opaqueIndices.end());
    mesh->indices.insert(mesh->indices.end(), glassIndices.begin(), glassIndices.end());
    mesh->CalculateBounds();
    return mesh;
}

} // namespace

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetBuildingDiagnostics(
    JNIEnv* env, jclass clazz
) {
    (void)clazz;
    std::string diag = procedural::buildings::BuildingSystem::instance().getDiagnostics();
    return env->NewStringUTF(diag.c_str());
}

JNIEXPORT jfloatArray JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeRenderBuildingBitmap(
    JNIEnv* env, jclass clazz,
    jobject bitmap,
    jint domainOrdinal,
    jint subTypeOrdinal,
    jfloat rotX,
    jfloat rotY,
    jfloat zoom,
    jint shapeOrdinal,
    jint cornerOrdinal,
    jfloat cornerSize,
    jint stepModeOrdinal,
    jint stepTiers,
    jfloat topTaperRatio,
    jfloat stepShrinkFactor,
    jboolean stepTerraceRailing,
    jint floorCount,
    jfloat floorHeight,
    jfloat width,
    jfloat depth,
    jfloat cornerRadius,
    jint windowsPerFacade,
    jint windowTypeOrdinal,
    jfloat windowWidth,
    jfloat windowHeight,
    jfloat windowSillHeight,
    jint glassTypeOrdinal,
    jfloat windowIor,
    jint wallMaterialOrdinal,
    jint wallColorArgb,
    jint doorTypeOrdinal,
    jfloat doorWidth,
    jfloat doorHeight,
    jboolean hasEntranceCanopy,
    jint roofTypeOrdinal,
    jfloat roofHeight,
    jfloat roofOverhang,
    jboolean showRooftopClutter,
    jint balconyTypeOrdinal,
    jboolean hasBalconies,
    jfloat balconyDepth,
    jboolean hasSunLouvres,
    jboolean hasFireEscape,
    jboolean hasCornice,
    jboolean hasGroundPillars,
    jint pillarCount,
    jboolean hasWindowShutters,
    jboolean showInside,
    jint interiorDesign,
    jfloat detailIntensity,
    jint detailCount,
    jboolean detailFlagA,
    jboolean detailFlagB,
    jboolean wireframe
) {
    (void)clazz;
    jfloatArray result = env->NewFloatArray(6);
    if (!result) return nullptr;
    jfloat outStats[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

    if (!bitmap) {
        env->SetFloatArrayRegion(result, 0, 6, outStats);
        return result;
    }

    AndroidBitmapInfo info{};
    if (AndroidBitmap_getInfo(env, bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS ||
        info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 ||
        info.width == 0 || info.height == 0 ||
        info.stride < info.width * 4u) {
        env->SetFloatArrayRegion(result, 0, 6, outStats);
        return result;
    }

    using namespace procedural::buildings;

    const float safeRotX = std::clamp(FiniteOr(rotX, 20.0f), -85.0f, 85.0f);
    const float safeRotY = FiniteOr(rotY, -35.0f);
    const float safeZoom = std::clamp(FiniteOr(zoom, 1.0f), 0.25f, 4.5f);
    const int32_t safeFloors = std::clamp<int32_t>(floorCount, 1, 80);
    const float safeFloorH = std::clamp(FiniteOr(floorHeight, 3.0f), 2.2f, 8.0f);
    const float safeW = std::clamp(FiniteOr(width, 20.0f), 3.0f, 120.0f);
    const float safeD = std::clamp(FiniteOr(depth, 14.0f), 3.0f, 120.0f);
    const float safeCornerSize = std::clamp(FiniteOr(cornerSize, 2.0f), 0.2f, 12.0f);
    const float safeCornerRad = std::clamp(FiniteOr(cornerRadius, 3.0f), 0.2f, 20.0f);
    const int32_t safeWinCount = std::clamp<int32_t>(windowsPerFacade, 0, 24);
    const float safeWinW = std::clamp(FiniteOr(windowWidth, 1.6f), 0.4f, 6.0f);
    const float safeWinH = std::clamp(FiniteOr(windowHeight, 1.8f), 0.4f, 4.5f);
    const float safeWinSill = std::clamp(FiniteOr(windowSillHeight, 0.8f), 0.0f, 2.2f);
    const float safeDoorW = std::clamp(FiniteOr(doorWidth, 2.4f), 0.8f, 8.0f);
    const float safeDoorH = std::clamp(FiniteOr(doorHeight, 2.6f), 1.8f, 6.0f);
    const float safeRoofH = std::clamp(FiniteOr(roofHeight, 2.5f), 0.2f, 18.0f);
    const float safeRoofOver = std::clamp(FiniteOr(roofOverhang, 0.6f), 0.0f, 4.0f);
    const float safeBalcDepth = std::clamp(FiniteOr(balconyDepth, 1.0f), 0.4f, 4.0f);
    const float safeIor = std::clamp(FiniteOr(windowIor, 1.52f), 1.0f, 2.8f);

    const auto shapeEnum = static_cast<BuildingVolumeShape>(std::clamp<int32_t>(shapeOrdinal, 0, 26));
    const auto cornerEnum = static_cast<BuildingCornerType>(std::clamp<int32_t>(cornerOrdinal, 0, 6));
    const auto stepEnum = static_cast<BuildingStepMode>(std::clamp<int32_t>(stepModeOrdinal, 0, 4));
    const auto winEnum = static_cast<WindowType>(std::clamp<int32_t>(windowTypeOrdinal, 0, 22));
    const int32_t safeGlassIdx = std::clamp<int32_t>(glassTypeOrdinal, 0, 8);
    const auto glassEnum = static_cast<GlassType>(safeGlassIdx);
    const auto wallEnum = static_cast<WallMaterialType>(std::clamp<int32_t>(wallMaterialOrdinal, 0, 15));
    const auto doorEnum = static_cast<DoorType>(std::clamp<int32_t>(doorTypeOrdinal, 0, 9));
    const auto roofEnum = static_cast<RoofType>(std::clamp<int32_t>(roofTypeOrdinal, 0, 21));
    const auto balcEnum = static_cast<BalconyType>(std::clamp<int32_t>(balconyTypeOrdinal, 0, 3));

    procedural::GeneratedMeshPayload payload;
    BuildingStats bStats{};
    bool hasExactStats = false;
    bool doubleSided = (showInside == JNI_TRUE);

    {
        std::lock_guard<std::mutex> lock(gGeometryRegistryMutex);
        auto& bs = BuildingSystem::instance();
        procedural::GeneratorContext ctx{};

        switch (domainOrdinal) {
            case 0: { // ProceduralBuildingGenerator (Massing & Shapes)
                BuildingParameters p{};
                p.shape = shapeEnum;
                p.cornerType = cornerEnum;
                p.cornerSize = safeCornerSize;
                p.stepMode = stepEnum;
                p.stepTiers = std::clamp<int32_t>(stepTiers, 2, 6);
                p.topTaperRatio = std::clamp(FiniteOr(topTaperRatio, 0.55f), 0.20f, 0.95f);
                p.stepShrinkFactor = std::clamp(FiniteOr(stepShrinkFactor, 0.20f), 0.05f, 0.40f);
                p.stepTerraceRailing = (stepTerraceRailing == JNI_TRUE);
                p.floorCount = safeFloors;
                p.floorHeight = safeFloorH;
                p.width = safeW;
                p.depth = safeD;
                p.cornerRadius = safeCornerRad;
                p.windowsPerFacade = safeWinCount;
                p.windowType = winEnum;
                p.windowWidth = safeWinW;
                p.windowHeight = safeWinH;
                p.windowSillHeight = safeWinSill;
                p.glassType = glassEnum;
                p.wallMaterial = wallEnum;
                p.doorType = doorEnum;
                p.doorWidth = safeDoorW;
                p.doorHeight = safeDoorH;
                p.hasEntranceCanopy = (hasEntranceCanopy == JNI_TRUE);
                p.roofType = roofEnum;
                p.roofHeight = safeRoofH;
                p.roofOverhang = safeRoofOver;
                p.showRooftopClutter = (showRooftopClutter == JNI_TRUE);
                p.balconyType = balcEnum;
                p.hasBalconies = (hasBalconies == JNI_TRUE) || (balcEnum != BalconyType::None);
                p.balconyDepth = safeBalcDepth;
                p.hasSunLouvres = (hasSunLouvres == JNI_TRUE);
                p.hasFireEscape = (hasFireEscape == JNI_TRUE);
                p.hasCornice = (hasCornice == JNI_TRUE);
                p.hasGroundPillars = (hasGroundPillars == JNI_TRUE);
                p.pillarCount = std::clamp<int32_t>(pillarCount, 2, 16);
                p.hasWindowShutters = (hasWindowShutters == JNI_TRUE);
                p.showInside = (showInside == JNI_TRUE);
                p.interiorDesign = std::clamp<int32_t>(interiorDesign, 0, 3);

                auto gen = bs.getProceduralBuildingGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                bStats = gen->getStats();
                hasExactStats = true;
                break;
            }
            case 1: { // HouseGen (53 Residential Types)
                ResidentialParams p{};
                p.subType = static_cast<ResidentialType>(std::clamp<int32_t>(subTypeOrdinal, 0, 52));
                p.stories = safeFloors;
                p.storyHeight = safeFloorH;
                p.footprintWidth = safeW;
                p.footprintDepth = safeD;
                p.roofStyle = roofEnum;
                p.hasGarage = (detailFlagA == JNI_TRUE);
                p.hasPorchOrBalcony = (hasBalconies == JNI_TRUE);
                p.wallColor = static_cast<uint32_t>(wallColorArgb);
                p.useExtendedParams = true;
                p.windowsPerFacade = safeWinCount;
                p.windowType = winEnum;
                p.windowWidth = safeWinW;
                p.windowHeight = safeWinH;
                p.windowSillHeight = safeWinSill;
                p.glassType = glassEnum;
                p.doorType = doorEnum;
                p.doorWidth = safeDoorW;
                p.doorHeight = safeDoorH;
                p.hasEntranceCanopy = (hasEntranceCanopy == JNI_TRUE);
                p.roofHeight = safeRoofH;
                p.roofOverhang = safeRoofOver;
                p.showRooftopClutter = (showRooftopClutter == JNI_TRUE);
                p.wallMaterial = wallEnum;
                p.hasCornice = (hasCornice == JNI_TRUE);
                p.hasWindowShutters = (hasWindowShutters == JNI_TRUE);
                p.hasSunLouvres = (hasSunLouvres == JNI_TRUE);
                p.balconyDepth = safeBalcDepth;

                auto gen = bs.getHouseGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 2:   // TowerGen (36 Commercial Types)
            case 3:   // PetrolStationGen
            case 4: { // CarWashGen
                CommercialParams p{};
                p.subType = static_cast<CommercialType>(std::clamp<int32_t>(subTypeOrdinal, 0, 35));
                p.stories = safeFloors;
                p.storyHeight = safeFloorH;
                p.footprintWidth = safeW;
                p.footprintDepth = safeD;
                p.hasSetbacks = (detailFlagA == JNI_TRUE);
                p.hasEntranceAtrium = (detailFlagB == JNI_TRUE);
                p.curtainWallGlazing = (wallEnum == WallMaterialType::GlassCurtainWall ||
                                        wallEnum == WallMaterialType::StructuralSiliconeGlazing ||
                                        wallEnum == WallMaterialType::SpiderPointFixedGlass);
                p.useExtendedParams = true;
                p.windowsPerFacade = safeWinCount;
                p.windowType = winEnum;
                p.windowWidth = safeWinW;
                p.windowHeight = safeWinH;
                p.windowSillHeight = safeWinSill;
                p.glassType = glassEnum;
                p.doorType = doorEnum;
                p.doorWidth = safeDoorW;
                p.doorHeight = safeDoorH;
                p.hasEntranceCanopy = (hasEntranceCanopy == JNI_TRUE);
                p.roofStyle = roofEnum;
                p.roofHeight = safeRoofH;
                p.roofOverhang = safeRoofOver;
                p.showRooftopClutter = (showRooftopClutter == JNI_TRUE);
                p.wallMaterial = wallEnum;
                p.hasBalconies = (hasBalconies == JNI_TRUE);
                p.hasCornice = (hasCornice == JNI_TRUE);
                p.hasWindowShutters = (hasWindowShutters == JNI_TRUE);
                p.hasSunLouvres = (hasSunLouvres == JNI_TRUE);
                p.balconyDepth = safeBalcDepth;
                p.showInside = (showInside == JNI_TRUE);
                p.interiorDesign = std::clamp<int32_t>(interiorDesign, 0, 3);

                if (domainOrdinal == 3) {
                    auto gen = bs.getPetrolStationGen();
                    gen->configure(p);
                    payload = gen->generateMesh(ctx);
                } else if (domainOrdinal == 4) {
                    auto gen = bs.getCarWashGen();
                    gen->configure(p);
                    payload = gen->generateMesh(ctx);
                } else {
                    auto gen = bs.getTowerGen();
                    gen->configure(p);
                    payload = gen->generateMesh(ctx);
                }
                break;
            }
            case 5: { // WarehouseGen (14 Industrial Types)
                IndustrialParams p{};
                p.subType = static_cast<IndustrialType>(std::clamp<int32_t>(subTypeOrdinal, 0, 13));
                p.length = safeD;
                p.width = safeW;
                p.height = safeFloors * safeFloorH;
                p.loadingBays = std::clamp<int32_t>(detailCount, 1, 10);
                p.hasCatwalks = (detailFlagA == JNI_TRUE);
                p.hasSawtoothRoof = (detailFlagB == JNI_TRUE);
                p.useExtendedParams = true;
                p.windowsPerFacade = safeWinCount;
                p.windowType = winEnum;
                p.windowWidth = safeWinW;
                p.windowHeight = safeWinH;
                p.windowSillHeight = safeWinSill;
                p.glassType = glassEnum;
                p.doorType = doorEnum;
                p.doorWidth = safeDoorW;
                p.doorHeight = safeDoorH;
                p.hasEntranceCanopy = (hasEntranceCanopy == JNI_TRUE);
                p.roofStyle = roofEnum;
                p.roofHeight = safeRoofH;
                p.roofOverhang = safeRoofOver;
                p.showRooftopClutter = (showRooftopClutter == JNI_TRUE);
                p.wallMaterial = wallEnum;
                p.hasCornice = (hasCornice == JNI_TRUE);
                p.showInside = (showInside == JNI_TRUE);
                p.interiorDesign = std::clamp<int32_t>(interiorDesign, 0, 3);

                auto gen = bs.getWarehouseGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 6: { // CivicGen (20 Civic Types)
                CivicParams p{};
                p.subType = static_cast<CivicType>(std::clamp<int32_t>(subTypeOrdinal, 0, 19));
                p.stories = safeFloors;
                p.storyHeight = safeFloorH;
                p.mainWidth = safeW;
                p.mainDepth = safeD;
                p.wingLength = safeW * 0.42f;
                p.hasClockTower = (detailFlagA == JNI_TRUE);
                p.hasHelipad = (detailFlagB == JNI_TRUE);
                p.hasPortico = (hasEntranceCanopy == JNI_TRUE);
                p.columnsCount = std::clamp<int32_t>(pillarCount, 2, 12);
                p.busBays = std::clamp<int32_t>(detailCount, 2, 10);
                p.hasCanopy = (hasEntranceCanopy == JNI_TRUE);
                p.hasFloodlights = (showRooftopClutter == JNI_TRUE);
                p.hasControlTower = (detailFlagA == JNI_TRUE);
                p.useExtendedParams = true;
                p.windowsPerFacade = safeWinCount;
                p.windowType = winEnum;
                p.windowWidth = safeWinW;
                p.windowHeight = safeWinH;
                p.windowSillHeight = safeWinSill;
                p.glassType = glassEnum;
                p.doorType = doorEnum;
                p.doorWidth = safeDoorW;
                p.doorHeight = safeDoorH;
                p.hasEntranceCanopy = (hasEntranceCanopy == JNI_TRUE);
                p.roofStyle = roofEnum;
                p.roofHeight = safeRoofH;
                p.roofOverhang = safeRoofOver;
                p.showRooftopClutter = (showRooftopClutter == JNI_TRUE);
                p.wallMaterial = wallEnum;
                p.hasBalconies = (hasBalconies == JNI_TRUE);
                p.hasCornice = (hasCornice == JNI_TRUE);
                p.hasWindowShutters = (hasWindowShutters == JNI_TRUE);
                p.hasSunLouvres = (hasSunLouvres == JNI_TRUE);
                p.balconyDepth = safeBalcDepth;

                auto gen = bs.getCivicGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 7: { // RoomGen (5 Interior Types)
                RoomParams p{};
                p.subType = static_cast<RoomType>(std::clamp<int32_t>(subTypeOrdinal, 0, 4));
                p.width = std::clamp(safeW, 4.0f, 40.0f);
                p.length = std::clamp(safeD, 4.0f, 40.0f);
                p.ceilingHeight = std::clamp(safeFloorH, 2.4f, 9.0f);
                p.hasInteriorDoors = (detailFlagA == JNI_TRUE);
                p.hasFurnitureStubs = (detailFlagB == JNI_TRUE);
                p.cubicleRows = std::clamp<int32_t>(detailCount, 1, 6);
                p.cubicleCols = std::clamp<int32_t>(detailCount, 1, 6);
                p.showInside = true;
                p.interiorDesign = std::clamp<int32_t>(interiorDesign, 0, 3);
                p.loadingBays = std::clamp<int32_t>(detailCount, 1, 6);

                doubleSided = true;
                auto gen = bs.getRoomGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 8: { // PropAttach (3 Rooftop Prop Types)
                PropAttachParams p{};
                p.subType = static_cast<AttachmentType>(std::clamp<int32_t>(subTypeOrdinal, 0, 2));
                p.scale = std::clamp(FiniteOr(detailIntensity, 1.0f), 0.4f, 4.0f);
                p.clusterUnits = (detailFlagA == JNI_TRUE);
                p.clusterCount = std::clamp<int32_t>(detailCount, 1, 8);

                auto gen = bs.getPropAttach();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 9: { // NeonSignGen (3 Signage Types)
                NeonSignParams p{};
                p.subType = static_cast<SignType>(std::clamp<int32_t>(subTypeOrdinal, 0, 2));
                p.width = std::clamp(safeW, 2.0f, 24.0f);
                p.height = std::clamp(safeFloorH * 1.4f, 1.5f, 12.0f);
                p.columnHeight = std::clamp(safeFloors * safeFloorH * 0.6f, 2.0f, 18.0f);
                p.hasSpotlights = (detailFlagA == JNI_TRUE);
                p.hasCatwalk = (detailFlagB == JNI_TRUE);

                auto gen = bs.getNeonSignGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 10: { // DamageGen (3 Structural Damage Types)
                DamageParams p{};
                p.subType = static_cast<DamageType>(std::clamp<int32_t>(subTypeOrdinal, 0, 2));
                p.severity = std::clamp(FiniteOr(detailIntensity, 0.65f), 0.1f, 1.0f);
                p.radius = std::clamp(safeW * 0.25f, 1.5f, 10.0f);
                p.exposeRebar = (detailFlagA == JNI_TRUE);
                p.rubbleChunkCount = std::clamp<int32_t>(detailCount, 6, 60);

                auto gen = bs.getDamageGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
            case 11: // ImpostorGen (3 LOD & Distant Block Types)
            default: {
                ImpostorParams p{};
                p.subType = static_cast<ImpostorType>(std::clamp<int32_t>(subTypeOrdinal, 0, 2));
                p.blockWidth = std::clamp(safeW * 2.5f, 20.0f, 160.0f);
                p.blockDepth = std::clamp(safeD * 2.5f, 20.0f, 160.0f);
                p.buildingCountX = std::clamp<int32_t>(detailCount, 1, 6);
                p.buildingCountZ = std::clamp<int32_t>(detailCount, 1, 6);
                p.maxBuildingHeight = std::clamp(safeFloors * safeFloorH, 6.0f, 120.0f);
                if (p.subType != ImpostorType::CityBlockOctahedral) {
                    doubleSided = true;
                }

                auto gen = bs.getImpostorGen();
                gen->configure(p);
                payload = gen->generateMesh(ctx);
                break;
            }
        }
    }

    auto sphereMesh = ConvertBuildingPayloadToSphereMesh(
        payload, wallColorArgb, safeGlassIdx, safeIor
    );

    const int32_t vtxCount = static_cast<int32_t>(sphereMesh->GetVertexCount());
    const int32_t triCount = static_cast<int32_t>(sphereMesh->GetTriangleCount());

    float minX = sphereMesh->bounds.min.x;
    float maxX = sphereMesh->bounds.max.x;
    float minY = sphereMesh->bounds.min.y;
    float maxY = sphereMesh->bounds.max.y;
    float minZ = sphereMesh->bounds.min.z;
    float maxZ = sphereMesh->bounds.max.z;
    float spanX = std::max(0.0f, maxX - minX);
    float spanY = std::max(0.0f, maxY - minY);
    float spanZ = std::max(0.0f, maxZ - minZ);

    if (hasExactStats) {
        outStats[0] = static_cast<jfloat>(bStats.vertexCount > 0 ? bStats.vertexCount : vtxCount);
        outStats[1] = static_cast<jfloat>(bStats.triangleCount > 0 ? bStats.triangleCount : triCount);
        outStats[2] = bStats.totalHeightMeters > 0.f ? bStats.totalHeightMeters : spanY;
        outStats[3] = bStats.footprintAreaSqM > 0.f ? bStats.footprintAreaSqM : (spanX * spanZ);
        outStats[4] = bStats.totalGrossFloorAreaSqM > 0.f ? bStats.totalGrossFloorAreaSqM : (spanX * spanZ * safeFloors);
        outStats[5] = static_cast<jfloat>(bStats.totalWindowCount);
    } else {
        float footArea = spanX * spanZ;
        outStats[0] = static_cast<jfloat>(vtxCount);
        outStats[1] = static_cast<jfloat>(triCount);
        outStats[2] = spanY;
        outStats[3] = footArea;
        outStats[4] = (domainOrdinal >= 8) ? footArea : (footArea * static_cast<float>(safeFloors));
        outStats[5] = (domainOrdinal <= 6) ? static_cast<jfloat>(safeWinCount * 4 * safeFloors) : 0.f;
    }

    float matRoughness = 0.65f;
    float matMetallic = 0.12f;
    switch (wallEnum) {
        case WallMaterialType::GlassCurtainWall:
        case WallMaterialType::StructuralSiliconeGlazing:
        case WallMaterialType::DoubleSkinAerodynamicGlass:
        case WallMaterialType::SpiderPointFixedGlass:
            matRoughness = 0.18f; matMetallic = 0.35f; break;
        case WallMaterialType::BlackAluminumGlassGrid:
        case WallMaterialType::ChampagneAluminumGlass:
        case WallMaterialType::BronzeAluminumSolarFins:
        case WallMaterialType::GunmetalAluminumCassette:
        case WallMaterialType::PerforatedAluminumBriseSoleil:
            matRoughness = 0.28f; matMetallic = 0.68f; break;
        case WallMaterialType::WeatheredCortenSteel:
            matRoughness = 0.52f; matMetallic = 0.55f; break;
        default:
            matRoughness = 0.68f; matMetallic = 0.08f; break;
    }

    void* pixelsPtr = nullptr;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixelsPtr) == ANDROID_BITMAP_RESULT_SUCCESS && pixelsPtr) {
        sphere::scene::BuildingRasterParams rParams;
        rParams.rotXDeg = safeRotX;
        rParams.rotYDeg = safeRotY;
        rParams.zoom = safeZoom;
        rParams.roughness = matRoughness;
        rParams.metallic = matMetallic;
        rParams.wireframe = (wireframe == JNI_TRUE);
        rParams.doubleSided = doubleSided;

        sphere::scene::NativeMeshRasterizer::RenderBuildingBitmap(
            static_cast<uint32_t*>(pixelsPtr),
            static_cast<int32_t>(info.stride / 4),
            static_cast<int32_t>(info.width),
            static_cast<int32_t>(info.height),
            sphereMesh.get(),
            rParams
        );
        AndroidBitmap_unlockPixels(env, bitmap);
    }

    env->SetFloatArrayRegion(result, 0, 6, outStats);
    return result;
}

} // extern "C"



