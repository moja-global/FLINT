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
    src/datetimetests.cpp
    src/dynamictests.cpp
    src/environmenttests.cpp
    src/floatcmptests.cpp
	src/hashingtests.cpp
    src/notificationcentertests.cpp
    src/pathtests.cpp
    src/sharedlibrarytests.cpp
	src/stopwatchtests.cpp
    src/timespantests.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(${TESTUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

target_link_libraries(
	${TESTUNIT}
    ${LIBNAME}
    ${Boost_LIBRARIES}
    ${SYSLIBS}
	)

add_dependencies(${TESTUNIT} moja.core testmodule)
IF (RUN_UNIT_TESTS_ON_BUILD)
add_custom_command(TARGET ${TESTUNIT}
                   POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
                   ARGS -C $<CONFIGURATION>)
ENDIF ()

add_subdirectory(testmodule)
### End unit test ###

### Begin PATH boilerplate for dependent libraries -- adapted from ###
### http://www.cmake.org/pipermail/cmake/2009-May/029464.html      ###
### This allows unit tests to run without having to manually add   ###
### dependent libraries to the system path.                        ###

# Include an entry for each library that needs to be in the system path.
file(TO_NATIVE_PATH "${Boost_LIBRARY_DIR}" boost_lib)

# Determine which environment variable controls the locating of
# DLL's and set that variable.
if(WIN32)
    set(LD_VARNAME "PATH")
    set(LD_PATH "${boost_lib};$ENV{PATH}")

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
