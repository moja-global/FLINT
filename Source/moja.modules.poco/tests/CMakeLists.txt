### Unit test ###
set(TESTUNIT "${LIBNAME}.test")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

find_package(Turtle)

configure_file(../../templates/unittestdefinition.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/_unittestdefinition.cpp)

set(TEST_SRCS
    src/_unittestdefinition.cpp
	src/nosqlcollectiontransformtests.cpp
	src/providernosqlpocojsontests.cpp
	src/providernosqlpocomongodbtests.cpp
	src/providerrelationalsqlitetests.cpp
)

set(TEST_TEST_DATA
   data/providernosqlpocomongodbtestsdata_trees.json
)

add_executable(${TESTUNIT} ${TEST_SRCS} ${TEST_TEST_DATA})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

target_link_libraries(${TESTUNIT}
	PRIVATE
		moja::moja.modules.poco Boost::unit_test_framework
)

if(WIN32)
	target_link_libraries(${TESTUNIT}
		PRIVATE
			wsock32 ws2_32
	)
endif()

configure_file(data/providernosqlpocomongodbtestsdata_trees.json providernosqlpocomongodbtestsdata_trees.json) 

IF (RUN_UNIT_TESTS_ON_BUILD)
add_custom_command(TARGET ${TESTUNIT}
                   POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
                   ARGS -C $<CONFIGURATION>)
ENDIF ()
