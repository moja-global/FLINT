set(PACKAGE "flint")
set(LIBNAME "moja.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

if(ENABLE_MOJAPY)
    if(CMAKE_SYSTEM MATCHES "Windows")
		set(PYTHON_LIBRARY "c:/python36/libs/python36.lib" CACHE FILEPATH "The file path to the Python library")
		set(PYTHON_INCLUDE_DIR "C:/python36/include" CACHE PATH "The path to the Python headers")
		find_package(PythonLibs 3.0 REQUIRED)
    endif(CMAKE_SYSTEM MATCHES "Windows")

    if (CMAKE_SYSTEM MATCHES "Linux" )
		#set(PYTHON_LIBRARY "/usr/lib/x86_64-linux-gnu/libpython3.4m.so" CACHE FILEPATH "The file path to the Python library")
		#set(PYTHON_INCLUDE_DIR "/usr/include/python3.4" CACHE PATH "The path to the Python headers")
		find_package(PythonLibs)
    endif(CMAKE_SYSTEM MATCHES "Linux")

    if(PYTHONLIBS_FOUND)
		include_directories("${PYTHON_INCLUDE_DIRS}")
    else()
		message(FATAL_ERROR "Unable to find PythonLibs.")
    endif()

endif()

include(${CMAKE_MODULE_PATH}/generate_product_version.cmake) 

if (ENABLE_MOJAPY)
    find_package(Boost COMPONENTS python3 REQUIRED)
endif()

# Version Info
if (MSVC)
  generate_product_version(ProductVersionFiles
    NAME ${LIBNAME}
    FILE_DESCRIPTION "moja global Full Lands Integration Tool"
    VERSION_MAJOR ${MOJA_VERSION_MAJOR}
    VERSION_MINOR ${MOJA_VERSION_MINOR}
    VERSION_PATCH ${MOJA_VERSION_PATCH}
    VERSION_REVISION ${MOJA_VERSION_REVISION}
	COMPANY_NAME "moja global"
#    VERSION_REVISION ${BUILD_ID}
  )
endif ()

# HEADERS AND SOURCE

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/${PACKAGE}/_${PACKAGE}_exports.h)

set(MOJA_INT_GENERATED_HEADERS
	include/moja/${PACKAGE}/_${PACKAGE}_exports.h
)

set(MOJA_FLINT_MODULE_HEADERS
	include/moja/flint/aggregatorfluxstep.h
	include/moja/flint/aggregatorstockstep.h
	include/moja/flint/debugnotificationmodule.h
	include/moja/flint/errorscreenwriter.h
	include/moja/flint/outputerstream.h
	include/moja/flint/outputerstreamflux.h
	include/moja/flint/proxymodule.h
	include/moja/flint/transactionmanageraftersubmitmodule.h
	include/moja/flint/transactionmanagerendofstepmodule.h
    include/moja/flint/writevariablegrid.h
	include/moja/flint/writesystemconfig.h

	include/moja/flint/aggregatorlandunit.h
	include/moja/flint/aggregatorfilewriter.h

	include/moja/flint/testmodule1.h
	include/moja/flint/testmodule2.h
	include/moja/flint/testmodule3.h
)

set(MOJA_FLINT_TRANSFORM_HEADERS
    include/moja/flint/compositetransform.h
	include/moja/flint/itransform.h
	include/moja/flint/iflintdata.h
	include/moja/flint/locationtransform.h
	include/moja/flint/locationidxtransform.h
	include/moja/flint/locationfromflintdatatransform.h
	include/moja/flint/locationidxfromflintdatatransform.h
	include/moja/flint/lookuptransform.h
	include/moja/flint/lookuprandomtransform.h
	include/moja/flint/sqlquerytransform.h
	include/moja/flint/sumpoolstransform.h
)

set(MOJA_FLINT_FLINTDATA_HEADERS
	include/moja/${PACKAGE}/idnamecollection.h
	include/moja/${PACKAGE}/idnamedesccollection.h
    include/moja/${PACKAGE}/operationextradata.h
	include/moja/${PACKAGE}/spatiallocationinfo.h
	include/moja/${PACKAGE}/systeminformation.h
	include/moja/${PACKAGE}/localdomaininformation.h
	include/moja/${PACKAGE}/eventqueue.h
	include/moja/${PACKAGE}/simulationunitdatabase.h
)

