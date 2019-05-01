### Unit test ###
set(TESTUNIT "${LIBNAME}.test")

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(Turtle)
if(TURTLE_FOUND)
	include_directories(${Turtle_INCLUDE_PATH})
endif()

configure_file(../../templates/unittestdefinition.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/_unittestdefinition.cpp)

set(TEST_SRCS
    src/_unittestdefinition.cpp
	src/providerrelationallibpqpostgresqltests.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(${TESTUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

if(WIN32)
	target_link_libraries(${TESTUNIT}
			${LIBNAME}
			${Boost_LIBRARIES}
			${SYSLIBS}
			wsock32 
			ws2_32
	)
ELSE()
	target_link_libraries(
		${TESTUNIT}
		${LIBNAME}
		${Boost_LIBRARIES}
		${SYSLIBS}
	)
endif()

add_dependencies(${TESTUNIT} moja.core moja.flint moja.datarepository)
IF (RUN_UNIT_TESTS_ON_BUILD)
	add_custom_command(TARGET ${TESTUNIT}
					   POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
					   ARGS -C $<CONFIGURATION>)
ENDIF ()
### End unit test ###

### Begin PATH boilerplate for dependent libraries -- adapted from ###
### http://www.cmake.org/pipermail/cmake/2009-May/029464.html      ###
### This allows unit tests to run without having to manually add   ###
### dependent libraries to the system path.                        ###

# Include an entry for each library that needs to be in the system path.
find_path(POCO_BIN NAMES PocoFoundation.dll PocoFoundation64.dll
          PATHS ${POCO_PATH}/bin ${POCO_PATH}/bin64
          PATH_SUFFIXES $<CONFIGURATION>)

file(TO_NATIVE_PATH "${Boost_LIBRARY_DIR}" boost_lib)
file(TO_NATIVE_PATH "${POCO_BIN}" poco_bin)

# Determine which environment variable controls the locating of
# DLL's and set that variable.
if(WIN32)
    set(LD_VARNAME "PATH")
    set(LD_PATH "${boost_lib};${poco_bin};$ENV{PATH}")

    # IMPORTANT NOTE: The set_tests_properties(), below, internally
    # stores its name/value pairs with a semicolon delimiter.
    # because of this we must protect the semicolons in the path.
    string(REPLACE ";" "\\;" LD_PATH "${LD_PATH}")
else()
    set(LD_VARNAME "LD_LIBRARY_PATH")
    set(LD_PATH "${boost_lib}:$ENV{LD_LIBRARY_PATH}")
endif()

set_tests_properties(${LIBNAME} PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")
### End PATH boilerplate ###
