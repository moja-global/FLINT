
include(CMakeFindDependencyMacro)
find_dependency(PostgreSQL REQUIRED)
find_dependency(moja REQUIRED COMPONENTS moja.flint )

if(NOT TARGET moja::moja.modules.libpq)
	include("${CMAKE_CURRENT_LIST_DIR}/moja::moja.modules.libpqTargets.cmake")
endif()
