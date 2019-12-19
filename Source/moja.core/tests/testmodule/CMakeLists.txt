set(LIBNAME "testmodule")

set(TestModule_headers
	include/externalmodel.h
)

set(TestModule_sources
	src/externalmodel.cpp
)

set (SRCS ${TestModule_sources} ${TestModule_headers})

add_library( ${LIBNAME} ${LIB_MODE} ${SRCS} )
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

set_target_properties( ${LIBNAME} 
    PROPERTIES
    VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
    DEFINE_SYMBOL TestModule_EXPORTS)

target_include_directories(${LIBNAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${LIBNAME} 
	PUBLIC
		moja::moja.core
)

