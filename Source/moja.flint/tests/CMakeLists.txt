set(TESTUNIT "${LIBNAME}.test")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

find_package(Turtle)

configure_file(../../templates/unittestdefinition.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/_unittestdefinition.cpp)

set(TEST_SRCS
    src/_unittestdefinition.cpp
	include/notificationtests.h
	src/notificationtests.cpp
    include/flinttests.h
	include/operationmanagertestutils.h
    src/compositetransformtests.cpp
    src/flinttests.cpp
    src/landunitcontrollertests.cpp
    src/lookuptransformtests.cpp
    src/matrixtestsublas.cpp
	src/operationmanagertestutils.cpp
	src/operationmanagersimpletests.cpp
	src/operationmanagersimplecachetests.cpp
	src/operationmanagerublastests.cpp
    src/sqlquerytransformtests.cpp
	src/recordaccumulatorintegrationtests.cpp
)

add_executable(${TESTUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

target_include_directories(${LIBNAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${TESTUNIT}
	PRIVATE
		Moja::Flint
		Moja::Test
		Boost::unit_test_framework
)

IF (RUN_UNIT_TESTS_ON_BUILD)
    add_custom_command(TARGET ${TESTUNIT}
                       POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
                       ARGS -C $<CONFIGURATION>)
ENDIF ()
