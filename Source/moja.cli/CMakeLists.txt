set(EXENAME "moja.cli")

if(MOJA_STATIC)
	set(CMAKE_CXX_FLAGS_RELEASE "/MT")
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
	add_definitions(-DUSE_STATIC_BOOST)
    set(Boost_USE_STATIC_LIBS ON)
else(MOJA_STATIC)
	add_definitions(-DBOOST_ALL_DYN_LINK)
    set(Boost_USE_STATIC_LIBS OFF)
endif(MOJA_STATIC)

find_package(Boost COMPONENTS system thread filesystem date_time chrono program_options log log_setup REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories( include ../moja.core/include ../moja.flint/include ../moja.flint.configuration/include ../moja.datarepository/include)

# Poco
if(Poco_FOUND)
	link_directories(${Poco_BINARY_DIRS})
    include_directories(${Poco_INCLUDE_DIRS})
endif()

set(MOJA_EXE_SRC
	src/moja.cpp
)

add_executable(${EXENAME} ${MOJA_EXE_SRC})

target_link_libraries(
	${EXENAME}
	moja.core 
	moja.flint
    moja.flint.configuration
    ${Boost_LIBRARIES}
    ${SYSLIBS}
    ${Poco_FOUNDATION} 
	${Poco_JSON} 
	)

add_dependencies(${EXENAME} moja.core)
add_dependencies(${EXENAME} moja.flint)
add_dependencies(${EXENAME} moja.flint.configuration)

install(TARGETS ${EXENAME}
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        RUNTIME DESTINATION bin)
