set(PACKAGE "test")
set(LIBNAME "moja.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

if(MOJA_STATIC)
	set(CMAKE_CXX_FLAGS_RELEASE "/MT")
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
	add_definitions(-DUSE_STATIC_BOOST)
    set(Boost_USE_STATIC_LIBS ON)
else(MOJA_STATIC)
	add_definitions(-DBOOST_ALL_DYN_LINK)
    set(Boost_USE_STATIC_LIBS OFF)
endif(MOJA_STATIC)

find_package(Boost)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(Turtle)
if(TURTLE_FOUND)
	include_directories(${Turtle_INCLUDE_PATH})
endif()

# SQLite
find_package(SQLite3 REQUIRED)

include_directories( include ../moja.core/include ../moja.flint/include ../moja.datarepository/include)


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
	src/moja.test.cpp)

set(SRCS ${MOJA_Test_sources} ${MOJA_Test_headers})

add_library(${LIBNAME} ${LIB_MODE} ${SRCS})

set_target_properties(${LIBNAME} 
	PROPERTIES
	VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
	DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_link_libraries(
	${LIBNAME} 
	moja.core 
	moja.flint
	moja.flint.configuration
	moja.datarepository
	SQLite::SQLite3
	)

install(DIRECTORY include/moja
		DESTINATION include
		PATTERN ".svn" EXCLUDE)

install(TARGETS ${LIBNAME}
		LIBRARY DESTINATION lib${LIB_SUFFIX}
		ARCHIVE DESTINATION lib${LIB_SUFFIX}
		RUNTIME DESTINATION bin)
