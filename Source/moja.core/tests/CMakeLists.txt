set(TESTUNIT "${LIBNAME}.test")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

find_package(Turtle)

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

add_executable(${TESTUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

target_link_libraries(${TESTUNIT}
	PRIVATE
		moja::moja.core
	    Boost::unit_test_framework
	)

add_dependencies(${TESTUNIT} testmodule)
IF (RUN_UNIT_TESTS_ON_BUILD)
add_custom_command(TARGET ${TESTUNIT}
                   POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
                   ARGS -C $<CONFIGURATION>)
ENDIF ()

add_subdirectory(testmodule)

