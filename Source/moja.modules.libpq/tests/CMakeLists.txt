set(TESTUNIT "${LIBNAME}.test")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(../../templates/unittestdefinition.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/_unittestdefinition.cpp)

set(TEST_SRCS
    src/_unittestdefinition.cpp
	src/providerrelationallibpqpostgresqltests.cpp
)

add_executable(${TESTUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

target_link_libraries(${TESTUNIT}
	PRIVATE
		Moja::ModulesLibPQ Boost::unit_test_framework
	)

if(WIN32)
	target_link_libraries(${TESTUNIT}
		PRIVATE
			wsock32 ws2_32
	)
endif()

IF (RUN_UNIT_TESTS_ON_BUILD)
	add_custom_command(TARGET ${TESTUNIT}
					   POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
					   ARGS -C $<CONFIGURATION>)
ENDIF ()
