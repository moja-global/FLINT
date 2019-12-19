
include(CMakeFindDependencyMacro)
find_dependency(moja REQUIRED COMPONENTS moja.flint )

if(NOT TARGET moja::moja.test)
	include("${CMAKE_CURRENT_LIST_DIR}/moja.testTargets.cmake")
endif()
