cmake_minimum_required(VERSION 3.10.0)
project(moja VERSION 1.0.6 LANGUAGES C CXX)

#turn on using solution folders
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

#turn on parallel builds
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MOJA_VERSION_MAJOR "1")
set(MOJA_VERSION_MINOR "0")
set(MOJA_VERSION_PATCH "0")
set(MOJA_VERSION_REVISION "0")
set(MOJA_VERSION "${MOJA_VERSION_MAJOR}.${MOJA_VERSION_MINOR}.${MOJA_VERSION_PATCH}")

# set the path for custom cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Include some common macros to simpilfy the Moja CMake files
include(MojaMacros)

# Collect the built libraries and include dirs, the will be used to create the moja.cmake file
set(Moja_COMPONENTS "")

# Allow enabling and disabling components
option(ENABLE_MOJA.MODULES.ZIPPER 					"moja.modules.zipper"			OFF)
option(ENABLE_MOJA.MODULES.POCO 					"moja.modules.poco"				OFF)
option(ENABLE_MOJA.MODULES.LIBPQ 					"moja.modules.libpq"			ON)
option(ENABLE_MOJA.MODULES.GDAL 					"moja.modules.gdal"				ON)
option(ENABLE_MOJA.CLI 								"moja.cli"						ON)
option(ENABLE_MOJA.SYSTEMTEST 						"moja.systemtest"				OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "C:/Development/Software/${PROJECT_NAME}" CACHE PATH "..." FORCE)
endif()

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# For Debug build types, append a "d" to the library names.
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Set debug library postfix" FORCE)

option(ENABLE_TESTS				"Set to OFF|ON (default is ON) to control build of moja tests" ON)
option(RUN_UNIT_TESTS_ON_BUILD	"Set to OFF|ON (default is OFF) to control automatic running of moja tests at build time" OFF)
option(MOJA_STATIC				"Set to OFF|ON (default is OFF) to control build of moja as STATIC library" OFF)

# Uncomment from next two lines to force statitc or dynamic library, default is autodetection
if(MOJA_STATIC)
    add_definitions( -DMOJA_STATIC -DMOJA_NO_AUTOMATIC_LIBS)    
    set( LIB_MODE STATIC )
    message(STATUS "Building static libraries")
else(MOJA_STATIC)
    set( LIB_MODE SHARED )
    message(STATUS "Building dynamic libraries")
endif(MOJA_STATIC)

IF (ENABLE_TESTS)
  include(CTest)
  set(BOOST_TEST_REPORTING_LEVEL "SHORT" CACHE STRING "Boost unit test reporting level")
  set_property(CACHE BOOST_TEST_REPORTING_LEVEL PROPERTY STRINGS "SHORT" "DETAILED")
  add_subdirectory(moja.test)
  ENABLE_TESTING()
  message(STATUS "Building with unittests")
ELSE ()
  message(STATUS "Building without tests")
ENDIF ()

# Boost
set(Boost_USE_STATIC_LIBS ${USE_STATIC_BOOST})
set(Boost_USE_MULTITHREADED ON)
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)

if(MOJA_STATIC)
	add_definitions(-DUSE_STATIC_BOOST)
    set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
else(MOJA_STATIC)
	add_definitions(-DBOOST_ALL_DYN_LINK)
    set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_STATIC_RUNTIME OFF)
endif(MOJA_STATIC)

if(USE_STATIC_BOOST)
   message(STATUS "Linking against boost static libraries")
else(USE_STATIC_BOOST)
   message(STATUS "Linking against boost dynamic libraries")
endif(USE_STATIC_BOOST)

# OS Detection
include(CheckCXXCompilerFlag)

message(STATUS "The compiler in use is ${CMAKE_CXX_COMPILER}.")
message(STATUS "The compiler flags in use is ${CMAKE_CXX_FLAGS}.")
if(CMAKE_SYSTEM MATCHES "Windows")
 	add_definitions( -DMOJA_OS_FAMILY_WINDOWS )
 	if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
		message(STATUS "XXX: MS Visual Compiler detected")
		CHECK_CXX_COMPILER_FLAG("/std:c++17" COMPILER_SUPPORTS_CXX17)
		if(COMPILER_SUPPORTS_CXX17)
            message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
		else()
            message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
		endif()
	endif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
