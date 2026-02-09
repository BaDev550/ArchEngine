add_library(ArchEngine)
set_target_properties(ArchEngine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

file(GLOB_RECURSE ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ENGINE_SOURCES})
target_sources(ArchEngine PRIVATE ${ENGINE_SOURCES} ${SPIRV_REFLECT_SOURCE})
target_precompile_headers(ArchEngine PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Source/ArchPch.h>")
target_include_directories(ArchEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/vendor/SPIRV-Reflect
)

target_link_libraries(ArchEngine PUBLIC
    glfw
    Vulkan::Vulkan
    ${Vulkan_SHADERC}
    ${Vulkan_SHADERC_UTILS}
)
