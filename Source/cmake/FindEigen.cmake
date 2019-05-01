SET(SEARCH_PATH_EIGEN "" CACHE PATH "Additional Eigen search path")

SET(EIGEN_DIR_SEARCH
	$ENV{EIGEN_ROOT}
    ${SEARCH_PATH_EIGEN}
	${EIGEN_DIR_SEARCH}
)

if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(EIGEN_DIR_SEARCH
			${EIGEN_DIR_SEARCH}
			"C:/Development/eigen-3.3.4/"
			"C:/Development/eigen-3.3.3/"
			"C:/Development/eigen-3.2.9/"
			"C:/Development/eigen-3.2.8/"
			"C:/Development/eigen-3.2.5/"
			"C:/Development/eigen-3.2.4/"
			"C:/Development/moja.global/Third Party/"
			"C:/Program Files/eigen/"
		)
	endif() 

	if (EXISTS "D:/") 
		SET(EIGEN_DIR_SEARCH
			${EIGEN_DIR_SEARCH}
			"D:/Development/eigen-3.3.4/"
			"D:/Development/eigen-3.3.3/"
			"D:/Development/eigen-3.2.9/"
			"D:/Development/eigen-3.2.8/"
			"D:/Development/eigen-3.2.5/"
			"D:/Development/eigen-3.2.4/"
			"D:/Development/moja.global/Third Party/"
			"D:/Program Files/eigen/"
		)
	endif() 
endif() 

if (CMAKE_SYSTEM MATCHES "Linux" )
	SET(EIGEN_DIR_SEARCH
        ${SEARCH_PATH_EIGEN}
		${EIGEN_DIR_SEARCH}
		/usr/local/include/eigen/
        /usr/include/eigen3/
		/usr/local/include/eigen3/
	)
endif() 

FIND_PATH(Eigen_INCLUDE_PATH Eigen/Eigen
    PATHS
        ${EIGEN_DIR_SEARCH}
)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Eigen DEFAULT_MSG Eigen_INCLUDE_PATH)

MARK_AS_ADVANCED(Eigen_INCLUDE_PATH)