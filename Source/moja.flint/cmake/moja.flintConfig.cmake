include(CMakeFindDependencyMacro)
#find_dependency(Boost 1.70 REQUIRED COMPONENTS log)
find_dependency(moja REQUIRED COMPONENTS moja.core moja.flint.configuration moja.datarepository )

if(NOT TARGET moja::moja.flint)
	include("${CMAKE_CURRENT_LIST_DIR}/moja.flintTargets.cmake")
endif()

