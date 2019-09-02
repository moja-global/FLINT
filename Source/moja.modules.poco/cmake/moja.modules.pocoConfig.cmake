
include(CMakeFindDependencyMacro)
find_dependency(Poco REQUIRED COMPONENTS DataSQLite MongoDB)
find_dependency(moja REQUIRED COMPONENTS moja.flint )

if(NOT TARGET moja::moja.modules.poco)
    include("${MojaModulesPoco_CMAKE_DIR}/moja.modules.pocoTargets.cmake")
endif()

