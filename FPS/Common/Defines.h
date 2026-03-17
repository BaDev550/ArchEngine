#pragma once
#include <iostream>

constexpr std::string GetResourcePath(const std::string& path) { return (RESOURCES_PATH + path); }
constexpr std::string GetEngineResourcePath(const std::string& path) { return (AE_ENGINE_RESOURCES_PATH + path); }
