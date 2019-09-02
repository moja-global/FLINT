include(CMakeFindDependencyMacro)
find_dependency(zipper)
find_dependency(moja REQUIRED COMPONENTS moja.flint )

if(NOT TARGET moja::moja.modules.zipper)
    include("${MojaModulesPoco_CMAKE_DIR}/moja.modules.zipperTargets.cmake")
endif()