set(BENCHUNIT "${LIBNAME}.benchmarks")

#find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(benchmark CONFIG REQUIRED)

#configure_file(../../templates/unittestdefinition.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/_unittestdefinition.cpp)

set(TEST_SRCS
	src/operationmanager.cpp
)

add_executable(${BENCHUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${BENCHUNIT} --result_code=yes --report_level=no)

target_include_directories(${BENCHUNIT}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${BENCHUNIT}
	PRIVATE
        moja::moja.flint
        benchmark::benchmark
)

IF (RUN_UNIT_TESTS_ON_BUILD)
    add_custom_command(TARGET ${BENCHUNIT}
                       POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
                       ARGS -C $<CONFIGURATION>)
ENDIF ()
