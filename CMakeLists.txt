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
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)
set(ASSIMP_GENERATE_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(ASSIMP_CREATE_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/assimp
)
FetchContent_MakeAvailable(assimp)
set(IMGUI_IMPL_OPENGL_LOADER_GLAD OFF CACHE BOOL "" FORCE)
set(IMGUI_IMPL_OPENGL_LOADER_GL3W OFF CACHE BOOL "" FORCE)
set(IMGUI_IMPL_OPENGL_LOADER_GLEW OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui
)
FetchContent_MakeAvailable(imgui)
if(NOT TARGET imgui)
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )
    target_include_directories(imgui PUBLIC 
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)
endif()
FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG master
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/imguizmo
)
FetchContent_MakeAvailable(imguizmo)
if(NOT TARGET imguizmo)
    add_library(imguizmo STATIC
        ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
        ${imguizmo_SOURCE_DIR}/ImSequencer.cpp
        ${imguizmo_SOURCE_DIR}/ImCurveEdit.cpp
        ${imguizmo_SOURCE_DIR}/ImGradient.cpp
    )
    target_include_directories(imguizmo PUBLIC ${imguizmo_SOURCE_DIR})
    target_include_directories(imguizmo PRIVATE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(imguizmo PRIVATE imgui)
endif()
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 0.8.0
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/yaml-cpp
)
FetchContent_MakeAvailable(yaml-cpp)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "" FORCE)
set(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "" FORCE)
set(ENABLE_ALL_WARNINGS OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    joltphysics
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG v5.0.0
    SOURCE_SUBDIR Build
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/joltphysics
)
FetchContent_MakeAvailable(joltphysics)
set(SPIRV_REFLECT_SOURCE "${CMAKE_SOURCE_DIR}/vendor/SPIRV-Reflect/spirv_reflect.c")

add_subdirectory(Engine)
add_subdirectory(Sandbox)
