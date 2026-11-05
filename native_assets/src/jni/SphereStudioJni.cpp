#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <vulkan/vulkan.h>
#include "textures/spec/shared_spec.h"
#include "core/Application.h"
#include "core/Log.h"
#include "shared/GeneratorRegistry.h"
#include "vulkan/VulkanNativeSceneRenderer.h"
#define LOG_TAG "SphereStudioNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
extern "C" {
JNIEXPORT jboolean JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeInitEngine(JNIEnv* env, jclass clazz) {
    LOGI("Sphere Studio Native C++20 Core initializing with Sascha Willems Vulkan 1.3 pipeline...");
    sphere::Log::Init();
    sphere::GeneratorRegistry::Get().Initialize();
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

} // extern "C"

