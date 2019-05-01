set(EXENAME "moja.systemtest")

find_package(Boost COMPONENTS system thread filesystem date_time chrono program_options log log_setup REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(Turtle)
if(TURTLE_FOUND)
	include_directories(${Turtle_INCLUDE_PATH})
endif()

# MongoDBCPlusPlus Driver
find_package(MongoDBCPlusPlusDriver)
if(MONGODBCPLUSPLUSDRIVER_FOUND)
	# MESSAGE("${EXENAME}: ${PACKAGE}: MONGODBCPLUSPLUSDRIVER_FOUND: ${MONGODBCPLUSPLUSDRIVER_FOUND}")

	include_directories(${MONGODBCPLUSPLUSDRIVER_INCLUDE_PATH})
	link_directories(${MONGODBCPLUSPLUSDRIVER_LIBRARY_DIR})
endif()

find_package(Libpq)
if(LIBPQ_FOUND)
	include_directories(${LIBPQ_INCLUDE_PATH})
	link_directories(${LIBPQ_LIBRARY_DIR})
endif()

include_directories( include ../moja.core/include ../moja.flint/include ../moja.datarepository/include ../moja.flint.configuration/include ${Poco_INCLUDE_DIRS})
include_directories(${Sqlite_INCLUDE_PATH})

if(Poco_FOUND)
	link_directories(${Poco_BINARY_DIRS})
endif()

set(MOJA_EXE_HDRS
	include/moja.systemtest.h
)

set(MOJA_EXE_SRC
	src/moja.systemtest.cpp
)

add_definitions(-DBOOST_ALL_DYN_LINK)

add_executable(${EXENAME} ${MOJA_EXE_HDRS} ${MOJA_EXE_SRC})

target_link_libraries(${EXENAME}
	moja.core 
	moja.flint
    moja.flint.configuration
    ${Boost_LIBRARIES}
    ${SYSLIBS}
    ${Poco_FOUNDATION} 
#	${Poco_DATA} 
	${Poco_NET}
	${Poco_JSON} 
#	${Poco_DATA_SQLITE}
#	${Poco_MONGODB}
	${LIBPQ_LIB}
	)

add_dependencies(${EXENAME} moja.core)
add_dependencies(${EXENAME} moja.flint)
add_dependencies(${EXENAME} moja.flint.configuration)

# Set local include path
include_directories(${Poco_INCLUDE_DIRS})
