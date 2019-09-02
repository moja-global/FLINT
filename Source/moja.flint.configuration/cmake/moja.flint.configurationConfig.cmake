include(CMakeFindDependencyMacro)

#find_dependency(Poco REQUIRED COMPONENTS Foundation JSON)
find_dependency(moja REQUIRED COMPONENTS moja.core )

if(NOT TARGET moja::moja.flint.configuration)
	include("${CMAKE_CURRENT_LIST_DIR}/moja.flint.configurationTargets.cmake")
endif()