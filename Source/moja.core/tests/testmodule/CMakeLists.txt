set(LIBNAME "testmodule")

set(TestModule_headers
	include/externalmodel.h
)

set(TestModule_sources
	src/externalmodel.cpp
)

include_directories( include )

set (SRCS ${TestModule_sources} ${TestModule_headers})

add_library( ${LIBNAME} ${LIB_MODE} ${SRCS} )
set_target_properties( ${LIBNAME} 
    PROPERTIES
    VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
    DEFINE_SYMBOL TestModule_EXPORTS)

target_link_libraries( 
	${LIBNAME} 
	moja.core
	)

#install(
#    DIRECTORY include/moja
#    DESTINATION include
#    PATTERN ".svn" EXCLUDE
#    )
#
#install(
#    TARGETS ${LIBNAME}
#    LIBRARY DESTINATION lib${LIB_SUFFIX}
#    ARCHIVE DESTINATION lib${LIB_SUFFIX}
#    RUNTIME DESTINATION bin
#    )