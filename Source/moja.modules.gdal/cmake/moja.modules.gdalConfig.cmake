include(CMakeFindDependencyMacro)
find_dependency(GDAL REQUIRED)
find_dependency(moja REQUIRED COMPONENTS moja.flint )

if(NOT TARGET moja::moja.modules.gdal)
	include("${CMAKE_CURRENT_LIST_DIR}/moja.modules.gdalTargets.cmake")
endif()