endif(CMAKE_SYSTEM MATCHES "Windows")

if (CMAKE_SYSTEM MATCHES "Linux" )
	add_definitions( -DMOJA_OS_FAMILY_UNIX )
	# Standard 'must be' defines
	#add_definitions( -D_XOPEN_SOURCE=500 -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64)
	#set(SYSLIBS pthread dl rt)

	CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

	if(COMPILER_SUPPORTS_CXX17)
		  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
	else()
		  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
	endif()

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
	endif()
endif(CMAKE_SYSTEM MATCHES "Linux" )

add_subdirectory(moja.core)
add_subdirectory(moja.flint)
add_subdirectory(moja.flint.configuration)
add_subdirectory(moja.datarepository)

if(ENABLE_MOJA.MODULES.ZIPPER)
	add_subdirectory(moja.modules.zipper)
	list(APPEND Moja_COMPONENTS "moja.modules.zipper")
endif()

if(ENABLE_MOJA.MODULES.POCO)
	add_subdirectory(moja.modules.poco)
	list(APPEND Moja_COMPONENTS "moja.modules.poco")
endif()

if(ENABLE_MOJA.MODULES.LIBPQ)
	add_subdirectory(moja.modules.libpq)
	list(APPEND Moja_COMPONENTS "moja.modules.libpq")
endif()

if(ENABLE_MOJA.MODULES.GDAL)
	add_subdirectory(moja.modules.gdal)
	list(APPEND Moja_COMPONENTS "moja.modules.gdal")
endif()

if(ENABLE_MOJA.CLI)
	add_subdirectory(moja.cli)
	list(APPEND Moja_COMPONENTS "moja.cli")
endif()

if(ENABLE_MOJA.SYSTEMTEST)
	add_subdirectory(moja.systemtest)
	list(APPEND Moja_COMPONENTS "moja.systemtest")
endif()

foreach(component ${Moja_COMPONENTS})
	message(STATUS "Building: ${component}")
endforeach()


#############################################################
# Uninstall stuff see: http://www.vtk.org/Wiki/CMake_FAQ

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

#############################################################
# Enable packaging

#INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My funky project")
SET(CPACK_PACKAGE_VENDOR "Me, myself, and I")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
# SET(CPACK_PACKAGE_VERSION_MAJOR "1")
# SET(CPACK_PACKAGE_VERSION_MINOR "3")
# SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MOJA_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MOJA_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${MOJA_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_REVISION "${MOJA_VERSION_REVISION}")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Moja Flint Framework")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\moja.global")
  #SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
  #SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/MyExecutable")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

## set(CPACK_PACKAGE_VERSION_MAJOR "${MOJA_VERSION_MAJOR}")
## set(CPACK_PACKAGE_VERSION_MINOR "${MOJA_VERSION_MINOR}")
## set(CPACK_PACKAGE_VERSION_PATCH "${MOJA_VERSION_PATCH}")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${MOJA_VERSION}"
)

#set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    "/.hg/"
    "/.hgignore$"
    "build/"
    "ipch/"
    "/resources/"
    ".sdf$;.suo$;.tss$"
)

include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

#############################################################
# cmake config files

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Set config script install location in a location that find_package() will
# look for, which is different on MS Windows than for UNIX
# Note: also set in MOJA_GENERATE_PACKAGE macro in cmake/MojaMacros.cmake
if (WIN32)
  set(MojaConfigPackageLocation "cmake")
else()
  set(MojaConfigPackageLocation "lib/cmake/${PROJECT_NAME}")
endif()

configure_file(cmake/${PROJECT_NAME}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake" @ONLY)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION
        "${MojaConfigPackageLocation}"
    COMPONENT
        Devel
)

# Install pkg-config files
install(FILES ${MOJA_PKG_CONFIG_FILES}
        DESTINATION lib${LIB_SUFFIX}/pkgconfig)

export(PACKAGE Moja)
		
message(STATUS "CMake ${CMAKE_VERSION} successfully configured ${PROJECT_NAME} using ${CMAKE_GENERATOR} generator")
message(STATUS "Installation target path: ${CMAKE_INSTALL_PREFIX}")

message(STATUS "C_FLAGS:  =${CMAKE_C_FLAGS}")
message(STATUS "CXX_FLAGS:=${CMAKE_CXX_FLAGS}")
