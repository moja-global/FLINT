set(PACKAGE "test")
set(LIBNAME "moja.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

find_package(Turtle)

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/${PACKAGE}/_${PACKAGE}_exports.h)

set(MOJA_Test_headers
	include/moja/${PACKAGE}/_${PACKAGE}_exports.h
	include/moja/test/mockaspatialtileinfo.h
	include/moja/test/mockaspatialtileinfocollection.h
	include/moja/test/mockblock.h
	include/moja/test/mockdatarepository.h
	include/moja/test/mockexternalvariableconfiguration.h
	include/moja/test/mocklandunitcontroller.h
	include/moja/test/mocklandunitdatawrapper.h
	include/moja/test/mocklibraryconfiguration.h
	include/moja/test/mocklocaldomainconfiguration.h
	include/moja/test/mockmodule.h
	include/moja/test/mockmoduleconfiguration.h
	include/moja/test/mockoperation.h
	include/moja/test/mockoperationmanager.h
	include/moja/test/mockoperationresult.h
	include/moja/test/mockoperationresultcollection.h
	include/moja/test/mockoperationresultflux.h
	include/moja/test/mockoperationresultfluxcollection.h
	include/moja/test/mockoperationresultfluxiterator.h
	include/moja/test/mockpool.h
	include/moja/test/mockpoolcollection.h
	include/moja/test/mockpoolconfiguration.h
	include/moja/test/mockproviderconfiguration.h
	include/moja/test/mockprovidernosqlinterface.h
	include/moja/test/mockproviderrelationalinterface.h
	include/moja/test/mocktile.h
	include/moja/test/mocktiming.h
	include/moja/test/mocktransform.h
	include/moja/test/mocktransformconfiguration.h
	include/moja/test/mockvariable.h
	include/moja/test/mockvariableconfiguration.h
)

set(MOJA_Test_sources
	#src/moja.test.cpp
	)

set(SRCS ${MOJA_Test_sources} ${MOJA_Test_headers})


add_library(${LIBNAME} INTERFACE)
if(MSVC)
    add_custom_target(${LIBNAME}.headers SOURCES ${SRCS})
endif()
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

target_include_directories(${LIBNAME} INTERFACE include/)

target_link_libraries(${LIBNAME} INTERFACE moja::moja.flint)

