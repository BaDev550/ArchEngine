add_executable(Sandbox "main.cpp" "BasicObject.h" "PhysicsObject.h")
set_target_properties(Sandbox PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
target_link_libraries(Sandbox PRIVATE
    ArchEngine
)
