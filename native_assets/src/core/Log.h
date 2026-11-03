#pragma once
#include <iostream>
#include <string>
namespace sphere {
class Log {
public:
    static void Init() {}
};
}
#define SPHERE_LOG_INFO(msg)  std::cout << "[INFO]  " << msg << std::endl
#define SPHERE_LOG_WARN(msg)  std::cout << "[WARN]  " << msg << std::endl
#define SPHERE_LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
