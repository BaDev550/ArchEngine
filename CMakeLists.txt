cmake_minimum_required(VERSION 3.16)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
  "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(ArchEngine LANGUAGES CXX)

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

add_executable(ArchEngine)
set_target_properties(ArchEngine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

file(GLOB_RECURSE ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ENGINE_SOURCES})
target_sources(ArchEngine PRIVATE ${ENGINE_SOURCES})
target_include_directories(ArchEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include
)

target_link_libraries(ArchEngine PRIVATE
    glfw
)
