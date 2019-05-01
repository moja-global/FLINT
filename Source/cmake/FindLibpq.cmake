SET(SEARCH_PATH_LIBPQ     "" CACHE PATH "Additional Libpq search path")
SET(SEARCH_PATH_LIB_LIBPQ "" CACHE PATH "Additional Libpq library search path")

SET(LIBPQ_DIR_SEARCH
        $ENV{LIBPQ_ROOT}
    ${SEARCH_PATH_LIBPQ}
        ${LIBPQ_DIR_SEARCH}
)
SET(LIBPQ_LIB_DIR_SEARCH
        $ENV{LIBPQ_ROOT}
    ${SEARCH_PATH_LIB_LIBPQ}
        ${LIBPQ_DIR_SEARCH}
)

if(CMAKE_SYSTEM MATCHES "Windows")
        SET(LIBPQ_LIBNAME "libpq")
        if (EXISTS "C:/")
                SET(LIBPQ_DIR_SEARCH
                        ${LIBPQ_DIR_SEARCH}
						"C:/Program Files/PostgreSQL/11/include"
						"C:/Program Files/PostgreSQL/10/include"
                        "C:/Program Files/PostgreSQL/9.6/include"
                        "C:/Development/PG9.5/include"
                        "C:/Program Files/PostgreSQL/9.5/include"
                        "C:/Program Files/PostgreSQL/9.4/include"
                )
                SET(LIBPQ_LIB_DIR_SEARCH
                        ${LIBPQ_LIB_DIR_SEARCH}
						"C:/Program Files/PostgreSQL/11/lib"
						"C:/Program Files/PostgreSQL/10/lib"
                        "C:/Program Files/PostgreSQL/9.6/lib"
                        "C:/Development/PG9.5/lib"
                        "C:/Program Files/PostgreSQL/9.5/lib"
                        "C:/Program Files/PostgreSQL/9.4/lib"
                )
        endif()
        if (EXISTS "D:/")
                SET(LIBPQ_DIR_SEARCH
                        ${LIBPQ_DIR_SEARCH}
                        D:/Development
						"D:/Program Files/PostgreSQL/10/include"
                        "D:/Program Files/PostgreSQL/9.6/include"
                        "D:/Program Files/PostgreSQL/9.5/include"
                        "D:/Program Files/PostgreSQL/9.4/include"
                )
                SET(LIBPQ_LIB_DIR_SEARCH
                        ${LIBPQ_LIB_DIR_SEARCH}
						"D:/Program Files/PostgreSQL/10/lib"
                        "D:/Program Files/PostgreSQL/9.6/lib"
                        "D:/Program Files/PostgreSQL/9.5/lib"
                        "D:/Program Files/PostgreSQL/9.4/lib"
                )
        endif()
endif()

if (CMAKE_SYSTEM MATCHES "Linux" )
        SET(LIBPQ_LIBNAME "pq")
        SET(LIBPQ_DIR_SEARCH
                ${LIBPQ_DIR_SEARCH}
                /usr/include/postgresql/
                /usr/pgsql-9.5/include
                /usr/include/pgsql95/
        )
        SET(LIBPQ_LIB_DIR_SEARCH
                ${LIBPQ_LIB_DIR_SEARCH}
                /usr/lib/x86_64-linux-gnu/
                /usr/pgsql-9.5/lib
                /usr/lib64/
        )
endif(CMAKE_SYSTEM MATCHES "Linux")

FIND_PATH(LIBPQ_INCLUDE_PATH libpq-fe.h
    PATHS
        ${LIBPQ_DIR_SEARCH}
)

IF(NOT LIBPQ_LIBRARY_DIR)

FIND_LIBRARY(LIBPQ_LIB NAMES ${LIBPQ_LIBNAME}
        PATHS # Look in other places.
          ${LIBPQ_LIB_DIR_SEARCH}
)
GET_FILENAME_COMPONENT(LIBPQ_LIBRARY_DIR ${LIBPQ_LIB} PATH CACHE)

ENDIF(NOT LIBPQ_LIBRARY_DIR)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBPQ DEFAULT_MSG LIBPQ_INCLUDE_PATH)

MARK_AS_ADVANCED(LIBPQ_INCLUDE_PATH)