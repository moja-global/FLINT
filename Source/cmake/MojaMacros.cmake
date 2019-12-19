#===============================================================================
# Macros for Package generation
#
#  MOJA_GENERATE_PACKAGE - Generates *Config.cmake
#    Usage: MOJA_GENERATE_PACKAGE(target_name)
#      INPUT:
#           target_name             the name of the target. e.g. Flint for MojaFlint
#    Example: MOJA_GENERATE_PACKAGE(Flint)
macro(MOJA_GENERATE_PACKAGE target_name)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${target_name}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
export(EXPORT "${target_name}Targets"
  FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${target_name}Targets.cmake"
  NAMESPACE "${PROJECT_NAME}::"
)
configure_file("cmake/${target_name}Config.cmake"
  "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${target_name}Config.cmake"
  @ONLY
)

# Set config script install location in a location that find_package() will
# look for, which is different on MS Windows than for UNIX
# Note: also set in root CMakeLists.txt
if (WIN32)
  set(MojaConfigPackageLocation "cmake")
else()
  set(MojaConfigPackageLocation "lib/cmake/${PROJECT_NAME}")
endif()

install(
    EXPORT "${target_name}Targets"
    FILE "${target_name}Targets.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${MojaConfigPackageLocation}"
    )

install(
    FILES
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${target_name}Config.cmake"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${target_name}ConfigVersion.cmake"
    DESTINATION "${MojaConfigPackageLocation}"
    COMPONENT Devel
    )

endmacro()

#===============================================================================
# Macros for simplified installation
#
#  MOJA_INSTALL - Install the given target
#    Usage: MOJA_INSTALL(target_name)
#      INPUT:
#           target_name             the name of the target. e.g. Flint for MojaFlint
#    Example: MOJA_INSTALL(Flint)
macro(MOJA_INSTALL target_name)
install(
    DIRECTORY include/moja
    DESTINATION include
    COMPONENT Devel
    )

install(
    TARGETS "${target_name}" EXPORT "${target_name}Targets"
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

if (MSVC)
# install the targets pdb
  MOJA_INSTALL_PDB(${target_name})
endif()

endmacro()

#===============================================================================
# Macros for simplified installation of package not following the Moja standard as CppUnit
#
#  SIMPLE_INSTALL - Install the given target
#    Usage: SIMPLE_INSTALL(target_name)
#      INPUT:
#           target_name             the name of the target. e.g. CppUnit
#    Example: SIMPLE_INSTALL(Flint)
macro(SIMPLE_INSTALL target_name)
install(
    DIRECTORY include
    DESTINATION include
    COMPONENT Devel
    PATTERN ".svn" EXCLUDE
    )

install(
    TARGETS "${target_name}" EXPORT "${target_name}Targets"
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

if (MSVC)
# install the targets pdb
  MOJA_INSTALL_PDB(${target_name})
endif()

endmacro()

#  MOJA_INSTALL_PDB - Install the given target's companion pdb file (if present)
#    Usage: MOJA_INSTALL_PDB(target_name)
#      INPUT:
#           target_name             the name of the target. e.g. Flint for MojaFlint
#    Example: MOJA_INSTALL_PDB(Flint)
#
#    This is an internal macro meant only to be used by MOJA_INSTALL.
macro(MOJA_INSTALL_PDB target_name)

    get_property(type TARGET ${target_name} PROPERTY TYPE)
    if ("${type}" STREQUAL "SHARED_LIBRARY" OR "${type}" STREQUAL "EXECUTABLE")
        install(
            FILES $<TARGET_PDB_FILE:${target_name}>
            DESTINATION bin
            COMPONENT Devel
            OPTIONAL
            )
    endif()
endmacro()