set(MOJA_FLINT_HEADERS
	include/moja/flint/aspatiallocaldomaincontroller.h
	include/moja/flint/calendarandeventsequencer.h
	include/moja/flint/calendarandeventflintdatasequencer.h
	include/moja/flint/calendarsequencer.h
	include/moja/flint/equalstepsequencer.h
	include/moja/flint/externalvariable.h
	include/moja/flint/flintdatavariable.h
	include/moja/flint/flintexceptions.h
    include/moja/flint/flux.h
	include/moja/flint/ilandunitcontroller.h
	include/moja/flint/ilandunitdatawrapper.h
	include/moja/flint/ilocaldomaincontroller.h
	include/moja/flint/imodule.h
	include/moja/flint/ivariable.h
	include/moja/flint/landunitcontroller.h
	include/moja/flint/landunitdatawrapper.h
    include/moja/flint/libraryfactory.h
	include/moja/flint/libraryinfobase.h
	include/moja/flint/libraryinfoexternal.h
	include/moja/flint/libraryinfointernal.h
	include/moja/flint/libraryinfomanaged.h
	include/moja/flint/librarymanager.h
	include/moja/flint/librarytype.h
	include/moja/flint/localdomaincontrollerbase.h
	include/moja/flint/modulebase.h
	include/moja/flint/moduleproxybase.h
	include/moja/flint/mojalibapi.h
	include/moja/flint/poolmetadata.h
	include/moja/flint/sequencermodulebase.h
	include/moja/flint/spinuplandunitcontroller.h
	include/moja/flint/threadedaspatiallocaldomaincontroller.h
	include/moja/flint/spatialtiledlocaldomaincontroller.h
	include/moja/flint/itiming.h
	include/moja/flint/timing.h
	include/moja/flint/variable.h
	include/moja/flint/record.h
	include/moja/flint/recordutils.h
	include/moja/flint/recordaccumulator.h
    include/moja/flint/recordaccumulatoruuid.h
	include/moja/flint/recordaccumulatorwithmutex.h
	include/moja/flint/variableandpoolstringbuilder.h
	include/moja/flint/operationdatapackage.h
)

if(ENABLE_MOJAPY)
    set(MOJA_FLINT_HEADERS ${MOJA_FLINT_HEADERS} include/moja/flint/libraryinfopython.h)
endif()

set(MOJA_FLINT_IOPERATION_MANAGER_HEADERS
	include/moja/flint/ioperation.h
	include/moja/flint/ioperationresultflux.h
	include/moja/flint/ioperationresultfluxiterator.h
	include/moja/flint/ioperationresult.h
	include/moja/flint/ioperationtransfer.h
	include/moja/flint/ioperationmanager.h
	include/moja/flint/ipool.h
	include/moja/flint/poolcollection.h
	include/moja/flint/operationresultcollection.h
	include/moja/flint/operationresultfluxcollection.h
)

set(MOJA_FLINT_OPERATION_MANAGER_SIMPLE_HEADERS
    include/moja/flint/externalpoolsimple.h
	include/moja/flint/operationmanagersimple.h
	include/moja/flint/operationresultfluxsimple.h
	include/moja/flint/operationresultfluxiteratorsimple.h
	include/moja/flint/operationproportionalsimple.h
	include/moja/flint/operationstocksimple.h
	include/moja/flint/operationresultsimple.h
	include/moja/flint/operationtransfersimple.h
	include/moja/flint/poolsimple.h
)

set(MOJA_FLINT_OPERATION_MANAGER_SIMPLECACHE_HEADERS
	include/moja/flint/operationmanagersimplecache.h
	include/moja/flint/operationresultfluxsimplecache.h
	include/moja/flint/operationresultfluxiteratorsimplecache.h
	include/moja/flint/operationproportionalsimplecache.h
	include/moja/flint/operationstocksimplecache.h
	include/moja/flint/operationtransfersimplecache.h
	include/moja/flint/poolsimplecache.h
)

