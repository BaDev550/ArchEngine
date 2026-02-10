cmake_minimum_required(VERSION 3.16)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
  "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(ArchEngine LANGUAGES C CXX)
find_package(Vulkan REQUIRED)
if(WIN32)
    find_library(Vulkan_SHADERC NAMES shaderc_shared HINTS "$ENV{VULKAN_SDK}/Lib")
    find_library(Vulkan_SHADERC_UTILS NAMES shaderc_util HINTS "$ENV{VULKAN_SDK}/Lib")
else()
    find_library(Vulkan_SHADERC NAMES shaderc_shared shaderc)
    set(Vulkan_SHADERC_UTILS "")
endif()

include(FetchContent)
# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/glfw
)
FetchContent_MakeAvailable(glfw)
set(SPIRV_REFLECT_SOURCE "${CMAKE_SOURCE_DIR}/vendor/SPIRV-Reflect/spirv_reflect.c")

add_subdirectory(Engine)
add_subdirectory(Sandbox)
