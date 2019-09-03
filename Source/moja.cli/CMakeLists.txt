set(EXENAME "moja.cli")

find_package(Boost COMPONENTS program_options REQUIRED)

set(MOJA_EXE_SRC
	src/moja.cpp
)

add_executable(${EXENAME} ${MOJA_EXE_SRC})

target_link_libraries(${EXENAME}
	PRIVATE
		moja::moja.flint Boost::program_options
)

install(TARGETS ${EXENAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${EXENAME}.pdb 
		DESTINATION ${CMAKE_INSTALL_BINDIR} 
		CONFIGURATIONS Debug
		)
endif()
