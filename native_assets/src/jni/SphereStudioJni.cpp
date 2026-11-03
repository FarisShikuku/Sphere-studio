#include <jni.h>
#include <string>
#include <android/log.h>
#include <vulkan/vulkan.h>
#include "textures/spec/shared_spec.h"
#include "core/Application.h"
#include "core/Log.h"
#include "shared/GeneratorRegistry.h"
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
} // extern "C
