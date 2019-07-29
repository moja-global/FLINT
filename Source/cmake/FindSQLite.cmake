SET(SEARCH_PATH_SQLITE "" CACHE PATH "Additional Sqlite search path")
SET(SQLITE_DIR_SEARCH
	$ENV{SQLITE_ROOT}
    ${SEARCH_PATH_SQLITE}
	${SQLITE_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(SQLITE_DIR_SEARCH
			${SQLITE_DIR_SEARCH}
            "C:/Development/sqlite-amalgamation-3260000/"
            "C:/Development/sqlite-amalgamation-3210000/"
            "C:/Development/sqlite-amalgamation-3200100/"
			"C:/Development/sqlite-amalgamation-3200000/"
			"C:/Development/sqlite-amalgamation-3170000/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(SQLITE_DIR_SEARCH
			${SQLITE_DIR_SEARCH}
			"D:/Development/sqlite-amalgamation-3260000/"
			"D:/Development/sqlite-amalgamation-3210000/"
			"D:/Development/sqlite-amalgamation-3200100/"
			"D:/Development/sqlite-amalgamation-3200000/"
			"D:/Development/sqlite-amalgamation-3170000/"
		)
	endif() 
endif()

if (CMAKE_SYSTEM MATCHES "Linux" )
	SET(SQLITE_DIR_SEARCH
		/usr/include/sqlite-autoconf-3270200/
		/usr/local/sqlite-autoconf-3270200/
        /usr/local/sqlite-autoconf-3260000/ 
        /usr/local/sqlite-autoconf-3210000/ 
        /usr/local/sqlite-autoconf-3200100/ 
        /usr/local/sqlite-autoconf-3200000/ 
        /usr/local/sqlite-autoconf-3170000/
		${SQLITE_DIR_SEARCH}
	)
endif(CMAKE_SYSTEM MATCHES "Linux")

find_path(Sqlite_INCLUDE_PATH sqlite3.h
    PATHS
        ${SQLITE_DIR_SEARCH}
)

# handle the QUIETLY and REQUIRED arguments and set SQLITE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sqlite DEFAULT_MSG Sqlite_INCLUDE_PATH)

MARK_AS_ADVANCED(Sqlite_INCLUDE_PATH)
