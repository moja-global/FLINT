set(EXENAME "moja.systemtest")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Poco REQUIRED Foundation)
find_package(libmongocxx)
find_package(PostgreSQL REQUIRED)

set(MOJA_EXE_HDRS
	include/moja.systemtest.h
)

set(MOJA_EXE_SRC
	src/moja.systemtest.cpp
)

add_executable(${EXENAME} ${MOJA_EXE_HDRS} ${MOJA_EXE_SRC})

target_include_directories(${EXENAME}
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${EXENAME}
	PRIVATE
		moja::moja.flint Boost::system Poco::Foundation PostgreSQL::PostgreSQL
	)

