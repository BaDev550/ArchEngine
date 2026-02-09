#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <map>
#include "ArchEngine/Grapichs/ShaderTypes.h"

namespace ae {
    class ShaderCompiler {
    public:
        static std::vector<char> CompileShaderFileToSpirv(const std::filesystem::path& path, grapichs::CompiledShaderInfo& shaderInfo, bool optimize = false);
        static void CollectReflectionData(grapichs::CompiledShaderInfo& shaderInfo, const void* code, size_t sizeInBytes);
    };
}