set(MOJA_FLINT_OPERATION_MANAGER_UBLAS_HEADERS
	include/moja/flint/matrixublas.h
	include/moja/flint/operationmanagerublas.h
	include/moja/flint/operationresultfluxublas.h
	include/moja/flint/operationresultfluxiteratorublas.h
	include/moja/flint/operationresultublas.h
	include/moja/flint/operationproportionalublas.h
	include/moja/flint/operationstockublas.h
	include/moja/flint/operationtransferublas.h
	include/moja/flint/poolublas.h
)

set(MOJA_FLINT_MODULE_SOURCES
	src/aggregatorfluxstep.cpp
	src/aggregatorstockstep.cpp
	src/debugnotificationmodule.cpp
	src/errorscreenwriter.cpp
	src/outputerstream.cpp
	src/outputerstreamflux.cpp
	src/proxymodule.cpp
	src/transactionmanageraftersubmitmodule.cpp
	src/transactionmanagerendofstepmodule.cpp
	src/writevariablegrid.cpp
	src/writesystemconfig.cpp
	src/aggregatorlandunit.cpp
	src/aggregatorfilewriter.cpp

	src/testmodule1.cpp
	src/testmodule2.cpp
	src/testmodule3.cpp
)

set(MOJA_FLINT_TRANSFORM_SOURCES
	src/compositetransform.cpp
	src/locationtransform.cpp
	src/locationidxtransform.cpp
	src/locationfromflintdatatransform.cpp
	src/locationidxfromflintdatatransform.cpp
	src/lookuptransform.cpp
	src/lookuprandomtransform.cpp
	src/sqlquerytransform.cpp
	src/sumpoolstransform.cpp
)

set(MOJA_FLINT_FLINTDATA_SOURCES
	src/spatiallocationinfo.cpp
	src/eventqueue.cpp
	src/idnamecollection.cpp
)

set(MOJA_FLINT_SOURCES
	src/aspatiallocaldomaincontroller.cpp
	src/calendarandeventsequencer.cpp
	src/calendarandeventflintdatasequencer.cpp
	src/calendarsequencer.cpp
	src/equalstepsequencer.cpp
	src/externalvariable.cpp
	src/flintdatavariable.cpp
	src/landunitcontroller.cpp
	src/landunitdatawrapper.cpp
	src/libraryfactory.cpp
	src/libraryinfobase.cpp
	src/librarymanager.cpp
	src/localdomaincontrollerbase.cpp
	src/modulebase.cpp
	src/poolcollection.cpp
	src/sequencermodulebase.cpp
	src/spinuplandunitcontroller.cpp
	src/threadedaspatiallocaldomaincontroller.cpp
	src/spatialtiledlocaldomaincontroller.cpp
	src/timing.cpp
	src/record.cpp
	src/recordaccumulator.cpp
	src/recordaccumulatorwithmutex.cpp
	src/variableandpoolstringbuilder.cpp
)

set(MOJA_FLINT_OPERATION_MANAGER_SIMPLE_SOURCE
    src/externalpoolsimple.cpp
	src/poolsimple.cpp
	src/operationmanagersimple.cpp
	src/operationproportionalsimple.cpp
	src/operationresultsimple.cpp
	src/operationstocksimple.cpp
)

set(MOJA_FLINT_OPERATION_MANAGER_SIMPLECACHE_SOURCE
	src/poolsimplecache.cpp
	src/operationmanagersimplecache.cpp
	src/operationproportionalsimplecache.cpp
	src/operationresultsimplecache.cpp
	src/operationstocksimplecache.cpp
)

set(MOJA_FLINT_OPERATION_MANAGER_UBLAS_SOURCE
	src/poolublas.cpp
	src/operationmanagerublas.cpp
	src/operationproportionalublas.cpp
	src/operationresultublas.cpp
	src/operationstockublas.cpp
)

