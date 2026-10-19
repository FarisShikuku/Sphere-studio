// GLSL 460 - Weather Common Definitions
#ifndef WEATHER_COMMON_GLSL
#define WEATHER_COMMON_GLSL

#define WEATHER_CLEAR 0
#define WEATHER_OVERCAST 1
#define WEATHER_RAIN 2
#define WEATHER_HEAVY_RAIN 3
#define WEATHER_SNOW 4
#define WEATHER_BLIZZARD 5
#define WEATHER_FOG 6
#define WEATHER_THUNDERSTORM 7

layout(std140, set = 0, binding = 4) uniform WeatherUniforms {
    vec4 u_WindVector;          // xyz: direction * speed, w: gustFactor
    vec4 u_PrecipitationParams; // x: rainRate, y: snowRate, z: dropletSize, w: splashIntensity
    vec4 u_AtmosphereParams;    // x: fogDensity, y: cloudCoverage, z: wetness, w: puddleDepth
    vec4 u_LightingParams;      // x: lightningIntensity, y: ambientDimming, z: timeSeconds, w: seed
} weather;

#endif // WEATHER_COMMON_GLSL
