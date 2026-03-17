add_executable(AnimationTesting 
    "main.cpp" 
    "Entities/PhysicsObject.h"
    "Entities/Entity_Box.h" 
    "Entities/Entity_BasicModel.h"
    "Entities/Entity_Archer.h" "Entities/Entity_TPCamera.h")
set_target_properties(AnimationTesting PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
target_link_libraries(AnimationTesting PRIVATE
    ArchEngine
)
