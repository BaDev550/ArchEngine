add_executable(TPS)
set_target_properties(TPS PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GAME_SOURCES})
target_sources(TPS PRIVATE ${GAME_SOURCES})
target_link_libraries(TPS PRIVATE
    ArchEngine
)
target_include_directories(TPS
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
)
target_compile_definitions(TPS PUBLIC 
    RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/"
)
