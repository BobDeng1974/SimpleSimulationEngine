
include_directories( ${COMMON_SRCS}/dynamics/Molecular )
add_library(Molecular   SHARED Molecular.cpp  $<TARGET_OBJECTS:MolecularEngine> $<TARGET_OBJECTS:DynamicOpt> )
add_library(RigidMol    SHARED RigidMol.cpp   $<TARGET_OBJECTS:MolecularEngine> $<TARGET_OBJECTS:DynamicOpt> )
add_library(ReactiveFF  SHARED ReactiveFF.cpp )