source_group("header files\\other"					FILES ${MOJA_FLINT_HEADERS})
source_group("source files\\other"					FILES ${MOJA_FLINT_SOURCES})
source_group("header files\\modules"				FILES ${MOJA_FLINT_MODULE_HEADERS})
source_group("source files\\modules"				FILES ${MOJA_FLINT_MODULE_SOURCES})
source_group("header files\\transforms"				FILES ${MOJA_FLINT_TRANSFORM_HEADERS})
source_group("source files\\transforms"				FILES ${MOJA_FLINT_TRANSFORM_SOURCES})
source_group("header files\\flintdata"				FILES ${MOJA_FLINT_FLINTDATA_HEADERS})
source_group("source files\\flintdata"				FILES ${MOJA_FLINT_FLINTDATA_SOURCES})
source_group("header files\\ioperationmanager"		FILES ${MOJA_FLINT_IOPERATION_MANAGER_HEADERS})
source_group("header files\\op_man_simple"			FILES ${MOJA_FLINT_OPERATION_MANAGER_SIMPLE_HEADERS})
source_group("source files\\op_man_simple"			FILES ${MOJA_FLINT_OPERATION_MANAGER_SIMPLE_SOURCE})
source_group("header files\\op_man_simplecache"		FILES ${MOJA_FLINT_OPERATION_MANAGER_SIMPLECACHE_HEADERS})
source_group("source files\\op_man_simplecache"		FILES ${MOJA_FLINT_OPERATION_MANAGER_SIMPLECACHE_SOURCE})
source_group("header files\\op_man_ublas"			FILES ${MOJA_FLINT_OPERATION_MANAGER_UBLAS_HEADERS})
source_group("source files\\op_man_ublas"			FILES ${MOJA_FLINT_OPERATION_MANAGER_UBLAS_SOURCE})

set (SRCS ${MOJA_FLINT_GENERATED_HEADERS}
		  ${MOJA_FLINT_SOURCES} ${MOJA_FLINT_HEADERS}
		  ${MOJA_FLINT_MODULE_SOURCES} ${MOJA_FLINT_MODULE_HEADERS} 
		  ${MOJA_FLINT_TRANSFORM_SOURCES} ${MOJA_FLINT_TRANSFORM_HEADERS} 
		  ${MOJA_FLINT_FLINTDATA_SOURCES} ${MOJA_FLINT_FLINTDATA_HEADERS} 
		  ${MOJA_FLINT_IOPERATION_MANAGER_HEADERS} 
          ${MOJA_FLINT_OPERATION_MANAGER_SIMPLE_HEADERS} ${MOJA_FLINT_OPERATION_MANAGER_SIMPLE_SOURCE} 
          ${MOJA_FLINT_OPERATION_MANAGER_SIMPLECACHE_HEADERS} ${MOJA_FLINT_OPERATION_MANAGER_SIMPLECACHE_SOURCE} 
		  ${MOJA_FLINT_OPERATION_MANAGER_UBLAS_HEADERS}  ${MOJA_FLINT_OPERATION_MANAGER_UBLAS_SOURCE})


add_library( ${LIBNAME} ${LIB_MODE} ${SRCS} ${ProductVersionFiles})
add_library(Moja::Flint ALIAS ${LIBNAME})

#Set target properties
set_target_properties( ${LIBNAME} 
	PROPERTIES
	VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
	DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_include_directories(${LIBNAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${LIBNAME} 
	PUBLIC
		Moja::Core Moja::FlintConfiguration Moja::DataRepository
	)

if (ENABLE_MOJAPY)
	target_link_libraries(${LIBNAME} 
		PRIVATE
			${PYTHON_LIBRARIES}
		)
endif()    

if (CMAKE_SYSTEM MATCHES "Linux" )
	target_link_libraries( ${LIBNAME} 
		PRIVATE 
			dl
	)
endif(CMAKE_SYSTEM MATCHES "Linux" )

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/MojaFlint)

install(TARGETS ${LIBNAME}
    EXPORT ${LIBNAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#This is required so that the exported target has the name Flint and not ${LIBNAME}
set_target_properties(${LIBNAME} PROPERTIES EXPORT_NAME Flint)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${LIBNAME}-targets
  FILE
    MojaFlintTargets.cmake
  NAMESPACE
    Moja::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MojaFlintConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree
export(EXPORT ${LIBNAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintTargets.cmake NAMESPACE Moja::)

#Register package in user's package registry
export(PACKAGE MojaFlint)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION ${CMAKE_INSTALL_BINDIR} 
		CONFIGURATIONS Debug
		)
endif()

if (ENABLE_TESTS)
  add_subdirectory( tests )
endif ()
