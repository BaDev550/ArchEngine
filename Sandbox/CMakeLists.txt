add_executable(Sandbox "main.cpp" "Entities/BasicObject.h" "Entities/PhysicsObject.h" "Entities/Entity_Rat.h" "Entities/EntityProp_Table.h" "Entities/EntityProp_Chair.h" "Entities/Entity_Box.h" "Entities/Entity_Mario.h")
set_target_properties(Sandbox PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
target_link_libraries(Sandbox PRIVATE
    ArchEngine
)
