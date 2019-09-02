include(CMakeFindDependencyMacro)
#find_dependency(Poco REQUIRED COMPONENTS Foundation JSON)
find_dependency(Boost COMPONENTS log REQUIRED)

if(NOT TARGET moja::moja.core)
	include("${CMAKE_CURRENT_LIST_DIR}/moja.coreTargets.cmake")
endif()