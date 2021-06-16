set(PACKAGE "core")
set(LIBNAME "moja.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

include(${CMAKE_MODULE_PATH}/generate_product_version.cmake) 

find_package(Poco REQUIRED Foundation JSON)
find_package(Boost COMPONENTS log log_setup REQUIRED)
find_package(fmt REQUIRED)

if (MSVC)
  generate_product_version(ProductVersionFiles
    NAME ${LIBNAME}
    FILE_DESCRIPTION "moja global core"
    VERSION_MAJOR ${MOJA_VERSION_MAJOR}
    VERSION_MINOR ${MOJA_VERSION_MINOR}
    VERSION_PATCH ${MOJA_VERSION_PATCH}
    VERSION_REVISION ${MOJA_VERSION_REVISION}
	COMPANY_NAME "moja global"
#    VERSION_REVISION ${BUILD_ID}
  )
endif ()

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/_${PACKAGE}_exports.h)

set(MOJA_Core_external_folly_headers
	external/folly/CPortability.h
	external/folly/CppAttributes.h
	external/folly/Portability.h
	external/folly/Traits.h
	external/folly/Utility.h
)
set(MOJA_Core_external_folly_hash_headers
	external/folly/hash/SpookyHashV2.h
	external/folly/hash/Hash.h
)
set(MOJA_Core_external_folly_hash_sources
	external/folly/hash/SpookyHashV2.cpp
)
set(MOJA_Core_external_folly_functional_headers
	external/folly/functional/ApplyTuple.h
	external/folly/functional/Invoke.h
)
set(MOJA_Core_external_folly_portability_headers
	external/folly/portability/Config.h
	external/folly/portability/SysTypes.h
)

source_group("Header Files\\external\\folly" FILES ${MOJA_Core_external_folly_headers})
source_group("Header Files\\external\\folly\\hash" FILES ${MOJA_Core_external_folly_hash_headers})
source_group("Source Files\\external\\folly\\hash" FILES ${MOJA_Core_external_folly_hash_sources})
source_group("Header Files\\external\\folly\\functional" FILES ${MOJA_Core_external_folly_functional_headers})
source_group("Header Files\\external\\folly\\portability" FILES ${MOJA_Core_external_folly_portability_headers})

set(FOLLY_SRCS ${MOJA_Core_external_folly_headers} ${MOJA_Core_external_folly_hash_headers} ${MOJA_Core_external_folly_hash_sources} ${MOJA_Core_external_folly_functional_headers} ${MOJA_Core_external_folly_portability_headers})

set(MOJA_Core_external_date_headers
	external/date/date.h
)

source_group("Header Files\\external\\date" FILES ${MOJA_Core_external_date_headers})
set(DATE_SRCS ${MOJA_Core_external_date_headers})

set(MOJA_Core_external_tlx_container_headers
	external/tlx/container/btree.hpp
	external/tlx/container/btree_map.hpp
)
set(MOJA_Core_external_tlx_die_headers
	external/tlx/die/core.hpp
)

set(MOJA_Core_external_tlx_die_sources
	external/tlx/die/core.cpp
)

source_group("Header Files\\external\\tlx\\container" FILES ${MOJA_Core_external_tlx_container_headers})
source_group("Header Files\\external\\tlx\\die" FILES ${MOJA_Core_external_tlx_die_headers})
source_group("Source Files\\external\\tlx\\die" FILES ${MOJA_Core_external_tlx_die_sources})
set(TLX_SRCS ${MOJA_Core_external_tlx_container_headers} ${MOJA_Core_external_tlx_die_headers} ${MOJA_Core_external_tlx_die_sources})

set(MOJA_Core_headers
	include/moja/alignment.h
	include/moja/datetime.h
	include/moja/dispatcher.h
	include/moja/dynamic.h
	include/moja/environment.h
	include/moja/filesystem.h
	include/moja/floatcmp.h
	include/moja/hash.h
	include/moja/logging.h
	include/moja/mathex.h
	include/moja/notificationcenter.h
	include/moja/platform.h
	include/moja/pocojsonutils.h
    include/moja/signals.h
    include/moja/stopwatch.h
    include/moja/timespan.h
	include/moja/types.h
	include/moja/version.h
	include/moja/publisher.h
	include/moja/utility.h
)

set(MOJA_Core_sources
	src/datetime.cpp
	src/environment.cpp
	src/filesystem.cpp
	src/floatcmp.cpp
	src/logging.cpp
	src/mathex.cpp
	src/notificationcenter.cpp
	src/pocojsonutils.cpp
    src/timespan.cpp
	src/publisher.cpp
)

set(SRCS ${MOJA_Core_sources} ${MOJA_Core_headers} ${MOJA_Core_Enum_headers} ${DATE_SRCS} ${FOLLY_SRCS} ${TLX_SRCS})

add_library(${LIBNAME} ${LIB_MODE} ${SRCS} ${ProductVersionFiles})
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

#Set target properties
set_target_properties(${LIBNAME} 
	PROPERTIES
	VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR} 
	OUTPUT_NAME ${LIBNAME}
	DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS
	CXX_EXTENSIONS OFF
)

target_include_directories(${LIBNAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(${LIBNAME} 
	PUBLIC
		BOOST_ALL_DYN_LINK
	PRIVATE 
		FOLLY_NO_CONFIG 
)

target_link_libraries(${LIBNAME} 
	PUBLIC
		Boost::log Boost::log_setup Poco::Foundation Poco::JSON 
		${CXX_FILESYSTEM_LIBRARIES}
		fmt::fmt-header-only
	PRIVATE
		${SYSLIBS}
	)

##############################################
# Installation instructions
include(GNUInstallDirs)
install(DIRECTORY external/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

MOJA_INSTALL(${LIBNAME})
MOJA_GENERATE_PACKAGE(${LIBNAME})

if (ENABLE_TESTS)
	add_subdirectory( tests )
endif ()


