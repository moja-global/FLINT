include(CMakeFindDependencyMacro)

#find_dependency(Poco REQUIRED COMPONENTS Foundation JSON)
find_dependency(moja REQUIRED COMPONENTS moja.core moja.flint.configuration)

if(NOT TARGET moja::moja.datarepository)
	include("${CMAKE_CURRENT_LIST_DIR}/moja.datarepositoryTargets.cmake")
endif()

